# C++ Crypto Paper-Trading Engine

A C++20 learning project for deterministic market replay, pluggable strategies, risk controls, simulated execution, and portfolio accounting. It is paper trading only: no exchange connection, credentials, or real funds.

```text
market replay → strategy::IStrategy → engine::RiskEngine → simulated exchange → portfolio
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
3. Order lifecycle and limit-order-book simulation.
4. Optional public-market data adapter, still paper-trading only.
