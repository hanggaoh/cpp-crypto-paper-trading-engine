#pragma once

#include <optional>
#include "domain/market.hpp"
#include "engine/portfolio.hpp"

namespace strategy {
class IStrategy {
public:
    virtual ~IStrategy() = default;
    [[nodiscard]] virtual std::optional<domain::OrderRequest> on_tick(const domain::MarketTick& tick, const engine::Portfolio& portfolio) = 0;
};
// Deterministic teaching strategy only; no claim of profitability or real-fund suitability.
class OneTickMeanReversionStrategy final : public IStrategy {
public:
    explicit OneTickMeanReversionStrategy(double order_size) : order_size_(order_size) {}
    [[nodiscard]] std::optional<domain::OrderRequest> on_tick(const domain::MarketTick& tick, const engine::Portfolio& portfolio) override {
        std::optional<domain::OrderRequest> order;
        if (last_price_ > 0.0 && tick.mid_price < last_price_ * 0.995 && portfolio.position(tick.symbol) < order_size_) order = {tick.symbol, domain::Side::buy, order_size_};
        else if (last_price_ > 0.0 && tick.mid_price > last_price_ * 1.005 && portfolio.position(tick.symbol) > 0.0) order = {tick.symbol, domain::Side::sell, order_size_};
        last_price_ = tick.mid_price;
        return order;
    }
private: double order_size_; double last_price_{};
};
}  // namespace strategy
