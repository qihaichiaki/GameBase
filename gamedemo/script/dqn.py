import torch.nn as nn
import torch.optim as optim
import torch
# import numpy as np
from torch.utils.data import DataLoader, TensorDataset

import my_input

# 定义 Q 网络

class DQN(nn.Module):
    def __init__(self, state_dim, action_dim):
        super(DQN, self).__init__()
        self.fc1 = nn.Linear(state_dim, 128)
        self.fc2 = nn.Linear(128, 128)
        self.fc3 = nn.Linear(128, action_dim)

    def forward(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        return self.fc3(x)  # 直接输出 Q 值

def dqn_train(states, actions, rewards, next_states):
    # 组合数据集
    dataset = TensorDataset(states, actions, rewards, next_states)
    data_loader = DataLoader(dataset, batch_size=32, shuffle=True)

    # 训练超参数
    gamma = 0.99   # 折扣因子
    lr = 1e-4      # 学习率
    batch_size = 32
    epochs = 1000

    # 初始化 DQN 网络
    state_dim = states.shape[1]
    action_dim = 13
    q_network = DQN(state_dim, action_dim)
    optimizer = optim.Adam(q_network.parameters(), lr=lr)
    loss_fn = nn.MSELoss()

    for epoch in range(epochs):
        epoch_loss = 0  # 记录本轮总损失
        for batch_states, batch_actions, batch_rewards, batch_next_states in data_loader:
            batch_states = batch_states.clone().detach()
            batch_actions = batch_actions.clone().detach()
            batch_rewards = batch_rewards.clone().detach()
            batch_next_states = batch_next_states.clone().detach()

            q_values = q_network(batch_states)
            q_value = q_values.gather(1, batch_actions.unsqueeze(1)).squeeze(1)

            with torch.no_grad():
                next_q_values = q_network(batch_next_states).max(1)[0]
                target_q_value = torch.clamp(batch_rewards + gamma * next_q_values, -100, 100)

            loss = loss_fn(q_value, target_q_value)
            epoch_loss += loss.item()

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

        if epoch % 50 == 0:
            print(f"Epoch {epoch}/{epochs}, Avg Loss: {epoch_loss / len(data_loader):.4f}")

        # 训练完毕，保存模型
    torch.save(q_network.state_dict(), "dqn_model.pth")
    print(f"训练完成，模型已保存到 'dqn_model.pth'")
    return q_network

# 预测函数
def predict_action(model_path):
    state_dim = 6
    action_dim = 13  # 一共有 13 个动作

    # 加载训练好的模型
    model = DQN(state_dim, action_dim)
    model.load_state_dict(torch.load(model_path))
    model.eval()

    # 处理输入状态
    print("开始输入状态进行预测（输入 'exit' 退出）")

    while True:
        try:
            # 输入状态
            state_input = input(f"请输入一个包含{state_dim}个数值的状态（用空格隔开）: ")
            
            # 判断是否退出
            if state_input.lower() == 'exit':
                print("退出预测过程")
                break

            # 将输入的字符串转换为浮点数列表，并确保它有6个维度
            state = list(map(float, state_input.split()))
            if len(state) != state_dim:
                print(f"错误：输入的状态必须包含 {state_dim} 个数值。")
                continue

            # 处理输入状态
            state_tensor = torch.tensor(state, dtype=torch.float32).unsqueeze(0)

            # 预测 Q 值
            with torch.no_grad():
                q_values = model(state_tensor)
                action = torch.argmax(q_values).item()

            print(f"输入状态: {state}")
            print(f"预测的 Q 值: {q_values.numpy()}")
            print(f"推荐的最佳动作: {action}")

        except ValueError:
            print("错误：请输入有效的数字状态（空格分隔）。")
        except KeyboardInterrupt:
            print("\n检测到中断，退出预测。")
            break


def convert_pt(model_path):
    # 加载训练好的模型
    state_dict = torch.load(model_path, map_location="cpu")  # 确保加载到 CPU

    model = DQN(6, 13)
    model.load_state_dict(state_dict)
    model.eval()
    # 使用 script 方式将模型转换为 TorchScript 格式
    scripted_model = torch.jit.script(model)

    # 保存为 .pt 文件
    scripted_model.save("dqn_model.jit")


if __name__ == "__main__":
    # 读取数据并且转换
    print(torch.__version__)

    # 载入数据
    # file_path = "test.data"
    # dataset = my_input.load_training_data(file_path)

    # # for i in range(min(5, len(dataset))):
    # #     print(dataset[i])
    # # 转换为 PyTorch Tensor

    # states = torch.tensor([data[0] for data in dataset], dtype=torch.float32)
    # actions = torch.tensor([data[1] for data in dataset], dtype=torch.long)
    # rewards = torch.tensor([data[2] for data in dataset], dtype=torch.float32)
    # next_states = torch.tensor([data[3] for data in dataset], dtype=torch.float32)
    # print("样本数据：", states.shape, actions.shape, rewards.shape, next_states.shape)

    # print("开始训练")
    # dqn_train(states, actions, rewards, next_states)
    # convert_pt("dqn_model.pth")  # 转换pt

    # predict_action("dqn_model.pth")
