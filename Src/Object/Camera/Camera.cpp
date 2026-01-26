#include "Camera.h"
#include <math.h>
#include <EffekseerForDXLib.h>

#include "../../Application/Application.h"

#include "../../Manager/Input/InputManager.h"

#include "../../Utility/Utility.h"

Camera* Camera::instance_ = nullptr;

void Camera::Init() 
{
    angle_ = Utility::VECTOR_ZERO;


    modeFuncs_ = {
        { MODE::PLAYER_FOLLOW, &Camera::PlayerFollowCamera  },
        { MODE::PLAYER_DEATH,  &Camera::PlayerDeathCamera   },
        { MODE::BOSS_DEATH,    &Camera::BossDeathCamera     },
    };

    deathTimer_ = 0.0f;
    radius_ = 30.0f;
    height_ = 10.0f;

    mode_ = MODE::PLAYER_DEATH;
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
    case Camera::PLAYER_DEATH:
        SetCameraPositionAndTarget_UpVecY(camPos_, *targetPlayerPos_);
        break;
    default:
        break;
    }

    SetCameraPositionAndAngle(camPos_, angle_.x, angle_.y, angle_.z);

    // DXライブラリのカメラとEffekseerのカメラを同期する
    Effekseer_Sync3DSetting();

    //SetUseLighting(false);

    //SetUseLighting(true);
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

void Camera::PlayerDeathCamera(void)
{
    // --- タイマー更新（60FPS想定） ---
    deathTimer_ += 1.0f / 60.0f;

    // --- 基本パラメータ ---
    const float startRadius = 1000.0f;    // 初期距離
    const float startHeight = 200.0f;     // 初期高さ
    const float rotationSpeedDegPerSec = 30.0f;  // Y軸回転速度（°/秒）
    const float zoomOutSpeed = 50.0f;     // ズームアウト速度（ユニット/秒）
    const float riseSpeed = 20.0f;        // 上昇速度（ユニット/秒）

    // --- カメラ位置計算 ---
    float currentRadius = startRadius + zoomOutSpeed * deathTimer_;
    float currentHeight = startHeight + riseSpeed * deathTimer_;

    VECTOR offset = VGet(0.0f, currentHeight, -currentRadius);
    camPos_ = VAdd(*targetPlayerPos_, offset);

    // --- Y軸回転（ラジアン換算） ---
    angle_.y += Utility::Deg2RadF(rotationSpeedDegPerSec) * (1.0f / 60.0f);

    // --- Optional: ターゲット方向を常にプレイヤーに向ける ---
    VECTOR dir = VSub(*targetPlayerPos_, camPos_);
    angle_.x = atanf(dir.y / sqrtf(dir.x * dir.x + dir.z * dir.z));
    angle_.z = 0.0f; // ロールはなし
}


void Camera::BossDeathCamera(void)
{
    const float baseRadius = 1000.0f;  
    const float baseHeight = 200.0f;   

    const VECTOR offset = VGet(0.0f, baseHeight, -baseRadius);
    camPos_ = VAdd(bossPos_, offset);
}

