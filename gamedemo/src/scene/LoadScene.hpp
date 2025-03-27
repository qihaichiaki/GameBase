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
        auto& resourceManager = ResourceManager::GetInstance();

        // === 加载主线程加载资源 ===
        resourceManager.LoadFont(ASSETS_PATH "SyneMono-Regular.ttf");
        // 加载菜单场景的音乐
        resourceManager.LoadAudio(ASSETS_PATH "audio/bgm-menu.wav", "menu-bgm");
        // 加载加载音效
        resourceManager.LoadAudio(ASSETS_PATH "audio/play_tape.mp3", "play_tape");
        // 加载加载场景的图集
        resourceManager.LoadAtlas(ASSETS_PATH "effect/load/little_knight/%d.png", 5,
                                  "little_knight");

        // 加载游戏场景 和 角色相关音频资源
        resourceManager.LoadAudio(ASSETS_PATH "audio/bgm-game.mp3", "game-bgm");

        // 加载角色公共音频
        resourceManager.LoadAudio(ASSETS_PATH "audio/metalwalk.mp3", "walk");
        resourceManager.LoadAudio(ASSETS_PATH "audio/run.mp3", "run");
        resourceManager.LoadAudio(ASSETS_PATH "audio/walljump.mp3", "jump");
        resourceManager.LoadAudio(ASSETS_PATH "audio/falling.mp3", "falling");
        resourceManager.LoadAudio(ASSETS_PATH "audio/land.mp3", "land");

        resourceManager.LoadAudio(ASSETS_PATH "audio/Block/sword_hit_1.mp3", "block1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/Block/sword_hit_2.mp3", "block2");
        resourceManager.LoadAudio(ASSETS_PATH "audio/Block/sword_hit_3.mp3", "block3");

        // 加载玩家音频
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Attack/PlayerAttackLong0.wav",
                                  "playerAttackLong1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Attack/PlayerAttackLong1.wav",
                                  "playerAttackLong2");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Attack/PlayerAttackLong2.wav",
                                  "playerAttackLong3");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Attack/PlayerAttackLong3.wav",
                                  "playerAttackLong4");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Attack/Short1.wav",
                                  "playerAttackShort1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Attack/Short2.wav",
                                  "playerAttackShort2");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Attack/Short3.wav",
                                  "playerAttackShort3");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Attack/Short4.wav",
                                  "playerAttackShort4");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Death/death_a_01.wav", "playerDead1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Death/death_a_02.wav", "playerDead2");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Death/death_a_03.wav", "playerDead3");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Death/death_a_04.wav", "playerDead4");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Hit/Hit2.wav", "playerHitShort");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Hit/Hit3.wav", "playerHitLong");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Hurt/04.wav", "playerHurt1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Hurt/05.wav", "playerHurt2");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Hurt/06.wav", "playerHurt3");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Hurt/07.wav", "playerHurt4");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Hurt/08.wav", "playerHurt5");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/Hurt/09.wav", "playerHurt6");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/JumpRoll/PlayerJumpRoll0.wav",
                                  "playerJumpRoll1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/JumpRoll/PlayerJumpRoll1.wav",
                                  "playerJumpRoll2");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/JumpRoll/PlayerJumpRoll2.wav",
                                  "playerJumpRoll3");
        resourceManager.LoadAudio(ASSETS_PATH "audio/player/JumpRoll/PlayerJumpRoll3.wav",
                                  "playerJumpRoll4");

        // 加载大黄蜂音频
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/Evade/evade.mp3", "hornetEvade");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/Evade/evade_0.mp3", "hornetEvade1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/Evade/evade_1.mp3", "hornetEvade2");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/Jump/voice_jump_0.mp3", "hornetJump1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/Jump/voice_jump_1.mp3", "hornetJump2");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/Jump/voice_jump_2.mp3", "hornetJump3");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/attack.mp3", "hornetAttack");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/dash.mp3", "hornetDash");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/hurt.mp3", "hornetHurt");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/voice_die.mp3", "hornetDead");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/voice_remoteskill_0.mp3",
                                  "hornetRemoteSkill1");
        resourceManager.LoadAudio(ASSETS_PATH "audio/hornet/voice_remoteskill_1.mp3",
                                  "hornetRemoteSkill2");

        // === 加载场景特有的初始化 ===
        auto load = std::make_shared<GameObject>("load");
        auto loadAnimator = load->CreateComponent<Animator>();
        loadAnimator->AddAnimationForAtlas("load", "little_knight", true);

        auto progressBar = std::make_shared<ProgressBar>();
        progressBar->Translate({0.0f, 200.0f});
        progressBar->SetSizeScale({2.0f, 1.0f});
        bar = progressBar.get();

        AddGameObjects({load, progressBar});
    }

    void OnUpdate() override
    {
        auto& sceneManager = SceneManager::GetInstance();
        bar->SetTargetProgressValue(sceneManager.GetLoadProgress());
    }

    void OnEnter() override { Audio::PlayAudio("play_tape", true); }

    void OnExit() override { Audio::StopAudio("play_tape"); }

private:
    ProgressBar* bar;
};