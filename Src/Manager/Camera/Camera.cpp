#include "Camera.h"
#include <math.h>

#include "../../Utility/Utility.h"

Camera* Camera::instance_ = nullptr;

void Camera::Init() 
{
    angle_ = Utility::VECTOR_ZERO;

    GetMousePoint(&mouseX, &mouseY);
}

void Camera::Update()
{
    if (CheckHitKey(KEY_INPUT_RIGHT)) angle_.y += 5;
    if (CheckHitKey(KEY_INPUT_LEFT))  angle_.y -= 5;
    if (CheckHitKey(KEY_INPUT_DOWN) && angle_.x <= 30)  angle_.x += 5;
    if (CheckHitKey(KEY_INPUT_UP) && angle_.x >= -30)  angle_.x -= 5;

    // Y軸回転行列を作成
    MATRIX matY = MGetRotY(angle_.y * DX_PI_F / 180.0f);
    MATRIX matX = MGetRotX(angle_.x * DX_PI_F / 180.0f);
    MATRIX mat = MMult(matX, matY);

    // LOCAL_POSの周りで回転させる
    VECTOR rotatePos = VTransform(LOCAL_POS, mat);

    // 回転後の位置をターゲット座標に加算
    camPos_ = VAdd(*camTarget_, rotatePos);
}

void Camera::Apply()
{
    SetCameraPositionAndTarget_UpVecY(camPos_, *camTarget_);
}

