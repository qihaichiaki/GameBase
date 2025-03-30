#pragma once

/**
 * 加载模型并且使用预测动作
 */

#include <torch/script.h>

#include "../Character.h"

// #include <common/Utils.hpp>

class ScriptModule
{
public:
    static bool Load();

    /// @brief 预测动作
    static void PredictAction(Character* src, Character* dst);

private:
    static torch::jit::Module module;
};