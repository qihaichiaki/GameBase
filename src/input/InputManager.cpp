#include "InputManager.h"

#include <GameAf.h>
#include <common/Macros.h>
#include <game_object/GameObject.h>

#include <common/Log.hpp>
#include <unordered_map>

namespace gameaf {

// 用于焦点控制，来通知是否需要检查窗口状态, 节省CPU开销
static bool isWindowActive = true;

// 用于检测鼠标移动, 判断是否进行处理鼠标事件
static bool isMouseMove = true;

/// 用于判断对象是否enter/exit
static GameObject* hoverObj = nullptr;

InputManager& InputManager::GetInstance()
{
    static InputManager instance;
    return instance;
}

void InputManager::ProcessInput()
{
#ifdef GAMEAF_USE_EASYX
    if (!isWindowActive) {
        // 失去焦点时检测窗口是否关闭, 如果关闭设置退出状态
        if (!IsWindow(GetHWnd())) GameAf::GetInstance().Exit();
    }
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
                if (hoverObj) hoverObj->OnMouseDown();
                break;
            case WM_LBUTTONUP:
                m_key_state[INPUT_CONTINUE_INDEX].reset((size_t)KeyValue::LButton);
                m_key_state[INPUT_CURRENT_INDEX].set((size_t)KeyValue::LButton);
                if (hoverObj) hoverObj->OnMouseUp();
                break;
            case WM_RBUTTONDOWN:
                m_key_state[INPUT_CONTINUE_INDEX].set((size_t)KeyValue::RButton);
                m_key_state[INPUT_CURRENT_INDEX].set((size_t)KeyValue::RButton);
                // debug : 显示屏幕坐标
                gameaf::log("[debug] {}", m_mousePos);
                break;
            case WM_RBUTTONUP:
                m_key_state[INPUT_CONTINUE_INDEX].reset((size_t)KeyValue::RButton);
                m_key_state[INPUT_CURRENT_INDEX].set((size_t)KeyValue::RButton);
                break;
            case WM_CHAR:
                break;
            case WM_MOUSEMOVE:
                // 鼠标移动
                isMouseMove = true;
                m_mousePos.X = msg.x;
                m_mousePos.Y = msg.y;
                break;
            case WM_MOVE:
                // 窗口移动
                break;
            case WM_ACTIVATE:
                // 窗口焦点切换
                if (msg.wParam == WA_INACTIVE) {
                    // gameaf::log("失去焦点");
                    isWindowActive = false;
                } else {
                    // gameaf::log("聚焦于此");
                    isWindowActive = true;
                }
                break;
            case WM_SIZE:
                // 窗口大小被调整
                break;
            case WM_MOUSEWHEEL:
                // 鼠标滚轮事件
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

void InputManager::ProcessMouseEvent(const std::vector<std::shared_ptr<GameObject>>& objs)
{
    if (!isMouseMove) return;
    GameObject* currentHoverObj = nullptr;
    auto rit = objs.rbegin();
    while (rit != objs.rend()) {
        GameObject* obj = (*rit).get();
        if (obj->ContainsScreenPoint(m_mousePos)) {
            // gameaf::log("鼠标移动到了`{}`上, 坐标为:{},{}", (*rit)->GetName(), m_mousePos.X,
            //             m_mousePos.Y);
            currentHoverObj = obj;
            if (hoverObj != currentHoverObj) {
                if (hoverObj) hoverObj->OnMouseExit();
                hoverObj = obj;
                obj->OnMouseEnter();
            }
            break;
        }
        ++rit;
    }
    if (currentHoverObj != hoverObj) {
        if (hoverObj) hoverObj->OnMouseExit();
        hoverObj = nullptr;
    }

    isMouseMove = false;
}

void InputManager::ClearSenseCache(GameObject* obj)
{
    if (obj == nullptr || obj == hoverObj) {
        if (hoverObj) hoverObj->OnMouseExit();
        hoverObj = nullptr;
    }
}

}  // namespace gameaf