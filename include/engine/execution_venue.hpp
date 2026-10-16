#pragma once

#include <optional>
#include <string>

#include "domain/market.hpp"

namespace engine {
struct ExecutionReport {
    domain::OrderStatus status{domain::OrderStatus::rejected};
    std::optional<domain::Fill> fill;
    std::string reason;
};

class IExecutionVenue {
public:
    virtual ~IExecutionVenue() = default;
    [[nodiscard]] virtual ExecutionReport execute(const domain::OrderRequest& order, const domain::MarketTick& tick) const = 0;
};

// A live venue must be explicitly enabled by its owning application. This guard never
// opens a network connection and makes a disabled execution path deterministic in tests.
class ExecutionGate final {
public:
    explicit ExecutionGate(bool enabled = false) : enabled_(enabled) {}
    [[nodiscard]] ExecutionReport execute(const IExecutionVenue& venue, const domain::OrderRequest& order,
                                          const domain::MarketTick& tick) const {
        if (!enabled_) return {.status = domain::OrderStatus::rejected, .fill = std::nullopt, .reason = "execution is disabled"};
        return venue.execute(order, tick);
    }
private:
    bool enabled_;
};
}  // namespace engine
