#include "ResourceCache.h"

namespace gameaf {

// 全局变量

// 字体宽度缓存
static std::unordered_map<std::wstring, std::unordered_map<int, std::unordered_map<wchar_t, int>>>
    G_fontWidthCache;

std::unordered_map<wchar_t, int>& ResourceCacheTool::FontWidthCache(const std::wstring& fontName,
                                                                    int fontSize)
{
    return G_fontWidthCache[fontName][fontSize];
}

}  // namespace gameaf