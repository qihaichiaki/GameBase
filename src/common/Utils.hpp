#pragma once

#ifdef _MSC_VER
#include <windows.h>
#endif

/**
 * 放置一些公共的工具函数
 */

#include <string>

namespace gameaf {

inline std::wstring UTF8StrToWStr(const std::string& str)
{
    // 将string 转换为 wstring, 拟增加text 缓存, 切换场景时清除缓存?
#ifdef _MSC_VER
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
#else
    return std::wstring{};
#endif
}
}  // namespace gameaf