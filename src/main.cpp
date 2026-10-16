#include <iomanip>
#include <iostream>
#include <vector>
#include "engine/portfolio.hpp"
#include "engine/risk_engine.hpp"
#include "engine/simulated_exchange.hpp"
#include "strategy/strategy.hpp"

int main() {
    const std::vector<domain::MarketTick> replay{
        {{0}, "BTC-USD", 100000.0}, {{1000}, "BTC-USD", 99200.0}, {{2000}, "BTC-USD", 99600.0},
        {{3000}, "BTC-USD", 100250.0}, {{4000}, "BTC-USD", 100900.0}, {{5000}, "BTC-USD", 100100.0},
    };
    engine::Portfolio portfolio(10000.0);
    engine::RiskEngine risk({.maximum_position_units = 0.01, .maximum_order_notional_usd = 1500.0});
    engine::SimulatedExchange exchange;
    strategy::OneTickMeanReversionStrategy strategy(0.005);
    std::cout << "time_ms,mid,action,position,cash,equity\n";
    for (const auto& tick : replay) {
        std::string action = "hold";
        if (const auto proposal = strategy.on_tick(tick, portfolio); proposal) {
            if (const auto approved = risk.approve(*proposal, tick, portfolio); approved) {
                portfolio.apply(exchange.execute(*approved, tick));
                action = approved->side == domain::Side::buy ? "buy" : "sell";
            } else action = "risk_reject";
        }
        std::cout << tick.timestamp.count() << ',' << tick.mid_price << ',' << action << ',' << portfolio.position(tick.symbol) << ',' << std::fixed << std::setprecision(2) << portfolio.cash_usd() << ',' << portfolio.equity_usd(tick) << '\n';
    }
}
