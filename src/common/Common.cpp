#include "Common.h"

namespace gameaf {
ColorRGB::ColorRGB(const std::string& hex)
{
    if (hex.size() == 7 && hex[0] == '#') {  // 格式检查 "#RRGGBB"
        r = std::stoi(hex.substr(1, 2), nullptr, 16);
        g = std::stoi(hex.substr(3, 2), nullptr, 16);
        b = std::stoi(hex.substr(5, 2), nullptr, 16);
    }
}

}  // namespace gameaf