#include "RotateHand.h"

#include "../../../Player/Player.h"

RotateHand::RotateHand(const VECTOR& bossPos)
{
    center_ = bossPos;
    radius_ = 1000.0f;
    angle_ = 0.0f;
    speed_ = Utility::Deg2RadF(2.0f); // 1ƒtƒŒ[ƒ€2“x
    end_ = false;
}

RotateHand::~RotateHand()
{
    this->SubRelease();
}

void RotateHand::SubLoad()
{
}

void RotateHand::SubInit()
{
    unit_.para_.colliShape = CollisionShape::SPHERE;
    unit_.para_.colliType = CollisionType::ENEMY;

    unit_.scale_ = SCALE;
    unit_.para_.radius = 300.0f;

    unit_.isAlive_ = true;

    end_ = false;
}

void RotateHand::SubUpdate()
{
    if (end_) return;

    angle_ += speed_;

    // ‰~Žüã‚ð‰ñ“]
    unit_.pos_.x = center_.x + cosf(angle_) * radius_;
    unit_.pos_.z = center_.z + sinf(angle_) * radius_;
    unit_.pos_.y = 0.0f + unit_.para_.radius;

    // 1Žü‚µ‚½‚çI—¹
    if (angle_ >= DX_PI_F * 2.0f) {
        end_ = true;
    }
}

void RotateHand::SubDraw()
{
    if (end_) return;

    DrawSphere3D(unit_.pos_, unit_.para_.radius, 0, 0xaaaaaa, 0xaaaaaa, true);
}

void RotateHand::SubRelease(void)
{
}

void RotateHand::OnCollision(UnitBase* other)
{
    if (end_) return;

    if (dynamic_cast<Player*>(other)) {

    }
}