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

inline std::string GBKStrToUTF8Str(const std::string& gbkStr)
{
#ifdef _MSC_VER
    wchar_t wideMsg[256] = {0};
    char utf8Msg[256] = {0};
    // 先将GBK->Unicode
    int wideLen = MultiByteToWideChar(936, 0, gbkStr.c_str(), (int)gbkStr.size(), wideMsg, 256);
    // check wideLen == 0
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideMsg, 256, utf8Msg, 256, NULL, NULL);
    // check utf8Len == 0
    return utf8Msg;
#else
    return std::string{};
#endif
}
}  // namespace gameaf