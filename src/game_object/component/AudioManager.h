#pragma once

#include <string>
#include <unordered_map>

/**
 * easyx - mci 是单线程版的, 在哪个线程加载, 才能在哪个线程上进行播放
 */

namespace gameaf {

constexpr static int MIN_AUDIOVOLUME = 0;
constexpr static int MAX_AUDIOVOLUME = 100;

class AudioManager
{
public:
    static AudioManager& GetInstance();

public:
    /// @brief 加载音频
    /// @param path 资源路径
    /// @param id 资源id,id不可为空
    bool OpenAudio(const std::string& path, const std::string& id);

    /// @brief 播放音频
    /// @param id 音频id
    /// @param isLoop 是否循环
    bool PlayAudio(const std::string& id, bool isLoop = false);

    /// @brief 停止音频
    /// @param id 音频id
    bool StopAudio(const std::string& id);

    /// @brief 关闭音频
    /// @param id 音频id
    bool CloseAudio(const std::string& id);

    /// @brief 暂停播放音频
    bool PauseAudio(const std::string& id);

    /// @brief 恢复播放音频
    bool ResumeAudio(const std::string& id);

    /// @brief 设置音频大小
    /// @param volume [0, 100] 超出范围者取最大或者最小值
    /// @param id 指定音频调节, 如果为空字符串表示设置全局音频大小
    bool SetAudioVolume(int volume, const std::string& id = "");

    /// @brief 改变音频大小
    /// @param mod 修改的大小, 可负可正
    /// @param id 指定音频调节, 如果为空字符串表示调节全局音频大小
    bool ChangeAudioVolume(int mod, const std::string& id = "");

    /// @brief 获取音频大小
    /// @param id 指定音频获取, 如果为空字符串表示获取全局音频大小
    /// @return [0, 100]的音量大小, 如果id不存在则返回-1
    int GetAudioVolume(const std::string& id = "");

private:
    AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    ~AudioManager();

private:
    std::unordered_map<std::string, int> m_audioVolumes;  // 音频强度表
    int m_globalVolume = MAX_AUDIOVOLUME;                 // 全局音频强度
    int m_minVolume = MAX_AUDIOVOLUME;                    // 最小音频强度
};
}  // namespace gameaf