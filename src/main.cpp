#include <curlpp.h>
#include <fmt/core.h>
#include <algorithm>
#include <iterator>
#include <sstream>

template <> struct fmt::formatter<std::optional<std::string>> {
  // Presentation format: 'f' - fixed, 'e' - exponential.
  std::string fallback_value;

  // Parses format specifications of the form ['f' | 'e'].
  constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
    // [ctx.begin(), ctx.end()) is a character range that contains a part of
    // the format string starting from the format specifications to be parsed,
    // e.g. in
    //
    //   fmt::format("{:f} - point of interest", point{1, 2});
    //
    // the range will contain "f} - point of interest". The formatter should
    // parse specifiers until '}' or the end of the range. In this example
    // the formatter should parse the 'f' specifier and return an iterator
    // pointing to '}'.

    // Please also note that this character range may be empty, in case of
    // the "{}" format string, so therefore you should check ctx.begin()
    // for equality with ctx.end().

    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();

    for (;it != end && *it != '}'; ++it)
    {
        fallback_value += *it;
    }

    // Check if reached the end of the range:
    if (it != end && *it != '}') throw std::runtime_error{"invalid format"};

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the point p using the parsed format specification (presentation)
  // stored in this formatter.
  auto format(const std::optional<std::string>& p, format_context& ctx) const -> format_context::iterator {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}", p.value_or(fallback_value));
  }
};

int main() {
    auto url = curlpp::Url("https://fele:pass@stackoverflow.com/questions/39991359/forward-declare-typedef");

    fmt::print("Before: {}\n", url.to_string());
    
    url += "/root?page=1";

    fmt::print("After: {}\n", url.to_string());
    return 0;
}