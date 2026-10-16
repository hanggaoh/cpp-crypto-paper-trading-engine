#include <cassert>
#include "engine/portfolio.hpp"
#include "engine/risk_engine.hpp"

int main() {
    engine::Portfolio portfolio(5000.0);
    engine::RiskEngine risk({.maximum_position_units = 0.01, .maximum_order_notional_usd = 1000.0});
    const domain::MarketTick tick{{}, "BTC-USD", 100000.0};
    assert(!risk.approve({"BTC-USD", domain::Side::buy, 0.02}, tick, portfolio));
    assert(!risk.approve({"BTC-USD", domain::Side::buy, 0.011}, tick, portfolio));
    assert(risk.approve({"BTC-USD", domain::Side::buy, 0.005}, tick, portfolio));
}
