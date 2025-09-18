#include "Camera.h"
#include <math.h>

#include "../../Utility/Utility.h"

Camera* Camera::instance_ = nullptr;

void Camera::Init() {
    camPos_ = VGet(0, height_, -distance_);
    camTarget_ = VGet(0, 0, 0);
}

void Camera::Update(const VECTOR& playerPos, const VECTOR& playerAngle) {
    // プレイヤーの向きに合わせて後方に配置
    float angle = playerAngle.y; // Y軸回転（向き）
    camPos_.x = playerPos.x - sinf(angle) * distance_;
    camPos_.z = playerPos.z - cosf(angle) * distance_;
    camPos_.y = playerPos.y + height_;

    // 注視点はプレイヤー位置
    camTarget_ = playerPos;
}

void Camera::Apply() {
    SetCameraPositionAndTarget_UpVecY(camPos_, camTarget_);
}

