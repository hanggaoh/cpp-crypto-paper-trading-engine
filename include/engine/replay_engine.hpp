#pragma once

#include <cmath>
#include <map>
#include <stdexcept>
#include <vector>

#include "engine/performance_reporter.hpp"
#include "engine/execution_venue.hpp"
#include "engine/risk_engine.hpp"
#include "strategy/strategy.hpp"

namespace engine {
enum class ReplayAction { hold, buy, sell, risk_reject };
struct ReplayStep { domain::MarketTick tick; ReplayAction action{}; double position{}; double cash_usd{}; double equity_usd{}; };
struct ReplayResult { std::vector<ReplayStep> steps; PerformanceMetrics performance; };

class ReplayEngine final {
public:
    [[nodiscard]] static ReplayResult run(const std::vector<domain::MarketTick>& ticks, strategy::IStrategy& strategy,
                                          Portfolio& portfolio, const RiskEngine& risk, const IExecutionVenue& venue) {
        if (ticks.empty()) throw std::invalid_argument("market replay requires at least one tick");
        validate(ticks);
        PerformanceReporter reporter(portfolio.cash_usd());
        std::map<std::string, double> marks;
        ReplayResult result;
        result.steps.reserve(ticks.size());
        for (const auto& tick : ticks) {
            marks[tick.symbol] = tick.mid_price;
            ReplayAction action = ReplayAction::hold;
            if (const auto proposal = strategy.on_tick(tick, portfolio); proposal) {
                if (const auto approved = risk.approve(*proposal, tick, portfolio); approved) {
                    const auto report = venue.execute(*approved, tick);
                    if (report.status == domain::OrderStatus::filled && report.fill) {
                        portfolio.apply(*report.fill);
                        action = approved->side == domain::Side::buy ? ReplayAction::buy : ReplayAction::sell;
                    } else action = ReplayAction::risk_reject;
                } else action = ReplayAction::risk_reject;
            }
            double equity = portfolio.cash_usd();
            for (const auto& [symbol, price] : marks) equity += portfolio.position(symbol) * price;
            reporter.observe(equity);
            result.steps.push_back({tick, action, portfolio.position(tick.symbol), portfolio.cash_usd(), equity});
        }
        result.performance = reporter.metrics();
        return result;
    }
private:
    static void validate(const std::vector<domain::MarketTick>& ticks) {
        for (std::size_t i = 0; i < ticks.size(); ++i) {
            if (ticks[i].symbol.empty() || !std::isfinite(ticks[i].mid_price) || ticks[i].mid_price <= 0.0)
                throw std::invalid_argument("market replay contains an invalid tick");
            if (i > 0 && ticks[i].timestamp < ticks[i - 1].timestamp)
                throw std::invalid_argument("market replay timestamps must be non-decreasing");
        }
    }
};
}  // namespace engine
