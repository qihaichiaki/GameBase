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
        {(BYTE)0x57, (size_t)KeyValue::W},     {(BYTE)0x53, (size_t)KeyValue::S},
        {(BYTE)0x41, (size_t)KeyValue::A},     {(BYTE)0x44, (size_t)KeyValue::D},
        {(BYTE)0x20, (size_t)KeyValue::Space}, {(BYTE)0x10, (size_t)KeyValue::Shift},
        {(BYTE)0x4a, (size_t)KeyValue::J},     {(BYTE)0x52, (size_t)KeyValue::R}};

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