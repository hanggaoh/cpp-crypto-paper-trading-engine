#pragma once

#include <cmath>
#include "domain/market.hpp"

namespace engine {
class SimulatedExchange final {
public:
    SimulatedExchange(double fee_bps = 10.0, double slippage_bps = 5.0) : fee_rate_(fee_bps / 10000.0), slippage_rate_(slippage_bps / 10000.0) {}
    [[nodiscard]] domain::Fill execute(const domain::OrderRequest& order, const domain::MarketTick& tick) const {
        const double direction = order.side == domain::Side::buy ? 1.0 : -1.0;
        const double price = tick.mid_price * (1.0 + direction * slippage_rate_);
        return {order, price, std::abs(order.quantity * price) * fee_rate_};
    }
private: double fee_rate_; double slippage_rate_;
};
}  // namespace engine
