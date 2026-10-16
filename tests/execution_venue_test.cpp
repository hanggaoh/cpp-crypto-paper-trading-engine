#include <cassert>

#include "engine/execution_venue.hpp"
#include "engine/simulated_exchange.hpp"

int main() {
    const domain::MarketTick tick{std::chrono::milliseconds{0}, "BTC-NZD", 100000.0};
    const domain::OrderRequest order{"BTC-NZD", domain::Side::buy, 0.01};
    const engine::SimulatedExchange venue(0.0, 0.0);

    const engine::ExecutionGate disabled;
    const auto blocked = disabled.execute(venue, order, tick);
    assert(blocked.status == domain::OrderStatus::rejected);
    assert(!blocked.fill);

    const engine::ExecutionGate enabled(true);
    const auto filled = enabled.execute(venue, order, tick);
    assert(filled.status == domain::OrderStatus::filled);
    assert(filled.fill);
    assert(filled.fill->price == 100000.0);
}
