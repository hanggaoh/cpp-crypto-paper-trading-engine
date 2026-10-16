#include <exception>
#include <iomanip>
#include <iostream>
#include <vector>
#include "engine/replay_engine.hpp"
#include "engine/portfolio.hpp"
#include "engine/risk_engine.hpp"
#include "engine/simulated_exchange.hpp"
#include "io/csv_market_replay_loader.hpp"
#include "strategy/strategy.hpp"

int main(int argc, char** argv) {
    std::vector<domain::MarketTick> replay{
        {std::chrono::milliseconds{0}, "BTC-USD", 100000.0},
        {std::chrono::milliseconds{1000}, "BTC-USD", 99200.0},
        {std::chrono::milliseconds{2000}, "BTC-USD", 99600.0},
        {std::chrono::milliseconds{3000}, "BTC-USD", 100250.0},
        {std::chrono::milliseconds{4000}, "BTC-USD", 100900.0},
        {std::chrono::milliseconds{5000}, "BTC-USD", 100100.0},
    };
    try {
        if (argc == 2) replay = io::CsvMarketReplayLoader::load(argv[1]);
        else if (argc != 1) { std::cerr << "usage: paper_trading_demo [replay.csv]\n"; return 2; }
        engine::Portfolio portfolio(10000.0);
        engine::RiskEngine risk({.maximum_position_units = 0.01, .maximum_order_notional_usd = 1500.0});
        engine::SimulatedExchange exchange;
        strategy::OneTickMeanReversionStrategy strategy(0.005);
        const auto result = engine::ReplayEngine::run(replay, strategy, portfolio, risk, exchange);
        std::cout << "time_ms,mid,action,position,cash,equity\n";
        for (const auto& step : result.steps) {
            const char* action = step.action == engine::ReplayAction::buy ? "buy" : step.action == engine::ReplayAction::sell ? "sell" : step.action == engine::ReplayAction::risk_reject ? "risk_reject" : "hold";
            std::cout << step.tick.timestamp.count() << ',' << step.tick.mid_price << ',' << action << ',' << step.position << ',' << std::fixed << std::setprecision(2) << step.cash_usd << ',' << step.equity_usd << '\n';
        }
        const auto& p = result.performance;
        std::cout << "summary,net_pnl_usd=" << p.net_pnl_usd << ",return_pct=" << p.return_pct << ",max_drawdown_usd=" << p.max_drawdown_usd << ",max_drawdown_pct=" << p.max_drawdown_pct << '\n';
    } catch (const std::exception& error) {
        std::cerr << "replay failed: " << error.what() << '\n';
        return 1;
    }
}
