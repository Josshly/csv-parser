#ifndef CSV_INC_PARAMETERS_HPP
#define CSV_INC_PARAMETERS_HPP
#include <string>

namespace csv {

template <bool b>
struct firstRowIsHeader {
  static constexpr bool value = b;
};

template <char ch>
struct delimiter {
  static constexpr char value = ch;
};

template <char ch>
struct quoteCharacter {
  static constexpr char value = ch;
};

struct trimNone {
  static void trim(std::string&) noexcept {}
};

struct trimWhitespace {
  static void trim(std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
      s.clear();
      return;
    }
    auto end = s.find_last_not_of(" \t\r\n");
    s = s.substr(start, end - start + 1);
  }
};

}  // namespace csv

#endif