#include "AudioManager.h"

#include <common/Macros.h>

#include <algorithm>
#include <common/Log.hpp>
#include <common/Utils.hpp>

#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
#pragma comment(lib, "WINMM.lib")  // 音频播放
#endif

namespace gameaf {
std::unordered_map<std::string, Audio::AudioVolume> Audio::audioVolumes{};  // 音频强度表
float Audio::m_globalVolume = 1.0f;

void Audio::ClearAllAudio()
{
    for (const auto& [id, _] : audioVolumes) {
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
        static char strCmd[512];
        _stprintf_s(strCmd, "close %s", id.c_str());
        mciSendStringA(strCmd, NULL, 0, NULL);
#else
#endif
    }
}

bool Audio::OpenAudio(const std::string& path, const std::string& id)
{
    if (audioVolumes.count(id) != 0) {
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
    audioVolumes.emplace(id, AudioVolume{});
    return true;
}

void Audio::PlayAudio(std::string_view id, bool isLoop)
{
    // if (audioVolumes.count(id.data()) == 0) {
    //     gameaf::log("[warring][PlayAudio] `{}`并非小曲......", id);
    //     return false;
    // }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "play %s %s from 0", id.data(), isLoop ? _T("repeat") : _T(""));
    mciSendStringA(strCmd, NULL, 0, NULL);
    // MMRESULT result = mciSendStringA(strCmd, NULL, 0, NULL);
    // if (result != 0) {
    //     char errorMsg[256] = {0};
    //     mciGetErrorStringA(result, errorMsg, sizeof(errorMsg));
    //     gameaf::log("[error][PlayAudio] path: {}音频播放失败:{}", id, GBKStrToUTF8Str(errorMsg));
    //     // return false;
    // }
#else
#endif
}

void Audio::StopAudio(std::string_view id)
{
    // if (audioVolumes.count(id.data()) == 0) {
    //     gameaf::log("[warring][StopAudio] `{}`并非小曲......", id);
    //     return false;
    // }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "stop %s", id.data());
    mciSendStringA(strCmd, NULL, 0, NULL);
#else
#endif
}

bool Audio::CloseAudio(const std::string& id)
{
    if (audioVolumes.count(id) == 0) {
        gameaf::log("[warring][StopAudio] `{}`并非小曲......", id);
        return false;
    }
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "close %s", id.c_str());
    mciSendStringA(strCmd, NULL, 0, NULL);
#else
#endif
    audioVolumes.erase(id);
    return true;
}

bool Audio::PauseAudio(const std::string& id)
{
    if (audioVolumes.count(id) == 0) {
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

bool Audio::ResumeAudio(const std::string& id)
{
    if (audioVolumes.count(id) == 0) {
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

bool Audio::SetCategoryVolume(const std::string& id, float volume)
{
    if (audioVolumes.count(id) == 0) {
        gameaf::log("[warring][SetCategoryVolume] `{}`并非小曲......", id);
        return false;
    }
    volume = std::clamp(volume, 0.0f, 1.0f);
    audioVolumes[id].categoryVolume = std::round(volume * 100) / 100;
    SetAudioVolume(id, m_globalVolume * audioVolumes[id].relativeVolume * volume);
    return true;
}

bool Audio::AdjustCategoryVolume(const std::string& id, float delta)
{
    if (audioVolumes.count(id) == 0) {
        gameaf::log("[warring][AdjustCategoryVolume] `{}`并非小曲......", id);
        return false;
    }
    return SetCategoryVolume(id, audioVolumes[id].categoryVolume + delta);
}

float Audio::GetCategoryVolume(const std::string& id)
{
    if (audioVolumes.count(id) == 0) {
        gameaf::log("[warring][GetCategoryVolume] `{}`并非小曲......", id);
        return -1.0f;
    }
    return audioVolumes[id].categoryVolume;
}

bool Audio::SetRelativeVolume(const std::string& id, float volume)
{
    if (audioVolumes.count(id) == 0) {
        gameaf::log("[warring][SetRelativeVolume] `{}`并非小曲......", id);
        return false;
    }
    volume = std::clamp(volume, 0.0f, 1.0f);
    audioVolumes[id].relativeVolume = std::round(volume * 100) / 100;
    SetAudioVolume(id, m_globalVolume * audioVolumes[id].categoryVolume * volume);
    return true;
}

bool Audio::AdjustRelativeVolume(const std::string& id, float delta)
{
    if (audioVolumes.count(id) == 0) {
        gameaf::log("[warring][AdjustRelativeVolume] `{}`并非小曲......", id);
        return false;
    }
    return SetRelativeVolume(id, audioVolumes[id].relativeVolume + delta);
}

float Audio::GetRelativeVolume(const std::string& id)
{
    if (audioVolumes.count(id) == 0) {
        gameaf::log("[warring][GetRelativeVolume] `{}`并非小曲......", id);
        return -1.0f;
    }
    return audioVolumes[id].relativeVolume;
}

void Audio::SetGlobalVolume(float volume)
{
    volume = std::clamp(volume, 0.0f, 1.0f);
    m_globalVolume = std::round(volume * 100) / 100;
    for (const auto& [id, audioVolume] : audioVolumes) {
        SetAudioVolume(id,
                       m_globalVolume * audioVolume.categoryVolume * audioVolume.relativeVolume);
    }
}

void Audio::AdjustGlobalVolume(float delta) { SetGlobalVolume(m_globalVolume + delta); }

float Audio::GetGlobalVolume() { return m_globalVolume; }

float Audio::GetVolume(const std::string& id)
{
    if (audioVolumes.count(id) == 0) return -1.0f;
    return m_globalVolume * audioVolumes[id].categoryVolume * audioVolumes[id].relativeVolume;
}

bool Audio::IsPlayingAudio(const std::string& id)
{
    if (audioVolumes.count(id) == 0) return false;

#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "status %s mode", id.c_str());
    char buffer[128];
    mciSendStringA(strCmd, buffer, sizeof(buffer), NULL);
    if (strcmp(buffer, "playing") == 0) return true;
#else
#endif
    return false;
}

float Audio::AskAudioLength(const std::string& id)
{
    if (audioVolumes.count(id) == 0) return 0.0f;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "status %s length", id.c_str());
    char buffer[128];
    mciSendStringA(strCmd, buffer, sizeof(buffer), NULL);
    return atof(buffer);
#else
#endif
    return 0.0f;
}

float Audio::AskAudioPos(const std::string& id)
{
    if (audioVolumes.count(id) == 0) return 0.0f;
#if defined(_MSC_VER) && defined(GAMEAF_USE_EASYX)
    static char strCmd[512];
    _stprintf_s(strCmd, "status %s position", id.c_str());
    char buffer[128];
    mciSendStringA(strCmd, buffer, sizeof(buffer), NULL);
    return atof(buffer);
#else
#endif
    return 0.0f;
}

}  // namespace gameaf