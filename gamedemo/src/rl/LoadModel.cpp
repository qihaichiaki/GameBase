#include "LoadModel.h"

#include <common/Log.hpp>
#include <common/Utils.hpp>
#include <filesystem>

torch::jit::Module ScriptModule::module;

bool ScriptModule::Load()
{
    std::string model_path = R"(D:/project/GameBase/gamedemo/script/dqn_model.pt)";
    // 加载 TorchScript 模型
    try {
        // 通过路径加载模型
        torch::NoGradGuard no_grad;
        module = torch::jit::load(model_path);
    } catch (const c10::Error& e) {
        std::cerr << "Error loading the model : " << e.what() << std::endl;
        return false;
    }

    gameaf::log("加载模型成功");
    return true;
}

void ScriptModule::PredictAction(Character* src, Character* dst)
{
    Vector2 dstPos = dst->GetPosition();
    Vector2 srcPos = src->GetPosition();

    static std::vector<float> stateData;
    stateData.push_back(src->hp * 1.0f);
    stateData.push_back(dst->hp * 1.0f);
    stateData.push_back(src->currentAction * 1.0f);
    stateData.push_back(dst->currentAction * 1.0f);
    stateData.push_back(dstPos.X - srcPos.X);
    stateData.push_back(dstPos.Y - srcPos.Y);

    torch::Tensor state_input = torch::tensor(stateData, torch::kFloat32);

    // 添加 batch 维度，因为 DQN 模型通常接收一个 batch 作为输入
    state_input = state_input.unsqueeze(0);  // (1, 6)

    // 通过模型进行预测
    torch::Tensor output = module.forward({state_input}).toTensor();

    // 输出预测结果
    // std::cout << "Model Output: " << output << std::endl;

    // 这是一个Q-learning模型, argmax()来选择最大Q值对应的动作
    int action = output.argmax(1).item<int>();
    // Idle,           // idle动作
    // Defend,         // 尝试格挡
    // Attack,         // 水平攻击
    // AttackUp,       // 向上攻击
    // AttackDown,     // 向下攻击
    // DashAttackAim,  // 进入冲刺攻击瞄准状态
    // Evade,          // 躲闪
    // Dash,           // 地面冲刺
    // Walk,           // 行走
    // AirDash,        // 空中冲刺
    // Jump,           // 尝试跳跃

    gameaf::log("选择动作: {}", action);
    switch (action) {
        case 0:
            src->SwitchState("Idle");
            break;
        case 1:
            src->SwitchState("Defend");
            break;
        case 2:
            src->SwitchState("Attack");
            break;
        case 3:
            src->SwitchState("AttackUp");
            break;
        case 4:
            src->SwitchState("AttackDown");
            break;
        case 5:
            src->SwitchState("DashAttackAim");
            break;
        case 6:
            src->SwitchState("Evade");
            break;
        case 7:
            src->SwitchState("Dash");
            break;
        case 8:
            src->SwitchState("Walk");
            break;
        case 9:
            src->SwitchState("AirDash");
            break;
        case 10:
            src->SwitchState("Jump");
            break;
        default:
            break;
    }

    stateData.clear();
}