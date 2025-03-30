# 读取训练数据
def load_training_data(file_path):
    training_data = []
    
    with open(file_path, "r") as file:
        for line in file:
            line = line.strip()
            if not line:  # 跳过空行
                continue
            
            values = list(map(float, line.split()))
            if len(values) != 15:  # 确保数据格式
                print(f"警告：跳过格式不匹配的行 -> {line}")
                continue
            
            state = values[:6]         # 观测状态
            action = int(values[6])    # 选择的动作
            # direction = values[7]  # 方向
            reward = values[8]         # 奖励
            next_state = values[9:]    # 执行动作后的状态
            
            training_data.append((state, action, reward, next_state))

    return training_data