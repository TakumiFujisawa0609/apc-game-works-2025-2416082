#include "HandSlap.h"

#include "../../../../Application/Application.h"
#include "../../../Player/Player.h"

#include "../../../../Scene/Game/GameScene.h"

HandSlap::HandSlap(const VECTOR& boss, const VECTOR& player, const int& voiceLevel) :
    AttackBase(boss, player, voiceLevel),
    isHit_(false),
    state_(STATE::WAIT),
    fallSpeed_(0.0f)
{
    unit_.model_ = MV1LoadModel("Data/Model/Boss/hand.mv1");
}

HandSlap::~HandSlap()
{
}

void HandSlap::DefaultInit(void)
{
    unit_.para_.colliShape = CollisionShape::OBB;
    unit_.para_.colliType = CollisionType::ENEMY;

    unit_.angle_ = { Utility::Deg2RadF(-90.0f), Utility::Deg2RadF(90.0f), 0.0f };

    unit_.para_.size = SIZE;
    unit_.scale_ = SCALE;

    // ターゲットの真上に配置
    unit_.pos_ = VGet(player_.x, player_.y + OFFSET_Y, player_.z);

    // ステート管理用関数======================================
    StateAdd((int)STATE::WAIT, [this](void) { Wait(); });
    StateAdd((int)STATE::FALL, [this](void) { Fall(); });
    StateAdd((int)STATE::STOP, [this](void) { Fly(); });
    StateAdd((int)STATE::END, [this](void) { End();  });
    // ========================================================

    state_ = STATE::WAIT;   // 状態の初期化

    isHit_ = false;     // 攻撃が当たったかどうか

    fallSpeed_ = 0.0f;  // 落ちる速度

    attack_.attackCounter_ = COUNT_DOWN;  // 攻撃中の処理用変数の初期化

    player_;
}



void HandSlap::DefaultUpdate(void)
{
    if (attack_.end_) { return; }

    StateUpdate(static_cast<int>(state_));

    Invi();
}


void HandSlap::LinesDraw(void)
{
    if (!unit_.isAlive_) { return; }

    if (state_ == STATE::WAIT) {
        SetFontSize(128);
        DrawString(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, "攻撃が来る!\n叫ぶんだ！！！", 0xffffff);
        SetFontSize(0);
    }
}

bool HandSlap::IsChanceNow(void)
{
    if (Utility::IsHitCircle(unit_.pos_, 20, player_, 15) &&
        isHit_ == false) {
        GameScene::Slow(10);
        return true;
    }
    return false;
}

void HandSlap::DebugDraw(void)
{
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
        if (voiceLevel_ > 2500) {
            state_ = STATE::STOP;
            GameScene::HitStop(10);
            GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 20);
        }
    }
}

// 吹っ飛び状態
void HandSlap::Fly(void)
{
    // 現在位置からターゲットへのベクトル（成分ごと）
    VECTOR dir;
    dir = VSub(boss_, unit_.pos_);
    //dir.x = targetPos_.x - unit_.pos_.x;
    //dir.y = targetPos_.y - unit_.pos_.y;
    //dir.z = targetPos_.z - unit_.pos_.z;

    // 距離を計算
    float dist = Utility::VLength(dir);
    float MOVE_SPEED = 10.0f;

    if (dist < MOVE_SPEED)
    {
        // ターゲットに到達
        unit_.pos_ = player_;
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

    //// 上方向への速度
    //const float upSpeed = 30.0f;

    //// プレイヤーから手までのベクトル
    //VECTOR dir = VSub(unit_.pos_, target_);
    //// Y軸は上方向なのでXZ平面だけで計算
    //dir.y = 0.0f;

    //// 正規化して単位ベクトルに
    //float length = sqrtf(dir.x * dir.x + dir.z * dir.z);
    //if (length > 0.0f) {
    //    dir.x /= length;
    //    dir.z /= length;
    //}

    //// 飛ぶ距離を掛ける（スピード）
    //float flySpeed = 50.0f; // 好きなスピードに調整
    //dir.x *= flySpeed;
    //dir.z *= flySpeed;

    //// 位置更新
    //unit_.pos_.x += dir.x;
    //unit_.pos_.z += dir.z;
    //unit_.pos_.y += upSpeed;

    //// 回転（演出用）
    //unit_.angle_.z += Utility::Deg2RadF(30);

    //// 上限に到達したら終了
    //if (unit_.pos_.y > 4000.0f) {
    //    end_ = true;
    //}
}

// 終了処理
void HandSlap::End(void)
{
    attack_.attackCounter_--;

    if (attack_.attackCounter_ <= 0) {
        attack_.attackCounter_ = 0;

        attack_.end_ = true;
    }
}

void HandSlap::OnCollision(UnitBase* other)
{
    if (attack_.end_) { return; }

	if (auto* player = dynamic_cast<Player*>(other)) {
        isHit_ = true;
	}
}

