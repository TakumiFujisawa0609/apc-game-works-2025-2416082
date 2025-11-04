#include "HandSlap.h"

#include "../../Player/Player.h"

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

    unit_.angle_ = { Utility::Deg2RadF(-90.0f), Utility::Deg2RadF(90.0f), 0.0f };

    unit_.para_.radius = 200.0f;

    unit_.para_.center = unit_.pos_;
    unit_.para_.size = { 200,500,200 };
    unit_.scale_ = SCALE;

	unit_.isAlive_ = true;


}

void HandSlap::SubUpdate(void)
{
    // ターゲットの真上に配置
    const float offsetY = 500.0f;
    unit_.pos_ = VGet(target_.x, target_.y + offsetY, target_.z);

    if (unit_.pos_.y > 0 && !end_) {
        unit_.pos_.y -= 5;
        if (unit_.pos_.y <= 0) {
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
    DrawLineBox(
        unit_.para_.center.x - unit_.para_.size.x,
        unit_.para_.center.y - unit_.para_.size.y,
        unit_.para_.center.x + unit_.para_.size.x,
        unit_.para_.center.y + unit_.para_.size.y,
        0xffffff
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

