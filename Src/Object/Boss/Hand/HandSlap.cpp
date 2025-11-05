#include "HandSlap.h"

#include "../../Player/Player.h"

#include "../../../Scene/Game/GameScene.h"

HandSlap::HandSlap(const VECTOR& target) :
    target_(target)
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

    unit_.para_.size = { 500,500,500 };
    unit_.scale_ = SCALE;

	unit_.isAlive_ = true;

    // ターゲットの真上に配置
    const float offsetY = 500.0f;
    unit_.pos_ = VGet(target_.x, target_.y + offsetY, target_.z);

    unit_.pos_.z = -1000.0f;
}

void HandSlap::SubUpdate(void)
{
    if (end_) { return; }
    static bool is = false;
    if (CheckHitKey(KEY_INPUT_L)) {
        is = true;
    }
    if (!is) { return; }

    if (unit_.pos_.y > 0) {
        unit_.pos_.y -= FALL_SPEED;
        if (unit_.pos_.y <= 0) {
            GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG, 60);
            end_ = true;
        }
    }

	Invi();
}

void HandSlap::SubDraw(void)
{
    if (!unit_.isAlive_) return;

    MATRIX mat = MGetIdent();

    mat = MMult(MGetScale(unit_.scale_), mat);

    mat = MMult(MGetRotX(unit_.angle_.x), mat);
    mat = MMult(MGetRotY(unit_.angle_.y), mat);
    mat = MMult(MGetRotZ(unit_.angle_.z), mat);

    mat.m[3][0] = unit_.pos_.x;
    mat.m[3][1] = unit_.pos_.y;
    mat.m[3][2] = unit_.pos_.z;

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
}

void HandSlap::SubRelease(void)
{   
    MV1DeleteModel(unit_.model_);
}

void HandSlap::OnCollision(UnitBase* other)
{
	if (dynamic_cast<Player*>(other)) {
	}
}

