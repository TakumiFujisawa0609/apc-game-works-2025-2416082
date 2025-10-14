#include "PauseScene.h"
#include <DxLib.h>
#include "../SceneManager/SceneManager.h"
#include "../../Application/Application.h"

void PauseScene::Update()
{
    if (CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W) && next_ != NEXT_SCENE::GAME) {
        next_ = NEXT_SCENE::GAME;
    }
    if (CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S) && next_ != NEXT_SCENE::END) {
        next_ = NEXT_SCENE::END;
    }

    if (CheckHitKey(KEY_INPUT_SPACE) || CheckHitKey(KEY_INPUT_RETURN))
    {
        switch (next_)
        {
        case PauseScene::NEXT_SCENE::END:
            Application::GetInstance().GameEnd();
            break;
        case PauseScene::NEXT_SCENE::GAME:
            auto& scene = SceneManager::GetInstance();
            scene.PopScene();
            break;
        }
    }
}

void PauseScene::Draw()
{
    // ”¼“§–¾‚Ì”wŒi
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ƒ^ƒCƒgƒ‹•¶Žš
    SetFontSize(64);
    DrawString(Application::SCREEN_SIZE_X / 2 - 120, 200, "PAUSE", GetColor(255, 255, 255));

    // ‘I‘ðŽˆ
    SetFontSize(32);
    int cx = Application::SCREEN_SIZE_X / 2 - 100;
    int cy = Application::SCREEN_SIZE_Y / 2;

    // Continue
    DrawString(cx, cy, "Continue", next_ == NEXT_SCENE::GAME ? GetColor(255, 255, 0) : GetColor(200, 200, 200));

    // End Game
    DrawString(cx, cy + 60, "End Game", next_ == NEXT_SCENE::END ? GetColor(255, 255, 0) : GetColor(200, 200, 200));

    SetFontSize(16);
}