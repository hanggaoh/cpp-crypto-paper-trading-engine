#pragma once

#include <algorithm>

namespace engine {
struct PerformanceMetrics {
    double starting_equity_usd{};
    double ending_equity_usd{};
    double net_pnl_usd{};
    double return_pct{};
    double max_drawdown_usd{};
    double max_drawdown_pct{};
};

class PerformanceReporter final {
public:
    explicit PerformanceReporter(double starting_equity_usd)
        : metrics_{.starting_equity_usd = starting_equity_usd, .ending_equity_usd = starting_equity_usd}, peak_equity_(starting_equity_usd) {}

    void observe(double equity_usd) {
        metrics_.ending_equity_usd = equity_usd;
        peak_equity_ = std::max(peak_equity_, equity_usd);
        const double drawdown = peak_equity_ - equity_usd;
        metrics_.max_drawdown_usd = std::max(metrics_.max_drawdown_usd, drawdown);
        if (peak_equity_ > 0.0) metrics_.max_drawdown_pct = std::max(metrics_.max_drawdown_pct, drawdown / peak_equity_ * 100.0);
    }
    [[nodiscard]] PerformanceMetrics metrics() const {
        auto result = metrics_;
        result.net_pnl_usd = result.ending_equity_usd - result.starting_equity_usd;
        if (result.starting_equity_usd != 0.0) result.return_pct = result.net_pnl_usd / result.starting_equity_usd * 100.0;
        return result;
    }
private:
    PerformanceMetrics metrics_;
    double peak_equity_;
};
}  // namespace engine
