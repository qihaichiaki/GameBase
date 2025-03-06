#include "InputManager.h"

#include <common/Macros.h>

#include <common/Log.hpp>
#include <unordered_map>

namespace gameaf {

InputManager& InputManager::GetInstance()
{
    static InputManager instance;
    return instance;
}

void InputManager::ProcessInput()
{
#ifdef GAMEAF_USE_EASYX
    // 按键类型映射
    static std::unordered_map<BYTE, size_t> key_map = {
        {(BYTE)0x30, (size_t)KeyValue::_0},          {(BYTE)0x31, (size_t)KeyValue::_1},
        {(BYTE)0x32, (size_t)KeyValue::_2},          {(BYTE)0x33, (size_t)KeyValue::_3},
        {(BYTE)0x34, (size_t)KeyValue::_4},          {(BYTE)0x35, (size_t)KeyValue::_5},
        {(BYTE)0x36, (size_t)KeyValue::_6},          {(BYTE)0x37, (size_t)KeyValue::_7},
        {(BYTE)0x38, (size_t)KeyValue::_8},          {(BYTE)0x39, (size_t)KeyValue::_9},
        {(BYTE)0x41, (size_t)KeyValue::A},           {(BYTE)0x42, (size_t)KeyValue::B},
        {(BYTE)0x43, (size_t)KeyValue::C},           {(BYTE)0x44, (size_t)KeyValue::D},
        {(BYTE)0x45, (size_t)KeyValue::E},           {(BYTE)0x46, (size_t)KeyValue::F},
        {(BYTE)0x47, (size_t)KeyValue::G},           {(BYTE)0x48, (size_t)KeyValue::H},
        {(BYTE)0x49, (size_t)KeyValue::I},           {(BYTE)0x4a, (size_t)KeyValue::J},
        {(BYTE)0x4b, (size_t)KeyValue::K},           {(BYTE)0x4c, (size_t)KeyValue::L},
        {(BYTE)0x4d, (size_t)KeyValue::M},           {(BYTE)0x4e, (size_t)KeyValue::N},
        {(BYTE)0x4f, (size_t)KeyValue::O},           {(BYTE)0x50, (size_t)KeyValue::P},
        {(BYTE)0x51, (size_t)KeyValue::Q},           {(BYTE)0x52, (size_t)KeyValue::R},
        {(BYTE)0x53, (size_t)KeyValue::S},           {(BYTE)0x54, (size_t)KeyValue::T},
        {(BYTE)0x55, (size_t)KeyValue::U},           {(BYTE)0x56, (size_t)KeyValue::V},
        {(BYTE)0x57, (size_t)KeyValue::W},           {(BYTE)0x58, (size_t)KeyValue::X},
        {(BYTE)0x59, (size_t)KeyValue::Y},           {(BYTE)0x5a, (size_t)KeyValue::Z},
        {(BYTE)0x20, (size_t)KeyValue::Space},       {(BYTE)0x10, (size_t)KeyValue::Shift},
        {VK_OEM_MINUS, (size_t)KeyValue::OEM_MINUS}, {VK_OEM_PLUS, (size_t)KeyValue::OEM_PLUS}};

    static ExMessage msg;

    while (peekmessage(&msg)) {
        // 消息类型映射
        switch (msg.message) {
            case WM_KEYDOWN:
                // 如果是没有重复按下, 那么keydown触发检测
                if (!m_key_state[INPUT_CONTINUE_INDEX][key_map[msg.vkcode]]) {
                    m_key_state[INPUT_CONTINUE_INDEX].set(key_map[msg.vkcode]);
                    m_key_state[INPUT_CURRENT_INDEX].set(key_map[msg.vkcode]);
                }
                break;
            case WM_KEYUP:
                m_key_state[INPUT_CONTINUE_INDEX].reset(key_map[msg.vkcode]);
                m_key_state[INPUT_CURRENT_INDEX].set(key_map[msg.vkcode]);
                break;
            case WM_LBUTTONDOWN:
                m_key_state[INPUT_CONTINUE_INDEX].set((size_t)KeyValue::LButton);
                m_key_state[INPUT_CURRENT_INDEX].set((size_t)KeyValue::LButton);
                break;
            case WM_LBUTTONUP:
                m_key_state[INPUT_CONTINUE_INDEX].reset((size_t)KeyValue::LButton);
                m_key_state[INPUT_CURRENT_INDEX].set((size_t)KeyValue::LButton);
                break;
            case WM_RBUTTONDOWN:
                m_key_state[INPUT_CONTINUE_INDEX].set((size_t)KeyValue::RButton);
                m_key_state[INPUT_CURRENT_INDEX].set((size_t)KeyValue::RButton);
                break;
            case WM_RBUTTONUP:
                m_key_state[INPUT_CONTINUE_INDEX].reset((size_t)KeyValue::RButton);
                m_key_state[INPUT_CURRENT_INDEX].set((size_t)KeyValue::RButton);
                break;
            case WM_CHAR:
                break;
            case WM_MOUSEMOVE:
                // 鼠标移动
                break;
            case WM_MOVE:
                // 窗口移动
                break;
            case WM_ACTIVATE:
                // 窗口焦点切换
                break;
            default:
                gameaf::log("[warning][onInput] 未处理输入信息类别:{}", msg.message);
                break;
        }
    }
    // gameaf::log("MessageType: {}, value: {}", (int)m_message, (int)m_value);
#else
#endif
}

}  // namespace gameaf