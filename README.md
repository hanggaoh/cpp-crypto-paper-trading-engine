# C++ Quantitative Cryptocurrency Trading System

A C++20 quantitative trading system for deterministic market replay, strategy research, backtesting, paper trading, risk controls, execution simulation, and portfolio accounting. The execution layer is adapter-based so a separately reviewed live-spot venue can be added without changing strategy or risk code.

The repository currently contains no exchange connection, credential handling, or real-money execution. Live execution is intentionally disabled by default and remains a future integration step.

```text
market data → strategy::IStrategy → engine::RiskEngine → engine::IExecutionVenue → portfolio
```

The included one-tick mean-reversion strategy only exercises order and risk paths. It does not claim profitability and must not be used for investment decisions.

## Run

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/paper_trading_demo
```

To replay a CSV file, pass its path to the demo:

```sh
./build/paper_trading_demo path/to/replay.csv
```

The loader requires a header with `timestamp_ms`, `symbol`, and `mid_price` columns (in any order); other columns are ignored. It rejects empty symbols, non-positive or non-finite prices, malformed CSV quoting, duplicate/missing required headers, and out-of-order timestamps. `timestamp_ms` is a non-negative integer. The replay runner processes valid rows in file order and reports equity, net P&L, return, and maximum drawdown. These are backtest metrics only, not investment advice.

## Roadmap

1. Deterministic replay, simulated fills, risk limits — implemented.
2. Validated CSV loader and basic performance metrics — implemented.
3. Adapter-based execution boundary, order statuses, and disabled-by-default execution gate — implemented.
4. Order lifecycle and deterministic limit-order-book simulation.
5. Optional public-market data and separately reviewed live-spot venue adapters.
