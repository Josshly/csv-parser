#ifndef CSV_INC_TABLE_CORE_HPP
#define CSV_INC_TABLE_CORE_HPP
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace csv {

namespace detail {

class TableCore {
 protected:
  using rowArray = std::vector<std::vector<std::string>>;

 protected:
  TableCore() = default;
  explicit TableCore(rowArray values) : rows(std::move(values)) {}

 public:
  virtual ~TableCore() = default;

 public:
  std::size_t rowCount() const noexcept { return rows.size(); }

  virtual std::size_t colCount() const noexcept {
    return rows.empty() ? 0 : rows.front().size();
  }

  bool empty() const noexcept { return rows.empty(); }

  const std::string& at(std::size_t row_i, std::size_t col_i) const {
    return rows.at(row_i).at(col_i);
  }

  const std::vector<std::string>& operator[](std::size_t i) const noexcept {
    return rows[i];
  }

  std::vector<std::string>& operator[](std::size_t i) noexcept {
    return rows[i];
  }

  const std::vector<std::string>& row(std::size_t i) const noexcept {
    return rows[i];
  }

  std::vector<std::string> getColumn(std::size_t i) const;
  void setColumn(std::size_t i, std::vector<std::string> values);
  void addColumn(std::vector<std::string> column);
  void addRow(std::vector<std::string> row);
  void deleteColumn(std::size_t i);
  void deleteRow(std::size_t i);

  rowArray::iterator begin() noexcept { return rows.begin(); }
  rowArray::const_iterator begin() const noexcept { return rows.begin(); }
  rowArray::const_iterator cbegin() const noexcept { return rows.cbegin(); }
  rowArray::iterator end() noexcept { return rows.end(); }
  rowArray::const_iterator end() const noexcept { return rows.end(); }
  rowArray::const_iterator cend() const noexcept { return rows.cend(); }
  rowArray::reverse_iterator rbegin() noexcept { return rows.rbegin(); }
  rowArray::const_reverse_iterator rbegin() const noexcept {
    return rows.rbegin();
  }
  rowArray::const_reverse_iterator crbegin() const noexcept {
    return rows.crbegin();
  }
  rowArray::reverse_iterator rend() noexcept { return rows.rend(); }
  rowArray::const_reverse_iterator rend() const noexcept { return rows.rend(); }
  rowArray::const_reverse_iterator crend() const noexcept {
    return rows.crend();
  }

 protected:
  virtual void validateRow(const std::vector<std::string>& row) const;

 protected:
  rowArray rows;
};

}  // namespace detail

}  // namespace csv

#endif