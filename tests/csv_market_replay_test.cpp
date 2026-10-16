#include <cassert>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "io/csv_market_replay_loader.hpp"

namespace {
std::filesystem::path fixture_path() { return std::filesystem::current_path() / "csv_market_replay_fixture.csv"; }
void write_fixture(const char* contents) { std::ofstream output(fixture_path()); output << contents; }
bool load_fails(const char* contents) {
    write_fixture(contents);
    try { static_cast<void>(io::CsvMarketReplayLoader::load(fixture_path())); return false; }
    catch (const std::runtime_error&) { return true; }
}
}  // namespace

int main() {
    write_fixture("symbol,extra,mid_price,timestamp_ms\n\"BTC-USD\",ignored,100000.25,0\nBTC-USD,ignored,99900.00,1000\n\n");
    const auto ticks = io::CsvMarketReplayLoader::load(fixture_path());
    assert(ticks.size() == 2);
    assert(ticks[0].symbol == "BTC-USD");
    assert(ticks[0].timestamp.count() == 0);
    assert(ticks[1].mid_price == 99900.0);
    assert(load_fails("timestamp_ms,symbol,mid_price\n0,BTC-USD,0\n"));
    assert(load_fails("timestamp_ms,symbol,mid_price\n0,\"BTC-USD\"unexpected,10\n"));
    assert(load_fails("timestamp_ms,symbol,mid_price\n1000,BTC-USD,10\n999,BTC-USD,10\n"));
    assert(load_fails("timestamp_ms,symbol\n0,BTC-USD\n"));
    std::filesystem::remove(fixture_path());
}
