#include "HandSlap.h"

#include "../../../../Application/Application.h"
#include "../../../Player/Player.h"

#include "../../../../Scene/Game/GameScene.h"

HandSlap::HandSlap(int model, const VECTOR& target, const int& voiceLevel) :
    target_(target),
    voiceLevel_(voiceLevel),
    end_(false),
    isHit_(false),
    state_(STATE::WAIT),
    attackCounter_(0),
    fallSpeed_(0.0f)
{
    unit_.model_ = model;
}

HandSlap::~HandSlap()
{
}

void HandSlap::SubLoad(void)
{

}

void HandSlap::SubInit(void)
{
	unit_.para_.colliShape = CollisionShape::OBB;
	unit_.para_.colliType = CollisionType::ENEMY;

    unit_.angle_ = { Utility::Deg2RadF(-90.0f), Utility::Deg2RadF(90.0f), 0.0f};

    unit_.para_.size = SIZE;
    unit_.scale_ = SCALE;

	unit_.isAlive_ = true;

    // ターゲットの真上に配置
    unit_.pos_ = VGet(target_.x, target_.y + OFFSET_Y, target_.z);
    
    // ステート管理用関数======================================
    StateAdd((int)STATE::WAIT, [this](void) { Wait(); });
    StateAdd((int)STATE::FALL, [this](void) { Fall(); });
    StateAdd((int)STATE::STOP, [this](void) { Fly(); });
    StateAdd((int)STATE::END,  [this](void) { End();  });
    // ========================================================

    state_ = STATE::WAIT;   // 状態の初期化

    end_ = false;       // 終了判定(true : 終了 / false : 攻撃中)

    isHit_ = false;     // 攻撃が当たったかどうか

    fallSpeed_ = 0.0f;  // 落ちる速度

    attackCounter_ = COUNT_DOWN;  // 攻撃中の処理用変数の初期化
}

void HandSlap::SubUpdate(void)
{
    if (end_) { return; }

    StateUpdate(static_cast<int>(state_));

    Invi();
}

void HandSlap::SubDraw(void)
{
    if (end_ || !unit_.isAlive_) return;

    MATRIX mat = MGetIdent();

    mat = MMult(MGetScale(unit_.scale_), mat);

    Utility::MatrixRotMult(mat, unit_.angle_);
    Utility::MatrixPosMult(mat, unit_.pos_);

    // モデル描画
    MV1SetMatrix(unit_.model_, mat);

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

    if (!unit_.isAlive_ && end_) { return; }
    MV1DrawModel(unit_.model_);
}

void HandSlap::SubRelease(void)
{   
    MV1DeleteModel(unit_.model_);
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

bool HandSlap::ChanceNow(void)
{
    if (Utility::IsHitCircle(unit_.pos_, 15, target_, 15) &&
        isHit_ == false) {
        GameScene::Slow(10);
        return true;
    }
    return false;
}

// 待機状態
void HandSlap::Wait(void)
{
    // 手がプレイヤーの頭上で待機
    unit_.pos_ = { target_.x, target_.y + OFFSET_Y, target_.z };
    attackCounter_--;

    // 時間がたったら落下状態に遷移
    if (attackCounter_ <= 0) {
        attackCounter_ = 0;
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
        attackCounter_ = COUNT_DOWN;
    }

    if (ChanceNow() == true) {
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
    // 上方向への速度
    const float upSpeed = 30.0f;

    // プレイヤーから手までのベクトル
    VECTOR dir = VSub(unit_.pos_, target_);
    // Y軸は上方向なのでXZ平面だけで計算
    dir.y = 0.0f;

    // 正規化して単位ベクトルに
    float length = sqrtf(dir.x * dir.x + dir.z * dir.z);
    if (length > 0.0f) {
        dir.x /= length;
        dir.z /= length;
    }

    // 飛ぶ距離を掛ける（スピード）
    float flySpeed = 50.0f; // 好きなスピードに調整
    dir.x *= flySpeed;
    dir.z *= flySpeed;

    // 位置更新
    unit_.pos_.x += dir.x;
    unit_.pos_.z += dir.z;
    unit_.pos_.y += upSpeed;

    // 回転（演出用）
    unit_.angle_.z += Utility::Deg2RadF(30);

    // 上限に到達したら終了
    if (unit_.pos_.y > 4000.0f) {
        end_ = true;
    }
}

// 終了処理
void HandSlap::End(void)
{
    attackCounter_--;

    if (attackCounter_ <= 0) {
        attackCounter_ = 0;

        end_ = true;
    }
}

void HandSlap::OnCollision(UnitBase* other)
{
    if (end_) { return; }

	if (auto* player = dynamic_cast<Player*>(other)) {
        isHit_ = true;
	}
}

