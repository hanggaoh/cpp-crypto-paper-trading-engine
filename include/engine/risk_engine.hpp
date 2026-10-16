#pragma once

#include <cmath>
#include <optional>
#include "domain/market.hpp"
#include "engine/portfolio.hpp"

namespace engine {
struct RiskLimits { double maximum_position_units{0.02}; double maximum_order_notional_usd{1000.0}; };
class RiskEngine final {
public:
    explicit RiskEngine(RiskLimits limits = {}) : limits_(limits) {}
    [[nodiscard]] std::optional<domain::OrderRequest> approve(const domain::OrderRequest& request, const domain::MarketTick& tick, const Portfolio& portfolio) const {
        if (request.symbol != tick.symbol || request.quantity <= 0.0 || request.quantity * tick.mid_price > limits_.maximum_order_notional_usd) return std::nullopt;
        const double signed_quantity = request.side == domain::Side::buy ? request.quantity : -request.quantity;
        if (std::abs(portfolio.position(request.symbol) + signed_quantity) > limits_.maximum_position_units) return std::nullopt;
        return request;
    }
private: RiskLimits limits_;
};
}  // namespace engine
