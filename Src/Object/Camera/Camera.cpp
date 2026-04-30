#include "Camera.h"
#include <math.h>
#include <EffekseerForDXLib.h>

#include "../../Application/Application.h"

#include "../../Manager/Input/KeyManager.h"

#include "../../Utility/Utility.h"

Camera* Camera::instance_ = nullptr;

Camera::Camera() :
    mode_(MODE::PLAYER_FOLLOW),
    modeFuncs_(),
    cameraPos_(0.0f,0.0f,0.0f),
    angle_(Utility::VECTOR_ZERO),
    mouseX(0),
    mouseY(0),
    targetPlayerPos_(nullptr),
    targetBossPos_(nullptr),
    bossPos_(Utility::VECTOR_ZERO),
    deathTimer_(0.0f),
    radius_(0.0f),
    height_(0.0f)
{

}

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
        SetCameraPositionAndTarget_UpVecY(cameraPos_, *targetPlayerPos_);
        break;
    case Camera::BOSS_DEATH:
        SetCameraPositionAndTarget_UpVecY(cameraPos_, *targetBossPos_);
        break;
    case Camera::PLAYER_DEATH:
        SetCameraPositionAndTarget_UpVecY(cameraPos_, *targetPlayerPos_);
        break;
    default:
        break;
    }

    //SetCameraPositionAndAngle(camPos_, angle_.x, angle_.y, angle_.z);

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

    const float sens = MOUSE_SENSI; // 感度調整
    angle_.y += deltaX * sens;
    angle_.x += deltaY * sens;


    if (angle_.x > CAMERA_MOVE_LIMIT) angle_.x = CAMERA_MOVE_LIMIT;
    if (angle_.x < -CAMERA_MOVE_LIMIT) angle_.x = -CAMERA_MOVE_LIMIT;
}

void Camera::PadMoveCamera()
{
    int lx = 0;
    int ly = 0;
    GetJoypadAnalogInputRight(&lx, &ly, DX_INPUT_PAD1);

    const int deadZone = 200;

    if (abs(lx) > deadZone)
        angle_.y += lx * PAD_SENSI * 0.1f;

    if (abs(ly) > deadZone)
        angle_.x += ly * PAD_SENSI * 0.1f;

    // 上下制限
    if (angle_.x > CAMERA_MOVE_LIMIT) angle_.x = CAMERA_MOVE_LIMIT;
    if (angle_.x < -CAMERA_MOVE_LIMIT) angle_.x = -CAMERA_MOVE_LIMIT;
}

void Camera::PlayerFollowCamera(void)
{
    MouseMoveCamera();
    PadMoveCamera();

    if (KEY::GetIns().GetInfo(KEY_TYPE::DOWN).down && angle_.x <= 30)  angle_.x += 5;
    if (KEY::GetIns().GetInfo(KEY_TYPE::UP).down && angle_.x >= -30)  angle_.x -= 5;
    if (KEY::GetIns().GetInfo(KEY_TYPE::RIGHT).down) angle_.y += 5;
    if (KEY::GetIns().GetInfo(KEY_TYPE::LEFT).down)  angle_.y -= 5;

    // Y軸回転行列を作成
    MATRIX matY = MGetRotY(Utility::Deg2RadF(angle_.y));
    MATRIX matX = MGetRotX(Utility::Deg2RadF(angle_.x));
    MATRIX mat = MMult(matX, matY);

    // LOCAL_POSの周りで回転させる
    VECTOR rotatePos = VTransform(LOCAL_POS, mat);

    // 回転後の位置をターゲット座標に加算
    cameraPos_ = VAdd(*targetPlayerPos_, rotatePos);
}

void Camera::PlayerDeathCamera(void)
{
    deathTimer_ += 1.0f / 60.0f;

    const float startRadius = 1000.0f;    // 初期距離
    const float startHeight = 200.0f;     // 初期高さ
    const float rotationSpeedDegPerSec = 30.0f;  // Y軸回転速度（°/秒）
    const float zoomOutSpeed = 50.0f;     // ズームアウト速度（ユニット/秒）
    const float riseSpeed = 20.0f;        // 上昇速度（ユニット/秒）

    // --- カメラ位置計算 ---
    float currentRadius = startRadius + zoomOutSpeed * deathTimer_;
    float currentHeight = startHeight + riseSpeed * deathTimer_;

    VECTOR offset = VGet(0.0f, currentHeight, -currentRadius);
    cameraPos_ = VAdd(*targetPlayerPos_, offset);

    // --- Y軸回転（ラジアン換算） ---
    angle_.y += Utility::Deg2RadF(rotationSpeedDegPerSec) * (1.0f / 60.0f);

    // --- Optional: ターゲット方向を常にプレイヤーに向ける ---
    VECTOR dir = VSub(*targetPlayerPos_, cameraPos_);
    angle_.x = atanf(dir.y / sqrtf(dir.x * dir.x + dir.z * dir.z));
    angle_.z = 0.0f; // ロールはなし
}


void Camera::BossDeathCamera(void)
{
    const float baseRadius = -1000.0f;
    const float baseHeight = 200.0f;   

    const VECTOR offset = VGet(0.0f, baseHeight, baseRadius);
    cameraPos_ = VAdd(bossPos_, offset);
}

