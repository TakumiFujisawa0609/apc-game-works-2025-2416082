#include "RotateHand.h"

#include "../../../Player/Player.h"

RotateHand::RotateHand(int model, const VECTOR& bossPos)
{
    center_ = bossPos;
    radius_ = 600.0f;
    angle_ = 0.0f;
    speed_ = Utility::Deg2RadF(2.0f); // 1フレーム2度
    end_ = false;

    unit_.model_ = model;
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
    unit_.para_.colliShape = CollisionShape::OBB;
    unit_.para_.colliType = CollisionType::ENEMY;

    unit_.scale_ = SCALE;
    unit_.para_.size = VGet(200.0f, 50.0f, 50.0f); // 腕っぽい当たり判定

    unit_.isAlive_ = true;

    end_ = false;
}

void RotateHand::SubUpdate()
{
    if (end_) return;

    angle_ += speed_;

    // 円周上を回転
    unit_.pos_.x = center_.x + cosf(angle_) * radius_;
    unit_.pos_.z = center_.z + sinf(angle_) * radius_;
    unit_.pos_.y = 0.0f;

    // 角度に合わせて向きを回す
    unit_.angle_.y = angle_ + DX_PI_F / 2.0f;

    // 1周したら終了
    if (angle_ >= DX_PI_F * 2.0f) {
        end_ = true;
    }
}

void RotateHand::SubDraw()
{
    if (end_) return;

    MATRIX mat = MGetIdent();

    mat = MMult(MGetScale(unit_.scale_), mat);
    Utility::MatrixRotMult(mat, unit_.angle_);
    Utility::MatrixPosMult(mat, unit_.pos_);

    MV1SetMatrix(unit_.model_, mat);
    MV1DrawModel(unit_.model_);
}

void RotateHand::SubRelease(void)
{
}

void RotateHand::OnCollision(UnitBase* other)
{
    if (end_) return;

    if (auto* player = dynamic_cast<Player*>(other)) {
        // ダメージ処理
    }
}