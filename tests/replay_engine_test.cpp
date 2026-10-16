#include <cassert>
#include <cmath>
#include <optional>
#include <vector>

#include "engine/replay_engine.hpp"
#include "engine/simulated_exchange.hpp"

namespace {
class BuyOnceStrategy final : public strategy::IStrategy {
public:
    [[nodiscard]] std::optional<domain::OrderRequest> on_tick(const domain::MarketTick& tick, const engine::Portfolio&) override {
        if (sent_) return std::nullopt;
        sent_ = true;
        return domain::OrderRequest{tick.symbol, domain::Side::buy, 1.0};
    }
private: bool sent_{};
};
bool near(double actual, double expected) { return std::abs(actual - expected) < 1e-9; }
}  // namespace

int main() {
    const std::vector<domain::MarketTick> ticks{{std::chrono::milliseconds{0}, "BTC-USD", 100.0},
                                                 {std::chrono::milliseconds{1000}, "BTC-USD", 90.0},
                                                 {std::chrono::milliseconds{2000}, "BTC-USD", 110.0}};
    engine::Portfolio portfolio(1000.0);
    engine::RiskEngine risk({.maximum_position_units = 2.0, .maximum_order_notional_usd = 1000.0});
    engine::SimulatedExchange exchange(0.0, 0.0);
    BuyOnceStrategy strategy;
    const auto result = engine::ReplayEngine::run(ticks, strategy, portfolio, risk, exchange);
    assert(result.steps.size() == 3);
    assert(result.steps[0].action == engine::ReplayAction::buy);
    assert(near(result.performance.net_pnl_usd, 10.0));
    assert(near(result.performance.return_pct, 1.0));
    assert(near(result.performance.max_drawdown_usd, 10.0));
    assert(near(result.performance.max_drawdown_pct, 1.0));

    engine::Portfolio repeat_portfolio(1000.0);
    BuyOnceStrategy repeat_strategy;
    const auto repeat = engine::ReplayEngine::run(ticks, repeat_strategy, repeat_portfolio, risk, exchange);
    assert(repeat.steps[2].equity_usd == result.steps[2].equity_usd);
}
