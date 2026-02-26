#include "BossShot.h"

#include "../../../Player/Player.h"
#include "../../../Player/Arm/Base/ArmBase.h"

#include "../../Boss.h"

#include "../../../../Scene/Game/GameScene.h"

BossShot::BossShot(const Base& boss, const VECTOR& player, const int& voiceLevel) :
    AttackBase(boss.pos_, player, voiceLevel),
	boss_(boss),
	player_(player),
    state_(STATE::MOVE)
{
}

BossShot::~BossShot()
{
}

void BossShot::DefaultLoad(void)
{
    // ステート管理用関数======================================
    StateAdd(static_cast<int>(STATE::WAIT), [this]() { Wait(); });
    StateAdd(static_cast<int>(STATE::MOVE), [this]() { Move(); });
    // ========================================================

    unit_.model_ = MV1LoadModel("Data/Model/Boss/Fireball/Fireball.mv1");
}

void BossShot::ParamInit(void)
{
    unit_.para_.colliShape = CollisionShape::SPHERE;
    unit_.para_.colliType = CollisionType::ALLY;
    unit_.para_.radius = RADIUS;

    // ボスのY回転行列を作る
    MATRIX rot = MGetRotY(boss_.angle_.y);

    VECTOR worldOffset = VTransform(LOCAL_POS, rot);

    // ボス基準で配置
    unit_.pos_ = VAdd(boss_.pos_, worldOffset);

    unit_.scale_ = { 2.5f,2.5f,2.5f };
    unit_.angle_ = Utility::VECTOR_ZERO;

    unit_.isAlive_ = true;
    attack_.isEnd_ = false;

    state_ = STATE::WAIT;

    attack_.attackCounter_ = SHOT_TIME;

    attack_.parryCollRadius_ = RADIUS + 20.0f;
}

void BossShot::DefaultUpdate(void)
{
    if (attack_.isEnd_) return;

    StateUpdate(static_cast<int>(state_));
}

void BossShot::DefaultDraw(void)
{
    if (!unit_.isAlive_) { return; }

#ifdef _DEBUG
    DrawSphere3D(unit_.pos_, unit_.para_.radius, 10, 0xff5555, 0xff5555, false);
#endif // _DEBUG
}

void BossShot::Wait(void)
{
    MATRIX rot = MGetRotY(boss_.angle_.y);
    VECTOR worldOffset = VTransform(LOCAL_POS, rot);

    unit_.pos_ = VAdd(boss_.pos_, worldOffset);

    attack_.attackCounter_--;

    if (attack_.attackCounter_ <= 0) {
        attack_.attackCounter_ = 0;
        state_ = STATE::MOVE;
    }
}

void BossShot::Move(void)
{
    // 現在位置からターゲットへのベクトル（成分ごと）
    VECTOR dir;
    dir = VSub(player_, unit_.pos_);

    // 距離を計算
    float dist = Utility::VLength(dir);

    if (dist < MOVE_SPEED)
    {
        // ターゲットに到達
        unit_.pos_ = player_;
    }
    else
    {
        // パリィされたかどうか
        if (IsChanceNow() == true) {
            // プレイヤーのボイスが一定以上なら吹っ飛ぶ
            if (voiceLevel_ > PARRY_LEVEL) {
                attack_.isParried_ = true;
                GameScene::HitStop(10);
                GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 20);
            }
#ifdef _DEBUG
            // プレイヤーのボイスが一定以上なら吹っ飛ぶ
            if (CheckHitKey(KEY_INPUT_1)) {
                attack_.isParried_ = true;
                GameScene::HitStop(10);
                GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 20);
            }
#endif // _DEBUG
        }

        // 正規化して速度分移動
        dir.x /= dist;
        dir.y /= dist;
        dir.z /= dist;

        unit_.pos_.x += dir.x * MOVE_SPEED;
        unit_.pos_.y += dir.y * MOVE_SPEED;
        unit_.pos_.z += dir.z * MOVE_SPEED;
    }

    attack_.attackCounter_++;
    if (attack_.attackCounter_ > 300) {
        attack_.isEnd_ = true;
        unit_.isAlive_ = false;
    
        attack_.attackCounter_ = SHOT_TIME;
    }
}

void BossShot::OnCollision(UnitBase* other)
{
    if(state_ == STATE::WAIT){return;}

    if (dynamic_cast<Player*>(other)) {
        attack_.isEnd_ = true;
        unit_.isAlive_ = false;
    }

    if (dynamic_cast<ArmBase*>(other)) {
        attack_.isEnd_ = true;
        unit_.isAlive_ = false;
    }

    if (!attack_.isParried_) { return; }

    if (dynamic_cast<Boss*> (other)) {
        attack_.isEnd_ = true;  // 攻撃終了
        unit_.isAlive_ = false; // 生存判定
    }
}
