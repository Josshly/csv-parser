#include "../inc/TableCore.hpp"

namespace csv {

namespace detail {

void TableCore::validateRow(const std::vector<std::string>& row) const {
  if (!rows.empty() && row.size() != rows.front().size()) {
    throw std::runtime_error("csv::TableCore::addRow: row size (" +
                             std::to_string(row.size()) +
                             ") does not match existing column count (" +
                             std::to_string(rows.front().size()) + ")");
  }
}

std::vector<std::string> TableCore::getColumn(std::size_t i) const {
  if (!rows.empty() && i >= rows.front().size()) {
    throw std::runtime_error(
        "csv::TableCore::getColumn: column index " + std::to_string(i) +
        " out of range (column count: " + std::to_string(rows.front().size()) +
        ")");
  }
  std::vector<std::string> res;
  res.reserve(rows.size());
  for (const auto& row : rows) {
    res.push_back(row[i]);
  }
  return res;
}

void TableCore::setColumn(std::size_t i, std::vector<std::string> values) {
  if (rows.size() != values.size()) {
    throw std::runtime_error("csv::TableCore::setColumn: values size (" +
                             std::to_string(values.size()) +
                             ") does not match row count (" +
                             std::to_string(rows.size()) + ")");
  }
  if (!rows.empty() && i >= rows.front().size()) {
    throw std::runtime_error(
        "csv::TableCore::setColumn: column index " + std::to_string(i) +
        " out of range (column count: " + std::to_string(rows.front().size()) +
        ")");
  }
  for (std::size_t j = 0; j < rows.size(); ++j) {
    rows[j][i] = std::move(values[j]);
  }
}

void TableCore::addColumn(std::vector<std::string> column) {
  if (rows.size() != column.size()) {
    throw std::runtime_error("csv::TableCore::addColumn: column size (" +
                             std::to_string(column.size()) +
                             ") does not match row count (" +
                             std::to_string(rows.size()) + ")");
  }
  for (std::size_t j = 0; j < rows.size(); ++j) {
    rows[j].push_back(std::move(column[j]));
  }
}

void TableCore::addRow(std::vector<std::string> row) {
  validateRow(row);
  rows.push_back(std::move(row));
}

void TableCore::deleteRow(std::size_t i) {
  if (i >= rows.size()) {
    throw std::out_of_range(
        "csv::TableCore::deleteRow: index " + std::to_string(i) +
        " out of range (row count: " + std::to_string(rows.size()) + ")");
  }
  rows.erase(rows.begin() + i);
}

void TableCore::deleteColumn(std::size_t i) {
  if (rows.empty() || i >= rows.front().size()) {
    throw std::out_of_range("csv::TableCore::deleteColumn: index out of range");
  }
  for (auto& row : rows) {
    row.erase(row.begin() + i);
  }
}

}  // namespace detail

}  // namespace csv