#include "RightArm.h"

#include "../../Boss/Boss.h"

#include "../../../Manager/Sound/SoundManager.h"
#include "LeftArm.h"

RightArm::RightArm(int modelId) :
	ArmBase(modelId)
{
}

RightArm::~RightArm(void)
{
}

void RightArm::SubLoad(void)
{
	ArmBase::SubLoad();
}

void RightArm::SubInit(void)
{
	//unit_.para_.colliShape = CollisionShape::SPHERE;
	//unit_.para_.colliType = CollisionType::ALLY;

	//unit_.isAlive_ = false;

	//unit_.para_.radius = RADIUS;

	ArmBase::SubInit();
}

void RightArm::SubUpdate(void)
{
	// --- スケール連動処理 ---
	MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, RIGHT_ARM_INDEX);

	// スケール抽出（各軸ベクトルの長さを取る）
	float scale[3];
	for (int i = 0; i < 3; i++) {
		scale[i] = sqrtf(mat.m[i][0] * mat.m[i][0] + mat.m[i][1] * mat.m[i][1] + mat.m[i][2] * mat.m[i][2]);
	}

	// 腕の大きさに応じて当たり判定を大きくする
	unit_.para_.radius = RADIUS * ((scale[0] + scale[1] + scale[2]) / 3.0f);

	MATRIX matp = MV1GetFrameLocalWorldMatrix(unit_.model_, RIGHT_HAND_INDEX);
	unit_.pos_ = VGet(matp.m[3][0], matp.m[3][1], matp.m[3][2]);

	ArmBase::SubUpdate();
}

void RightArm::SubDraw(void)
{
	if (!unit_.isAlive_) { return; }

	DrawSphere3D(unit_.pos_, unit_.para_.radius, 16, 0xfff000, 0xfff000, false);

	ArmBase::SubDraw();
}

void RightArm::SubRelease(void)
{
	ArmBase::SubRelease();
}

// 呼び出して時間を設定した瞬間から、攻撃判定が出てくる
void RightArm::SetAttackTime(int collTime)
{
	cnt_ = collTime;
}