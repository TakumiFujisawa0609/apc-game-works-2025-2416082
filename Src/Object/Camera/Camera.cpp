#include "Camera.h"
#include <math.h>

#include "../../Application/Application.h"

#include "../../Manager/Input/InputManager.h"

#include "../../Utility/Utility.h"

Camera* Camera::instance_ = nullptr;

void Camera::Init() 
{
    angle_ = Utility::VECTOR_ZERO;

    modeFuncs_ = {
        { MODE::PLAYER_FOLLOW, &Camera::PlayerFollowCamera },
        { MODE::BOSS_DEATH,    &Camera::BossDeathCamera     },
    };

    deathTimer_ = 0.0f;
    radius_ = 30.0f;
    height_ = 10.0f;

    mode_ = MODE::PLAYER_FOLLOW;
}

void Camera::Update()
{
    auto func = modeFuncs_.find(mode_);
    if (func != modeFuncs_.end()) {
        (this->*(func->second))();
    }
}

void Camera::Apply()
{
    switch (mode_)
    {
    case Camera::PLAYER_FOLLOW:
        SetCameraPositionAndTarget_UpVecY(camPos_, *targetPlayerPos_);
        break;
    case Camera::BOSS_DEATH:
        SetCameraPositionAndTarget_UpVecY(camPos_, *targetBossPos_);
        break;
    default:
        break;
    }

    SetUseLighting(false);

    SetUseLighting(true);
}

void Camera::MouseMoveCamera(void)
{
    // 画面の中央を基準にする
    int nowMouseX, nowMouseY;
    GetMousePoint(&nowMouseX, &nowMouseY);

    // 差分計算（相対移動量）
    int deltaX = nowMouseX - Application::SCREEN_SIZE_X / 2;
    int deltaY = nowMouseY - Application::SCREEN_SIZE_Y / 2;

    // マウスを再び中央に戻す
    SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

    // 初回だけ差分を無視（大ジャンプ防止）
    static bool first = true;
    if (first) {
        first = false;
        return;
    }

    const float sens = 0.1f; // 感度調整
    angle_.y += deltaX * sens;
    angle_.x += deltaY * sens;

    // 上下の回転制限
    float limit = DX_PI_F * 18;
    if (angle_.x > limit) angle_.x = limit;
    if (angle_.x < -limit) angle_.x = -limit;
}

void Camera::PadMoveCamera()
{
    int lx = 0, ly = 0;
    GetJoypadAnalogInputRight(&lx, &ly, DX_INPUT_PAD1);

    const float sens = 0.05f;
    const int deadZone = 200;

    if (abs(lx) > deadZone)
        angle_.y += lx * sens * 0.1f;

    if (abs(ly) > deadZone)
        angle_.x += ly * sens * 0.1f;

    // 上下制限
    if (angle_.x > 89.0f) angle_.x = 89.0f;
    if (angle_.x < -89.0f) angle_.x = -89.0f;
}

void Camera::PlayerFollowCamera(void)
{
    auto& input = InputManager::GetInstance();
    MouseMoveCamera();
    PadMoveCamera();

    if (input.IsNew(KEY_INPUT_RIGHT)) angle_.y += 5;
    if (input.IsNew(KEY_INPUT_LEFT))  angle_.y -= 5;
    if (input.IsNew(KEY_INPUT_DOWN) && angle_.x <= 30)  angle_.x += 5;
    if (input.IsNew(KEY_INPUT_UP) && angle_.x >= -30)  angle_.x -= 5;

    // Y軸回転行列を作成
    MATRIX matY = MGetRotY(angle_.y * DX_PI_F / 180.0f);
    MATRIX matX = MGetRotX(angle_.x * DX_PI_F / 180.0f);
    MATRIX mat = MMult(matX, matY);

    // LOCAL_POSの周りで回転させる
    VECTOR rotatePos = VTransform(LOCAL_POS, mat);

    // 回転後の位置をターゲット座標に加算
    camPos_ = VAdd(*targetPlayerPos_, rotatePos);
}

void Camera::BossDeathCamera(void)
{
    deathTimer_ += 1.0f / 60.0f;

    const float baseRadius = 1000.0f;   // ← 横距離をしっかり取る
    const float baseHeight = 200.0f;   // ← 上から見下ろす

    const VECTOR offset = VGet(0.0f, baseHeight, -baseRadius);
    camPos_ = VAdd(bossPos_, offset);
    // ボス注視
}

