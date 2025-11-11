#include "HandSlap.h"

#include "../../../../Application/Application.h"
#include "../../../Player/Player.h"

#include "../../../../Scene/Game/GameScene.h"

HandSlap::HandSlap(const VECTOR& target, const int& voiceLevel) :
    target_(target),
    voiceLevel_(voiceLevel)
{
}

HandSlap::~HandSlap()
{
}

void HandSlap::SubLoad(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Boss/hand.mv1");
}

void HandSlap::SubInit(void)
{
	unit_.para_.colliShape = CollisionShape::OBB;
	unit_.para_.colliType = CollisionType::ENEMY;

    unit_.angle_ = { Utility::Deg2RadF(-90.0f), Utility::Deg2RadF(90.0f), 0.0f};

    unit_.para_.radius = 200.0f;

    unit_.para_.size = COLLISION_SIZE;
    unit_.scale_ = SCALE;

	unit_.isAlive_ = true;

    // ターゲットの真上に配置
    unit_.pos_ = VGet(target_.x, target_.y + OFFSET_Y, target_.z);

    unit_.pos_.z = -1000.0f;

    counter_ = COUNT_DOWN;

    StateAdd((int)HAND_STATE::WAIT, [this](void) { Wait(); });
    StateAdd((int)HAND_STATE::FALL, [this](void) { Fall(); });
    StateAdd((int)HAND_STATE::STOP, [this](void) { Stop(); });
    StateAdd((int)HAND_STATE::END,  [this](void) { End();  });

    handState_ = HAND_STATE::WAIT;

    end_ = false;       // 終了判定(true : 終了 / false : 攻撃中)
    isHit_ = false;     // プレイヤーに当たったらそれ以降true

    fallSpeed_ = 0.0f;
}

void HandSlap::SubUpdate(void)
{
    if (end_) { return; }

    StateUpdate(static_cast<int>(handState_));

    Invi();
}

void HandSlap::SubDraw(void)
{
    if (end_) return;

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

    if (!unit_.isAlive_) { return; }
    MV1DrawModel(unit_.model_);

#endif


}

void HandSlap::SubRelease(void)
{   
    MV1DeleteModel(unit_.model_);
}

void HandSlap::MarkerDraw(void)
{
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
    if (handState_ == HAND_STATE::WAIT) {
        VECTOR markerPos = { target_.x, 0.0f, target_.z };
        float radius = 300.0f;
        int color = 0xff3333;

        DrawCone3D(unit_.pos_, markerPos, radius, 0, color, color, true);
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    if (handState_ == HAND_STATE::WAIT) {
        SetFontSize(128);
        DrawString(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, "攻撃が来る\n！叫ぶんだ！！！", 0xffffff);
        SetFontSize(0);
    }
}

void HandSlap::Wait(void)
{
    counter_--;
    unit_.pos_ = VGet(target_.x, target_.y + OFFSET_Y, target_.z);

    if (CheckHitKey(KEY_INPUT_L) || counter_ <= 0) {
        counter_ = 0;
        handState_ = HAND_STATE::FALL;
    }
}

void HandSlap::Fall(void)
{
    fallSpeed_ += GRAVITY;
    unit_.pos_.y -= fallSpeed_;
    if (unit_.pos_.y <= 0) {
        unit_.pos_.y = 0;
        GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG, 60);
        handState_ = HAND_STATE::END;
        counter_ = COUNT_DOWN;
    }

    if (voiceLevel_ > 4000) {
        handState_ = HAND_STATE::STOP;
    }
}

void HandSlap::Stop(void)
{
    unit_.pos_.y += 30;
    if (unit_.pos_.y > 5000) {
        end_ = true;
    }
}

void HandSlap::End(void)
{
    counter_--;
    if (counter_ <= 0) {
        counter_ = 0;

        isHit_ = true;
        end_ = true;
    }
}

void HandSlap::OnCollision(UnitBase* other)
{
    if (end_ || isHit_) { return; }

	if (auto* player = dynamic_cast<Player*>(other)) {

        isHit_ = true;
	}
}

