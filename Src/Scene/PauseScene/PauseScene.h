#pragma once
#pragma once
#include "../SceneBase.h"

class PauseScene : public SceneBase
{
public:
    PauseScene() = default;
    ~PauseScene() override = default;

    void Load() override {}
    void Init() override {}
    void Update() override;
    void Draw() override;
    void Release() override {}

private:


    enum class NEXT_SCENE
    {
        END,
        GAME,
    };

    NEXT_SCENE next_ = NEXT_SCENE::GAME;
};