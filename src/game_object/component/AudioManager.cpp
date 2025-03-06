#include "AudioManager.h"

#include <common/Macros.h>

#include <algorithm>
#include <common/Log.hpp>

#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
#pragma comment(lib, "WINMM.lib")  // 音频播放
#endif

namespace gameaf {

AudioManager::AudioManager() {}

AudioManager::~AudioManager() {}

AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}

bool AudioManager::OpenAudio(const std::string& path, const std::string& id)
{
    if (m_audioVolumes.count(id) != 0 || id == "") return false;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("open %s alias %s"), path.c_str(), id.c_str());
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
    m_audioVolumes.emplace(id, AudioVolume{});
    return true;
}

bool AudioManager::PlayAudio(const std::string& id, bool isLoop)
{
    if (m_audioVolumes.count(id) == 0) return false;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("play %s %s from 0"), id.c_str(), isLoop ? _T("repeat") : _T(""));
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
    return true;
}

bool AudioManager::StopAudio(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) return false;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("stop %s"), id.c_str());
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
    return true;
}

bool AudioManager::CloseAudio(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) return false;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("close %s"), id.c_str());
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
    m_audioVolumes.erase(id);
    return true;
}

bool AudioManager::PauseAudio(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) return false;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("pause %s"), id.c_str());
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
    return true;
}

bool AudioManager::ResumeAudio(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) return false;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("resume %s"), id.c_str());
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
    return true;
}

inline static void SetAudioVolume(const std::string& id, float scale)
{
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("setaudio %s volume to %d"), id.c_str(), static_cast<int>(1000 * scale));
    mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
}

bool AudioManager::SetCategoryVolume(const std::string& id, float volume)
{
    if (m_audioVolumes.count(id) == 0) return false;
    volume = std::clamp(volume, 0.0f, 1.0f);
    m_audioVolumes[id].categoryVolume = volume;
    SetAudioVolume(id, m_globalVolume * m_audioVolumes[id].relativeVolume * volume);
    return true;
}

bool AudioManager::AdjustCategoryVolume(const std::string& id, float delta)
{
    if (m_audioVolumes.count(id) == 0) return false;
    return SetCategoryVolume(id, m_audioVolumes[id].categoryVolume + delta);
}

float AudioManager::GetCategoryVolume(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) return -1.0f;
    return m_audioVolumes[id].categoryVolume;
}

bool AudioManager::SetRelativeVolume(const std::string& id, float volume)
{
    if (m_audioVolumes.count(id) == 0) return false;
    volume = std::clamp(volume, 0.0f, 1.0f);
    m_audioVolumes[id].relativeVolume = volume;
    SetAudioVolume(id, m_globalVolume * m_audioVolumes[id].categoryVolume * volume);
    return true;
}

bool AudioManager::AdjustRelativeVolume(const std::string& id, float delta)
{
    if (m_audioVolumes.count(id) == 0) return false;
    return SetRelativeVolume(id, m_audioVolumes[id].relativeVolume + delta);
}

float AudioManager::GetRelativeVolume(const std::string& id)
{
    if (m_audioVolumes.count(id) == 0) return -1.0f;
    return m_audioVolumes[id].relativeVolume;
}

void AudioManager::SetGlobalVolume(float volume)
{
    volume = std::clamp(volume, 0.0f, 1.0f);
    m_globalVolume = volume;
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