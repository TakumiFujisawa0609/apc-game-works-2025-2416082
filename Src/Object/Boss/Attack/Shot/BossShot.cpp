#include "BossShot.h"

#include "../../../Player/Player.h"
#include "../../../Player/Arm/Base/ArmBase.h"

BossShot::BossShot(int modelId_, const Base& boss, const VECTOR& targetPos) :
	boss_(boss),
	targetPos_(targetPos),
	isEnd_(false)
{
    unit_.model_ = modelId_;
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
    unit_.para_.radius = RADIUS;

    // ボスのY回転行列を作る
    MATRIX rot = MGetRotY(boss_.angle_.y);

    // ローカル → ワールドへ
    VECTOR worldOffset = VTransform(LOCAL_POS, rot);

    // ボス基準で配置
    unit_.pos_ = VAdd(boss_.pos_, worldOffset);

    unit_.scale_ = { 2.5f,2.5f,2.5f };
    unit_.angle_ = Utility::VECTOR_ZERO;

    unit_.isAlive_ = true;
    isEnd_ = false;

    state_ = STATE::WAIT;

    attackCounter_ = SHOT_TIME;

    StateAdd(static_cast<int>(STATE::WAIT), [this]() { Wait(); });
    StateAdd(static_cast<int>(STATE::MOVE), [this]() { Move(); });
}

void BossShot::SubUpdate(void)
{
    if (isEnd_) return;

    StateUpdate(static_cast<int>(state_));
}

void BossShot::SubDraw(void)
{
    if (!unit_.isAlive_) { return; }
	DrawSphere3D(unit_.pos_, unit_.para_.radius, 10, 0xff5555, 0xff5555, false);

    MATRIX mat = MGetIdent();

    mat = MMult(MGetScale(unit_.scale_), mat);

    Utility::MatrixRotMult(mat, unit_.angle_);

    VECTOR worldPos = VTransform(VGet(-0.0f, -60.0f, -20.0f), mat);
    VECTOR offset = VAdd(unit_.pos_, worldPos);

    Utility::MatrixPosMult(mat, offset);

    MV1SetMatrix(unit_.model_,mat);
    MV1DrawModel(unit_.model_);
}

void BossShot::SubRelease(void)
{
    MV1DeleteModel(unit_.model_);
}

void BossShot::Wait(void)
{
    MATRIX rot = MGetRotY(boss_.angle_.y);
    VECTOR worldOffset = VTransform(LOCAL_POS, rot);

    unit_.pos_ = VAdd(boss_.pos_, worldOffset);

    attackCounter_--;

    if (attackCounter_ <= 0) {
        attackCounter_ = SHOT_TIME;
        state_ = STATE::MOVE;
    }

}

void BossShot::Move(void)
{
    // 現在位置からターゲットへのベクトル（成分ごと）
    VECTOR dir;
    dir = VSub(targetPos_, unit_.pos_);
    //dir.x = targetPos_.x - unit_.pos_.x;
    //dir.y = targetPos_.y - unit_.pos_.y;
    //dir.z = targetPos_.z - unit_.pos_.z;

    // 距離を計算
    float dist = Utility::VLength(dir);

    if (dist < MOVE_SPEED)
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

        unit_.pos_.x += dir.x * MOVE_SPEED;
        unit_.pos_.y += dir.y * MOVE_SPEED;
        unit_.pos_.z += dir.z * MOVE_SPEED;
    }
}

void BossShot::OnCollision(UnitBase* other)
{
    if(state_ == STATE::WAIT){return;}

    if (dynamic_cast<Player*>(other)) {
        isEnd_ = true;
        unit_.isAlive_ = false;
    }

    if (dynamic_cast<ArmBase*>(other)) {
        isEnd_ = true;
        unit_.isAlive_ = false;
    }
}
