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

    /// @brief 设置音频的类别强度
    /// @param id 指定音频id
    /// @param volume [0.0, 1.0]
    bool SetCategoryVolume(const std::string& id, float volume);

    /// @brief 调整音频的类别强度
    /// @param id 指定音频id
    /// @param volume [-1.0, 1.0]
    bool AdjustCategoryVolume(const std::string& id, float delta);

    /// @brief 获取音频类别强度
    /// @param id 指定音频id
    /// @return [0.0, 1.0] 如果id没找到, 返回-1.0
    float GetCategoryVolume(const std::string& id);

    /// @brief 设置音频的相对自身的强度
    /// @param id 指定音频id
    /// @param volume [0.0, 1.0]
    bool SetRelativeVolume(const std::string& id, float volume);

    /// @brief 调整音频相对自身的强度
    /// @param id 指定音频id
    /// @param delta [-1.0, 1.0]
    bool AdjustRelativeVolume(const std::string& id, float delta);

    /// @brief 获取音频相对自身强度
    /// @param id 指定音频id
    /// @return [0.0, 1.0] 如果id没找到, 返回-1.0
    float GetRelativeVolume(const std::string& id);

    /// @brief 设置全局音频强度
    /// @param volume [0.0, 1.0]
    void SetGlobalVolume(float volume);

    /// @brief 调节全局音频强度
    /// @param delta [-1.0, 1.0]
    void AdjustGlobalVolume(float delta);

    /// @brief 获取全局音频强度
    /// @return [0.0, 1.0] 如果id没找到, 返回-1.0
    float GetGlobalVolume();

    /// @brief 获取指定音频的强度
    /// @param id 指定音频
    /// @return [0.0, 1.0] 如果id没找到, 返回-1.0
    float GetVolume(const std::string& id);

    /// @brief 查询对应音频是否在播放
    /// @param id 指定音频
    bool IsPlayingAudio(const std::string& id);

private:
    AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    ~AudioManager();

private:
    struct AudioVolume
    {
        float categoryVolume = 1.0f;  // 类别数值, 可以设置区分不同的音效.
        float relativeVolume = 1.0f;  // 相对变换数值, 一般调节单个音频数值时使用它.
    };

    std::unordered_map<std::string, AudioVolume> m_audioVolumes;  // 音频强度表
    float m_globalVolume = 1.0f;                                  // 全局音频强度
};
}  // namespace gameaf