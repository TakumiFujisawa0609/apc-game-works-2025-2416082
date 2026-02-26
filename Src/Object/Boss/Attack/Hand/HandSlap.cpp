#include "HandSlap.h"

#include "../../../../Application/Application.h"
#include "../../../Player/Player.h"

#include "../../Boss.h"

#include "../../../../Scene/Game/GameScene.h"

HandSlap::HandSlap(const VECTOR& boss, const VECTOR& player, const int& voiceLevel) :
    AttackBase(boss, player, voiceLevel),
    isHit_(false),
    state_(STATE::WAIT),
    fallSpeed_(0.0f)
{

}

HandSlap::~HandSlap()
{
}

void HandSlap::DefaultLoad(void)
{
    // ステート管理用関数======================================
    StateAdd((int)STATE::WAIT, [this](void) { Wait(); });
    StateAdd((int)STATE::FALL, [this](void) { Fall(); });
    StateAdd((int)STATE::END, [this](void) { End();  });
    // ========================================================

    unit_.model_ = MV1LoadModel("Data/Model/Boss/hand.mv1");
}

void HandSlap::ParamInit(void)
{
    unit_.para_.colliShape = CollisionShape::OBB;
    unit_.para_.colliType = CollisionType::ENEMY;

    unit_.angle_ = { Utility::Deg2RadF(-90.0f), Utility::Deg2RadF(90.0f), 0.0f };

    unit_.para_.size = SIZE;
    unit_.scale_ = SCALE;

    // ターゲットの真上に配置
    unit_.pos_ = VGet(player_.x, player_.y + OFFSET_Y, player_.z);

    state_ = STATE::WAIT;   // 状態の初期化

    isHit_ = false;     // 攻撃が当たったかどうか

    fallSpeed_ = 0.0f;  // 落ちる速度

    attack_.attackCounter_ = COUNT_DOWN;  // 攻撃中の処理用変数の初期化

    attack_.parryCollRadius_ = 50;

    attack_.isEnd_ = false;
}

void HandSlap::DefaultUpdate(void)
{
    if (attack_.isEnd_) { return; }

    StateUpdate(static_cast<int>(state_));

    Invi();
}


void HandSlap::DefaultDraw(void)
{
    if (!unit_.isAlive_) { return; }


#ifdef _DEBUG

    //DrawSphere3D(unit_.pos_, unit_.para_.radius, 16, 0xff00ff, 0xff00ff, false);
    VECTOR size = { unit_.para_.size.x / 2, unit_.para_.size.y / 2, unit_.para_.size.z / 2 };
    VECTOR pos1 = VSub(unit_.pos_, size);
    VECTOR pos2 = VAdd(unit_.pos_, size);

    DrawCube3D(
        pos1,
        pos2,
        GetColor(255, 0, 0),
        GetColor(255, 0, 0),
        false
    );

#endif
}

// 待機状態
void HandSlap::Wait(void)
{
    // 手がプレイヤーの頭上で待機
    unit_.pos_ = { player_.x, player_.y + OFFSET_Y, player_.z };
    attack_.attackCounter_--;

    // 時間がたったら落下状態に遷移
    if (attack_.attackCounter_ <= 0) {
        attack_.attackCounter_ = 0;
        state_ = STATE::FALL;
    }
}

// 落下状態
void HandSlap::Fall(void)
{
    fallSpeed_ += GRAVITY;
    unit_.pos_.y -= fallSpeed_;

    // 落下して地面に着地、終了処理に遷移
    if (unit_.pos_.y <= 0) {
        unit_.pos_.y = 0;
        GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG, 60);
        state_ = STATE::END;
        attack_.attackCounter_ = COUNT_DOWN;
    }

    if (IsChanceNow() == true) {
        // プレイヤーのボイスが一定以上なら吹っ飛ぶ
        if (voiceLevel_ > PARRY_LEVEL) {
            //state_ = STATE::STOP;
            attack_.isParried_ = true;
            GameScene::HitStop(10);
            GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 20);
        }
#ifdef _DEBUG
        if (CheckHitKey(KEY_INPUT_1)) {
            attack_.isParried_ = true;
            GameScene::HitStop(10);
            GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 20);
        }
#endif // _DEBUG
    }
}

// 終了処理
void HandSlap::End(void)
{
    attack_.attackCounter_--;

    if (attack_.attackCounter_ <= 0) {
        attack_.attackCounter_ = 0;

        attack_.isEnd_ = true;
    }
}

void HandSlap::OnCollision(UnitBase* other)
{
    if (attack_.isEnd_) { return; }

	if (auto* player = dynamic_cast<Player*>(other)) {
        isHit_ = true;
	}

    if (!attack_.isParried_) { return; }

    if (dynamic_cast<Boss*> (other)) {
        attack_.isEnd_ = true;  // 攻撃終了
        unit_.isAlive_ = false; // 生存判定
    }
}

