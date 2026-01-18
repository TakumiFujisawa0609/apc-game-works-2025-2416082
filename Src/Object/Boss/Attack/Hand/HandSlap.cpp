#include "HandSlap.h"

#include "../../../../Application/Application.h"
#include "../../../Player/Player.h"

#include "../../../../Scene/Game/GameScene.h"

HandSlap::HandSlap(int model, const VECTOR& target, const int& voiceLevel) :
    target_(target),
    voiceLevel_(voiceLevel),
    end_(false),
    state_(STATE::WAIT),
    counter_(0),
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
    
    unit_.pos_ = { target_.x, target_.y + OFFSET_Y, target_.z };

    counter_ = COUNT_DOWN;

    StateAdd((int)STATE::WAIT, [this](void) { Wait(); });
    StateAdd((int)STATE::FALL, [this](void) { Fall(); });
    StateAdd((int)STATE::STOP, [this](void) { Stop(); });
    StateAdd((int)STATE::END,  [this](void) { End();  });

    state_ = STATE::WAIT;

    end_ = false;       // 終了判定(true : 終了 / false : 攻撃中)

    fallSpeed_ = 0.0f;
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

// 待機状態
void HandSlap::Wait(void)
{
    // 手がプレイヤーの頭上で待機
    unit_.pos_ = { target_.x, target_.y + OFFSET_Y, target_.z };
    counter_--;

    // 時間がたったら落下状態に遷移
    if (counter_ <= 0) {
        counter_ = 0;
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
        counter_ = COUNT_DOWN;
    }

    // プレイヤーのボイスが一定以上なら吹っ飛ぶ
    if (voiceLevel_ > 2500) {
        state_ = STATE::STOP;
        GameScene::HitStop(10);
        GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 20);
    }
}

// 吹っ飛び状態
void HandSlap::Stop(void)
{
    unit_.pos_.y += 30;
    unit_.angle_.z += Utility::Deg2RadF(30);

    if (unit_.pos_.y > 5000) {
        end_ = true;
    }
}

// 終了処理
void HandSlap::End(void)
{
    counter_--;

    if (counter_ <= 0) {
        counter_ = 0;

        end_ = true;
    }
}

void HandSlap::OnCollision(UnitBase* other)
{
    if (end_) { return; }

	if (auto* player = dynamic_cast<Player*>(other)) {

	}
}

