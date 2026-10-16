#pragma once

#include <unordered_map>
#include "domain/market.hpp"

namespace engine {
class Portfolio final {
public:
    explicit Portfolio(double starting_cash_usd) : cash_usd_(starting_cash_usd) {}
    void apply(const domain::Fill& fill) {
        const double signed_quantity = fill.order.side == domain::Side::buy ? fill.order.quantity : -fill.order.quantity;
        positions_[fill.order.symbol] += signed_quantity;
        cash_usd_ -= signed_quantity * fill.price + fill.fee;
    }
    [[nodiscard]] double cash_usd() const { return cash_usd_; }
    [[nodiscard]] double position(const std::string& symbol) const {
        const auto it = positions_.find(symbol); return it == positions_.end() ? 0.0 : it->second;
    }
    [[nodiscard]] double equity_usd(const domain::MarketTick& tick) const { return cash_usd_ + position(tick.symbol) * tick.mid_price; }
private:
    double cash_usd_; std::unordered_map<std::string, double> positions_;
};
}  // namespace engine
