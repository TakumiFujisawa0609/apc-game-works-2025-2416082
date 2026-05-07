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
	ArmBase::SubInit();
}

void RightArm::SubUpdate(void)
{
	MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, MUSCLE_INDEX);

	// スケール抽出（各軸ベクトルの長さを取る）
	float scale[3];
	for (int i = 0; i < 3; i++) {
		scale[i] = sqrtf(mat.m[i][0] * mat.m[i][0] + mat.m[i][1] * mat.m[i][1] + mat.m[i][2] * mat.m[i][2]);
	}

	// 腕の大きさに応じて当たり判定を大きくする
	unit_.para_.radius = RADIUS * ((scale[0] + scale[1] + scale[2]) / 3.0f);

	MATRIX matp = MV1GetFrameLocalWorldMatrix(unit_.model_, RIGHT_HAND_INDEX);
	unit_.pos_ = Utility::GetMatrixPos(matp);	// 右手のフレーム座標取得

	ArmBase::SubUpdate();
}

void RightArm::SubDraw(void)
{
	if (!unit_.isAlive_) { return; }

	ArmBase::SubDraw();
}

void RightArm::SubRelease(void)
{
	ArmBase::SubRelease();
}

