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

## Roadmap

1. Deterministic replay, simulated fills, risk limits — implemented.
2. CSV loader and performance metrics.
3. Order lifecycle and limit-order-book simulation.
4. Optional public-market data adapter, still paper-trading only.
