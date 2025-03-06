#include "AudioManager.h"

#include <common/Macros.h>

#include <algorithm>

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
    m_audioVolumes.emplace(id, 100);
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

bool AudioManager::SetAudioVolume(int volume, const std::string& id)
{
    volume = std::clamp(volume, 0, 100);
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
#endif
    if (id != "") {
        if (m_audioVolumes.count(id) == 0) return false;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
        _stprintf_s(strCmd, _T("setaudio %s volume to %d"), id.c_str(), volume * 10);
        mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
        m_audioVolumes[id] = volume;
    } else {
        // 全局音频操控
        float scaleFactor = volume * 1.0f / MAX_AUDIOVOLUME;
        for (const auto [id, maxVolume] : m_audioVolumes) {
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
            _stprintf_s(strCmd, _T("setaudio %s volume to %d"), id.c_str(),
                        static_cast<int>(maxVolume * scaleFactor) * 10);
            mciSendString(strCmd, NULL, 0, NULL);
#else
#endif
        }
        m_globalVolume = volume;
    }
    return true;
}

bool AudioManager::ChangeAudioVolume(int mod, const std::string& id)
{
    if (id != "") {
        if (m_audioVolumes.count(id) == 0) return false;
        return SetAudioVolume(m_audioVolumes[id] + mod, id);
    }
    return SetAudioVolume(m_globalVolume + mod);
}

int AudioManager::GetAudioVolume(const std::string& id)
{
    int volume = 0;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static TCHAR strCmd[512];
#endif

    if (id != "") {
        if (m_audioVolumes.count(id) == 0) return -1;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
        _stprintf_s(strCmd, _T("status %s volume"), id.c_str());
        static char buffer[128];
        mciSendString(strCmd, buffer, sizeof(buffer), NULL);
        volume = std::stoi(buffer) / 10;
#else
#endif
    } else {
        volume = m_globalVolume;
    }
    return volume;
}

}  // namespace gameaf