#pragma once

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "domain/market.hpp"

namespace io {
class CsvMarketReplayLoader final {
public:
    // Required columns may appear in any order. Extra columns are ignored.
    // Timestamp is milliseconds since the Unix epoch; rows must be non-decreasing by timestamp.
    [[nodiscard]] static std::vector<domain::MarketTick> load(const std::filesystem::path& path) {
        std::ifstream input(path);
        if (!input) throw std::runtime_error("unable to open replay CSV: " + path.string());

        std::string line;
        if (!std::getline(input, line)) throw std::runtime_error("replay CSV is empty: " + path.string());
        auto header = parse_row(line, 1);
        if (!header.empty() && header.front().size() >= 3 &&
            static_cast<unsigned char>(header.front()[0]) == 0xEF &&
            static_cast<unsigned char>(header.front()[1]) == 0xBB &&
            static_cast<unsigned char>(header.front()[2]) == 0xBF) {
            header.front().erase(0, 3);
        }

        const auto columns = required_columns(header);
        std::vector<domain::MarketTick> ticks;
        std::size_t line_number = 1;
        while (std::getline(input, line)) {
            ++line_number;
            if (trim(line).empty()) continue;
            const auto fields = parse_row(line, line_number);
            const auto required_width = std::max({columns.timestamp, columns.symbol, columns.mid_price}) + 1;
            if (fields.size() < required_width) fail(line_number, "fewer fields than the header");

            const auto timestamp = parse_timestamp(fields[columns.timestamp], line_number);
            const auto symbol = trim(fields[columns.symbol]);
            if (symbol.empty()) fail(line_number, "symbol must not be empty");
            const double price = parse_price(fields[columns.mid_price], line_number);
            if (!ticks.empty() && timestamp < ticks.back().timestamp) fail(line_number, "timestamps must be non-decreasing");
            ticks.push_back({timestamp, symbol, price});
        }
        if (ticks.empty()) throw std::runtime_error("replay CSV has no data rows: " + path.string());
        return ticks;
    }

private:
    struct Columns { std::size_t timestamp; std::size_t symbol; std::size_t mid_price; };

    [[nodiscard]] static std::string trim(std::string_view value) {
        const auto first = value.find_first_not_of(" \t\r\n");
        if (first == std::string_view::npos) return {};
        const auto last = value.find_last_not_of(" \t\r\n");
        return std::string(value.substr(first, last - first + 1));
    }
    [[noreturn]] static void fail(std::size_t line, std::string_view reason) {
        throw std::runtime_error("invalid replay CSV at line " + std::to_string(line) + ": " + std::string(reason));
    }
    [[nodiscard]] static std::vector<std::string> parse_row(std::string_view line, std::size_t line_number) {
        std::vector<std::string> fields;
        std::string field;
        bool quoted = false;
        bool quote_closed = false;
        for (std::size_t i = 0; i < line.size(); ++i) {
            const char character = line[i];
            if (quoted) {
                if (character == '"') {
                    if (i + 1 < line.size() && line[i + 1] == '"') { field += '"'; ++i; }
                    else { quoted = false; quote_closed = true; }
                } else field += character;
            } else if (character == ',') {
                fields.push_back(trim(field)); field.clear();
                quote_closed = false;
            } else if (quote_closed) {
                if (character != ' ' && character != '\t' && character != '\r') fail(line_number, "characters after closing quote");
            } else if (character == '"' && field.empty()) quoted = true;
            else field += character;
        }
        if (quoted) fail(line_number, "unterminated quoted field");
        fields.push_back(trim(field));
        return fields;
    }
    [[nodiscard]] static Columns required_columns(const std::vector<std::string>& header) {
        std::unordered_map<std::string, std::size_t> indices;
        for (std::size_t i = 0; i < header.size(); ++i) {
            if (!indices.emplace(header[i], i).second) throw std::runtime_error("replay CSV has duplicate header: " + header[i]);
        }
        const auto lookup = [&indices](const char* name) -> std::size_t {
            const auto it = indices.find(name);
            if (it == indices.end()) throw std::runtime_error(std::string("replay CSV missing required header: ") + name);
            return it->second;
        };
        return {lookup("timestamp_ms"), lookup("symbol"), lookup("mid_price")};
    }
    [[nodiscard]] static std::chrono::milliseconds parse_timestamp(const std::string& value, std::size_t line) {
        std::int64_t parsed{};
        const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), parsed);
        if (error != std::errc{} || end != value.data() + value.size() || parsed < 0) fail(line, "timestamp_ms must be a non-negative integer");
        return std::chrono::milliseconds{parsed};
    }
    [[nodiscard]] static double parse_price(const std::string& value, std::size_t line) {
        try {
            std::size_t consumed{};
            const double parsed = std::stod(value, &consumed);
            if (consumed != value.size() || !std::isfinite(parsed) || parsed <= 0.0) fail(line, "mid_price must be a finite positive number");
            return parsed;
        } catch (const std::invalid_argument&) { fail(line, "mid_price is not a number");
        } catch (const std::out_of_range&) { fail(line, "mid_price is out of range"); }
    }
};
}  // namespace io
