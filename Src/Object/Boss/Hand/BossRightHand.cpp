#include "BossRightHand.h"

BossRightHand::BossRightHand()
{
}

BossRightHand::~BossRightHand()
{
}

void BossRightHand::Load(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Boss/hand.mv1");
}

void BossRightHand::Init(void)
{
	unit_.para_.colliShape = CollisionShape::OBB;
	unit_.para_.colliType = CollisionType::ENEMY;

    unit_.scale_ = SCALE;

	unit_.isAlive_ = true;
}

void BossRightHand::Update(void)
{

}

void BossRightHand::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	MATRIX localmat = MGetIdent();
	localmat = MMult(localmat, MGetScale(unit_.scale_));
	localmat = MMult(localmat, MGetRotX(unit_.angle_.x));
	localmat = MMult(localmat, MGetRotY(unit_.angle_.y));
	localmat = MMult(localmat, MGetRotZ(unit_.angle_.z));

	VECTOR worldPos = VTransform(LOCAL_POS, baseMat_);

	localmat.m[3][0] = worldPos.x;
	localmat.m[3][1] = worldPos.y;
	localmat.m[3][2] = worldPos.z;

	MATRIX drawMat = MMult(localmat, baseMat_);

	DrawSphere3D(unit_.pos_, 10, 16, 0xff00ff, 0xff00ff, true);

	MV1SetMatrix(unit_.model_, drawMat);

	MV1DrawModel(unit_.model_);
}

void BossRightHand::Release(void)
{
    MV1DeleteModel(unit_.model_);
}

void BossRightHand::OnCollision(UnitBase* other)
{
}
