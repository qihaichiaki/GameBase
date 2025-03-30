#pragma once

/**
 * 大黄蜂强化学习训练模块
 */

// State Action Reward

// 定义状态空间
struct HornetState
{
    int health;        // 自身的当前血量
    int dstHealth;     // 目标的当前血量
    int action;        // 自身的当前动作
    int dstAction;     // 目标的当前动作
    float distance;    // 和目标的水平距离
    float heightDiff;  // 和目标的高度差
};

// 定义大黄蜂动作空间
enum class HornetAction {
    Idle,           // idle动作
    Defend,         // 尝试格挡
    Attack,         // 水平攻击
    AttackUp,       // 向上攻击
    AttackDown,     // 向下攻击
    DashAttackAim,  // 进入冲刺攻击瞄准状态
    Evade,          // 躲闪
    Dash,           // 地面冲刺
    Walk,           // 行走
    AirDash,        // 空中冲刺
    Jump,           // 尝试跳跃
    Fall,           // 降落动作
    DoNothing,      // 什么都不做
};

// 抽象的目标动作空间
enum class OpponentAction {
    Idle,       // 待机
    Defend,     // 防御
    Attacking,  // 进攻（包括近战和远程）
    Moving,     // 行走或冲刺
    Airing,     // 空中
};

// 转换函数, 用于将详细的大黄蜂动作空间转换为抽象的动作空间(适用于boss对战时的效果)
inline int HornetToOpponentAction(int hornetAction)
{
    switch (hornetAction) {
        case 2:
        case 3:
        case 4:
        case 5:
            return 2;
            break;
        case 6:
        case 7:
        case 8:
            return 3;
            break;
        case 9:
        case 10:
        case 11:
            return 4;
            break;
        default:
            return hornetAction;
    }
}

struct TrainingInfo
{
    HornetState state;          // 当前环境状态
    int action;                 // 当前选择动作
    float horizontalDirection;  // 水平方向
    float reward;               // 执行当前动作到下一个状态后的 奖励
    HornetState nextState;      // 执行动作后的下一个环境状态
};
