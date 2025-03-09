#pragma once

#include <string>
#include <unordered_map>

/**
 * 资源缓存操作类
 * 当前使用到的:
 *  - 字体宽度缓存
 */

namespace gameaf {
struct ResourceCacheTool
{
    /// @brief 获取字体单词宽度缓存
    /// @param fontName 字体名称
    /// @param fontSize 字体大小
    /// @return <char - width>
    static std::unordered_map<wchar_t, int>& FontWidthCache(const std::wstring& fontName,
                                                            int fontSize);
};
}  // namespace gameaf