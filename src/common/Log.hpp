#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace gameaf {

template <typename... Args>
inline void log(const std::string& fmt, Args&&... args)
{
    std::ostringstream oss;
    format_helper(oss, fmt, std::forward<Args>(args)...);
    std::cout << oss.str() << std::endl;
}

template <typename T, typename... Args>
inline void format_helper(std::ostringstream& ss, const std::string& fmt, T&& value, Args&&... args)
{
    size_t pos = fmt.find("{}");
    if (pos != std::string::npos) {
        ss << fmt.substr(0, pos) << value;
        format_helper(ss, fmt.substr(pos + 2), std::forward<Args>(args)...);
    } else {
        ss << fmt;
    }
}

// 递归终止
inline void format_helper(std::ostringstream& ss, const std::string& fmt) { ss << fmt; }

}  // namespace gameaf
