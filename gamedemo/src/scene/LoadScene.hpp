#pragma once

#include <game_object/GameObject.h>
#include <game_object/component/Animator.h>
#include <game_object/component/AudioManager.h>
#include <game_object/widgets/ProgressBar.h>
#include <resource/ResourceManager.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

#include <common/Log.hpp>

/**
 * 加载场景
 */

class LoadScene : public Scene
{
public:
    void OnAwake() override
    {
        auto load = std::make_shared<GameObject>("load");
        ResourceManager::GetInstance().LoadAtlas(ASSETS_PATH "effect/little_knight/%d.png", 5,
                                                 "little_knight");
        load->CreateComponent<Animator>()->AddAnimationForAtlas("load", "little_knight", true);

        auto progressBar = std::make_shared<ProgressBar>();
        progressBar->Translate({0.0f, 200.0f});
        progressBar->SetSizeScale({2.0f, 1.0f});
        bar = progressBar.get();

        AddGameObjects({load, progressBar});
        ResourceManager::GetInstance().LoadAudio(ASSETS_PATH "audio/play_tape.mp3", "play_tape");
    }

    void OnUpdate() override
    {
        auto& sceneManager = SceneManager::GetInstance();
        bar->SetTargetProgressValue(sceneManager.GetLoadProgress());
    }

    void OnEnter() override { AudioManager::GetInstance().PlayAudio("play_tape", true); }

    void OnExit() override { AudioManager::GetInstance().StopAudio("play_tape"); }

private:
    ProgressBar* bar;
};