#include "BossShot.h"

#include "../../../Player/Player.h"

BossShot::BossShot(VECTOR bossPos, const VECTOR& targetPos) :
	bossPos_(bossPos),
	targetPos_(targetPos),
	isEnd_(false)
{
}

BossShot::~BossShot()
{
}

void BossShot::SubLoad(void)
{
}

void BossShot::SubInit(void)
{
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.colliType = CollisionType::ENEMY;

    unit_.para_.radius = 300;

	unit_.pos_ = bossPos_;
	unit_.scale_ = Utility::VECTOR_ONE;
	unit_.angle_ = Utility::VECTOR_ONE;

	isEnd_ = false;
}

void BossShot::SubUpdate(void)
{
    if (isEnd_) return;

    // 移動速度
    float speed = 10.0f;

    // 現在位置からターゲットへのベクトル（成分ごと）
    VECTOR dir;
    dir.x = targetPos_.x - unit_.pos_.x;
    dir.y = targetPos_.y - unit_.pos_.y;
    dir.z = targetPos_.z - unit_.pos_.z;

    // 距離を計算
    float dist = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

    if (dist < speed)
    {
        // ターゲットに到達
        unit_.pos_ = targetPos_;
    }
    else
    {
        // 正規化して速度分移動
        dir.x /= dist;
        dir.y /= dist;
        dir.z /= dist;

        unit_.pos_.x += dir.x * speed;
        unit_.pos_.y += dir.y * speed;
        unit_.pos_.z += dir.z * speed;
    }

}

void BossShot::SubDraw(void)
{
    if (isEnd_) { return; }
	DrawSphere3D(unit_.pos_, unit_.para_.radius, 0, 0xffffff, 0xffffff, true);
}

void BossShot::SubRelease(void)
{
}

void BossShot::OnCollision(UnitBase* other)
{
    if (dynamic_cast<Player*>(other)) {
        isEnd_ = true;
    }
}
