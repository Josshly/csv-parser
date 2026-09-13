#ifndef CSV_INC_TABLE_HPP
#define CSV_INC_TABLE_HPP
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Parameters.hpp"
#include "Reader.hpp"
#include "TableCore.hpp"

namespace csv {

template <typename HeaderPolicy = firstRowIsHeader<true>>
class Table : public detail::TableCore {
 public:
  using TableCore::TableCore;

  explicit Table(rowArray values) {
    if (values.empty()) {
      return;
    }
    setHeader(std::move(values.front()));
    values.erase(values.begin());
    for (auto& row : values) {
      addRow(std::move(row));
    }
  }

  std::size_t colCount() const noexcept override { return header.size(); }

  const std::vector<std::string>& getHeader() const noexcept { return header; }

  void setHeader(std::vector<std::string> values) {
    header = std::move(values);
    header_index_map.clear();
    for (std::size_t i = 0; i < header.size(); ++i) {
      auto [it, inserted] = header_index_map.try_emplace(header[i], i);
      if (!inserted) {
        throw std::runtime_error(
            "csv::Table::setHeader: duplicate column name: " + header[i]);
      }
    }
  }

  const std::string& at(std::size_t row_i, const std::string& col_name) const {
    auto it = header_index_map.find(col_name);
    if (it == header_index_map.end()) {
      throw std::runtime_error("csv::Table::at: no such column: " + col_name);
    }
    return TableCore::at(row_i, it->second);
  }

  bool hasColumn(const std::string& name) const noexcept {
    return header_index_map.find(name) != header_index_map.end();
  };

  std::vector<std::string> getColumn(const std::string& name) const {
    auto it = header_index_map.find(name);
    if (it == header_index_map.end()) {
      throw std::runtime_error("csv::Table::getColumn: no such column: " +
                               name);
    }
    return TableCore::getColumn(it->second);
  }

  void setColumn(const std::string& name, std::vector<std::string> values) {
    if (rows.size() != values.size()) {
      throw std::runtime_error("csv::TableCore::setColumn: values size (" +
                               std::to_string(values.size()) +
                               ") does not match row count (" +
                               std::to_string(rows.size()) + ")");
    }
    auto it = header_index_map.find(name);
    if (it == header_index_map.end()) {
      throw std::runtime_error("csv::Table::setColumn: no such column: " +
                               name);
    }
    TableCore::setColumn(it->second, std::move(values));
  }

  void addColumn(const std::string& name, std::vector<std::string> values) {
    auto it = header_index_map.find(name);
    if (it != header_index_map.end()) {
      TableCore::setColumn(it->second, std::move(values));
      return;
    }
    TableCore::addColumn(std::move(values));
    header.push_back(name);
    header_index_map[name] = header.size() - 1;
  }

  void deleteColumn(const std::string& name) {
    auto it = header_index_map.find(name);
    if (it == header_index_map.end()) {
      throw std::runtime_error("csv::Table::deleteColumn: no such column: " +
                               name);
    }
    std::size_t removed_index = it->second;
    TableCore::deleteColumn(removed_index);
    header.erase(header.begin() + removed_index);
    header_index_map.erase(it);
    for (auto& [col_name, idx] : header_index_map) {
      if (idx > removed_index) {
        --idx;
      }
    }
  }

  Table filter(const std::string& col_name, const std::string& value) const {
    auto it = header_index_map.find(col_name);
    if (it == header_index_map.end()) {
      throw std::runtime_error("csv::Table::filter: no such column: " +
                               col_name);
    }
    std::size_t col_index = it->second;
    rowArray matched;
    for (const auto& row : rows) {
      if (row[col_index] == value) {
        matched.push_back(row);
      }
    }
    return Table(header, std::move(matched));
  }

  Table filterAll(const std::vector<std::string>& col_names,
                  const std::vector<std::string>& values) const {
    if (col_names.size() != values.size()) {
      throw std::runtime_error("csv::Table::filter: col_names size (" +
                               std::to_string(col_names.size()) +
                               ") does not match values size (" +
                               std::to_string(values.size()) + ")");
    }
    std::vector<std::size_t> col_indices;
    col_indices.reserve(col_names.size());
    for (const auto& name : col_names) {
      auto it = header_index_map.find(name);
      if (it == header_index_map.end()) {
        throw std::runtime_error("csv::Table::filter: no such column: " + name);
      }
      col_indices.push_back(it->second);
    }

    rowArray matched;
    for (const auto& row : rows) {
      bool all_match = true;
      for (std::size_t i = 0; i < col_indices.size(); ++i) {
        if (row[col_indices[i]] != values[i]) {
          all_match = false;
          break;
        }
      }
      if (all_match) {
        matched.push_back(row);
      }
    }
    return Table(header, matched);
  }

 protected:
  void validateRow(const std::vector<std::string>& row) const override {
    if (row.size() != header.size()) {
      throw std::runtime_error("csv::Table::addRow: row size (" +
                               std::to_string(row.size()) +
                               ") does not match header size (" +
                               std::to_string(header.size()) + ")");
    }
  }

  Table(std::vector<std::string> header_input, rowArray rows_input) {
    setHeader(std::move(header_input));
    for (auto& row : rows_input) {
      addRow(std::move(row));
    }
  }

 private:
  std::vector<std::string> header;
  std::unordered_map<std::string, std::size_t> header_index_map;
};

template <>
class Table<firstRowIsHeader<false>> : public detail::TableCore {
 public:
  Table() = default;
  explicit Table(rowArray values) : TableCore(std::move(values)) {}
};

template <typename HeaderPolicy = firstRowIsHeader<true>,
          typename Delimiter = delimiter<','>,
          typename QuoteChar = quoteCharacter<'"'>,
          typename TrimPolicy = trimNone>
Table<HeaderPolicy> readCSV(const std::string& filename) {
  std::ifstream input(filename);
  if (!input) {
    throw std::runtime_error("csv::readCSV: cannot open file: " + filename);
  }
  Reader<Delimiter, QuoteChar, TrimPolicy> reader(&input);
  std::vector<std::vector<std::string>> rows;
  reader.parse(rows);
  return Table<HeaderPolicy>(std::move(rows));
}

}  // namespace csv

#endif