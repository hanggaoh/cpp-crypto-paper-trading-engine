#pragma once

#include <chrono>
#include <string>

namespace domain {
struct MarketTick { std::chrono::milliseconds timestamp{}; std::string symbol; double mid_price{}; };
enum class Side { buy, sell };
struct OrderRequest { std::string symbol; Side side{}; double quantity{}; };
struct Fill { OrderRequest order; double price{}; double fee{}; };
}  // namespace domain
