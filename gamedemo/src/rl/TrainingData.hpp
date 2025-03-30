#pragma once

#include <fstream>
#include <iomanip>  // std::fixed, std::setprecision
#include <vector>

#include "../Character.h"
#include "TrainingInfo.h"

// 训练数据类
class TrainingData
{
public:
    /// @brief 记录决策
    /// @param src 当前角色
    /// @param dst 目标角色
    /// @param action 当前决策的行为
    /// @param preReward 上一次决策的奖励
    /// @param isEnd 本次训练是否结束, 结束的话
    /// @param overReward 本次训练的结束奖励
    void RecordDecision(Character* src, Character* dst, HornetAction action, float preReward,
                        bool isEnd = false, float overReward = 0.0f)
    {
        TrainingInfo currentdata;

        HornetState currentState;
        currentState.health = src->hp;
        currentState.dstHealth = dst->hp;
        currentState.action = src->currentAction;
        currentState.dstAction = dst->currentAction;
        Vector2 dstPos = dst->GetPosition();
        Vector2 srcPos = src->GetPosition();
        currentState.distance = dstPos.X - srcPos.X;
        currentState.heightDiff = dstPos.Y - srcPos.Y;

        if (!trainingDatas.empty()) {
            auto& preData = trainingDatas.back();
            preData.reward = preReward;
            preData.nextState = currentState;
        }

        currentdata.state = currentState;            // 当前决策的环境状态
        currentdata.action = (int)action;            // 当前决策
        currentdata.horizontalDirection = src->dir;  // 当前决定的水平方向
        currentdata.nextState = {};  // 当前决策的下一个状态(如果存在下一个决策, 由下一个决策填充)
        currentdata.reward = 100.0f;
        if (isEnd) {
            currentdata.reward = overReward;  // 如果没有下一个决策了，则启用结算奖励
        }

        trainingDatas.emplace_back(currentdata);  // 填充训练集
    }

    /// @brief 清理训练数据
    void Clear() { trainingDatas.clear(); }

    /// @brief 上一个的动作
    HornetAction PreAction() const
    {
        if (trainingDatas.empty()) {
            return HornetAction::DoNothing;
        }
        return (HornetAction)trainingDatas.back().action;
    }

    /// @brief 序列化数据
    void Serialization()
    {
        std::ofstream file(TRAIN_PATH "test.data", std::ios::app);
        if (!file.is_open()) {
            return;
        }
        file << std::endl;
        for (const auto& trainingData : trainingDatas) {
            // 保留三位小数
            file << std::fixed << std::setprecision(2) << trainingData.state.health << ' '
                 << trainingData.state.dstHealth << ' ' << trainingData.state.action << ' '
                 << trainingData.state.dstAction << ' ' << trainingData.state.distance << ' '
                 << trainingData.state.heightDiff << ' ' << trainingData.action << ' '
                 << trainingData.horizontalDirection << ' ' << trainingData.reward << ' '
                 << trainingData.nextState.health << ' ' << trainingData.nextState.dstHealth << ' '
                 << trainingData.nextState.action << ' ' << trainingData.nextState.dstAction << ' '
                 << trainingData.nextState.distance << ' ' << trainingData.nextState.heightDiff
                 << "\n";
        }
        file.close();
    }

private:
    std::vector<TrainingInfo> trainingDatas;
};
