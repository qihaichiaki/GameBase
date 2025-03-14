#include "AudioManager.h"

#include <common/Macros.h>

#include <algorithm>
#include <common/Log.hpp>
#include <common/Utils.hpp>

#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
#pragma comment(lib, "WINMM.lib")  // 音频播放
#endif

namespace gameaf {

AudioManager::AudioManager() {}

AudioManager::~AudioManager()
{
    for (const auto [id, _] : m_audioVolumes) {
        CloseAudio(id);
    }
}

AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}

bool AudioManager::OpenAudio(const std::string& path, const std::string& id)
{
    if (m_audioVolumes.count(id) != 0) {
        gameaf::log("[warring][OpenAudio] `{}`重名了......", id);
        return false;
    }

    if (id == "") {
        gameaf::log("[warring][OpenAudio] `{}`id不可为空......", id);
        return false;
    }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "open %s alias %s", path.c_str(), id.c_str());
    MMRESULT result = mciSendStringA(strCmd, NULL, 0, NULL);
    if (result != 0) {
        char errorMsg[256] = {0};
        mciGetErrorStringA(result, errorMsg, sizeof(errorMsg));
        gameaf::log("[error][OpenAudio] path: `{}`-{},音频加载失败:{}", path, id,
                    GBKStrToUTF8Str(errorMsg));
        return false;
    }
#else
#endif
    m_audioVolumes.emplace(id, AudioVolume{});
    return true;
}

bool AudioManager::PlayAudio(const std::string& id, bool isLoop)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][PlayAudio] `{}`并非小曲......", id);
        return false;
    }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "play %s %s from 0", id.c_str(), isLoop ? _T("repeat") : _T(""));
    MMRESULT result = mciSendStringA(strCmd, NULL, 0, NULL);
    if (result != 0) {
        char errorMsg[256] = {0};
        mciGetErrorStringA(result, errorMsg, sizeof(errorMsg));
        gameaf::log("[error][PlayAudio] path: {}音频播放失败:{}", id, GBKStrToUTF8Str(errorMsg));
        return false;
    }
#else
#endif
    return true;
}

bool AudioManager::StopAudio(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][StopAudio] `{}`并非小曲......", id);
        return false;
    }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "stop %s", id.c_str());
    mciSendStringA(strCmd, NULL, 0, NULL);
#else
#endif
    return true;
}

bool AudioManager::CloseAudio(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][StopAudio] `{}`并非小曲......", id);
        return false;
    }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "close %s", id.c_str());
    mciSendStringA(strCmd, NULL, 0, NULL);
#else
#endif
    m_audioVolumes.erase(id);
    return true;
}

bool AudioManager::PauseAudio(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][StopAudio] `{}`并非小曲......", id);
        return false;
    }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "pause %s", id.c_str());
    mciSendStringA(strCmd, NULL, 0, NULL);
#else
#endif
    return true;
}

bool AudioManager::ResumeAudio(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][ResumeAudio] `{}`并非小曲......", id);
        return false;
    }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "resume %s", id.c_str());
    mciSendStringA(strCmd, NULL, 0, NULL);
#else
#endif
    return true;
}

inline static void SetAudioVolume(const std::string& id, float scale)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "setaudio %s volume to %d", id.c_str(), static_cast<int>(1000 * scale));
    mciSendStringA(strCmd, NULL, 0, NULL);
#else
#endif
}

bool AudioManager::SetCategoryVolume(const std::string& id, float volume)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][SetCategoryVolume] `{}`并非小曲......", id);
        return false;
    }
    volume = std::clamp(volume, 0.0f, 1.0f);
    m_audioVolumes[id].categoryVolume = std::round(volume * 100) / 100;
    SetAudioVolume(id, m_globalVolume * m_audioVolumes[id].relativeVolume * volume);
    return true;
}

bool AudioManager::AdjustCategoryVolume(const std::string& id, float delta)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][AdjustCategoryVolume] `{}`并非小曲......", id);
        return false;
    }
    return SetCategoryVolume(id, m_audioVolumes[id].categoryVolume + delta);
}

float AudioManager::GetCategoryVolume(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][GetCategoryVolume] `{}`并非小曲......", id);
        return -1.0f;
    }
    return m_audioVolumes[id].categoryVolume;
}

bool AudioManager::SetRelativeVolume(const std::string& id, float volume)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][SetRelativeVolume] `{}`并非小曲......", id);
        return false;
    }
    volume = std::clamp(volume, 0.0f, 1.0f);
    m_audioVolumes[id].relativeVolume = std::round(volume * 100) / 100;
    SetAudioVolume(id, m_globalVolume * m_audioVolumes[id].categoryVolume * volume);
    return true;
}

bool AudioManager::AdjustRelativeVolume(const std::string& id, float delta)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][AdjustRelativeVolume] `{}`并非小曲......", id);
        return false;
    }
    return SetRelativeVolume(id, m_audioVolumes[id].relativeVolume + delta);
}

float AudioManager::GetRelativeVolume(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) {
        gameaf::log("[warring][GetRelativeVolume] `{}`并非小曲......", id);
        return -1.0f;
    }
    return m_audioVolumes[id].relativeVolume;
}

void AudioManager::SetGlobalVolume(float volume)
{
    volume = std::clamp(volume, 0.0f, 1.0f);
    m_globalVolume = std::round(volume * 100) / 100;
    for (const auto& [id, audioVolume] : m_audioVolumes) {
        SetAudioVolume(id,
                       m_globalVolume * audioVolume.categoryVolume * audioVolume.relativeVolume);
    }
}

void AudioManager::AdjustGlobalVolume(float delta) { SetGlobalVolume(m_globalVolume + delta); }

float AudioManager::GetGlobalVolume() { return m_globalVolume; }

float AudioManager::GetVolume(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) return -1.0f;
    return m_globalVolume * m_audioVolumes[id].categoryVolume * m_audioVolumes[id].relativeVolume;
}

}  // namespace gameaf