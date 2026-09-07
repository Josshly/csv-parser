#ifndef CSV_INC_READER_HPP
#define CSV_INC_READER_HPP
#include <algorithm>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

#include "parameters.hpp"

namespace csv {

template <typename Delimiter = delimiter<','>,
          typename QuoteChar = quoteCharacter<'"'>,
          typename TrimPolicy = trim_none>
class Reader {
 public:
  Reader() : input(nullptr) {}
  explicit Reader(std::istream& is) : input(&is) {}

  bool getRow(std::vector<std::string>& output) {
    if (input == nullptr) {
      throw std::runtime_error("csv::Reader::getRow: no input stream attached");
    }
    output.clear();
    std::string current_field;
    bool in_quotes = false;
    bool got_anything = false;
    char c;
    constexpr char quote_char = QuoteChar::value;
    constexpr char delim = Delimiter::value;

    while (input->get(c)) {
      got_anything = true;
      if (c == '\r') {
        continue;
      }
      if (in_quotes) {
        if (c == quote_char) {
          if (input->peek() == quote_char) {
            input->get(c);
            current_field += quote_char;
          } else {
            in_quotes = false;
          }
        } else {
          current_field += c;
        }
      } else {
        if (c == quote_char) {
          in_quotes = true;
        } else if (c == delim) {
          TrimPolicy::trim(current_field);
          output.push_back(std::move(current_field));
          current_field.clear();
        } else if (c == '\n') {
          TrimPolicy::trim(current_field);
          output.push_back(std::move(current_field));
          return true;
        } else {
          current_field += c;
        }
      }
    }

    if (got_anything) {
      TrimPolicy::trim(current_field);
      output.push_back(std::move(current_field));
      return true;
    }
    return false;
  }

  void parse(std::vector<std::vector<std::string>>& output) {
    std::vector<std::string> row;
    while (getRow(row)) {
      output.push_back(std::move(row));
    }
  }

 private:
  std::istream* input;
};

}  // namespace csv

#endif