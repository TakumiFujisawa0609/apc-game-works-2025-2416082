#include "LeftArm.h"
#include <DxLib.h>

#include "../../Boss/Boss.h"

#include "../../../Scene/Game/GameScene.h"

#include "../../../Manager/Sound/SoundManager.h"

LeftArm::LeftArm(int modelId) :
	ArmBase(modelId)
{
}

LeftArm::~LeftArm()
{
}

void LeftArm::SubLoad(void)
{
	ArmBase::SubLoad();
}

void LeftArm::SubInit(void)
{
	ArmBase::SubInit();
}

void LeftArm::SubUpdate(void)
{
	MATRIX mat_ = MV1GetFrameLocalMatrix(unit_.model_, LEFT_ARM_INDEX);

	// スケール抽出（各軸ベクトルの長さを取る）
	float scale[3];
	for (int i = 0; i < 3; i++) {
		scale[i] = sqrtf(mat_.m[i][0] * mat_.m[i][0] + mat_.m[i][1] * mat_.m[i][1] + mat_.m[i][2] * mat_.m[i][2]);
	}

	// 腕の大きさに応じて当たり判定を大きくする
	unit_.para_.radius = RADIUS * ((scale[0] + scale[1] + scale[2]) / 3.0f);

	MATRIX matp = MV1GetFrameLocalWorldMatrix(unit_.model_, LEFT_HAND_INDEX);

	// 行列から位置（座標）を取り出す
	unit_.pos_ = VGet(matp.m[3][0], matp.m[3][1], matp.m[3][2]);

	ArmBase::SubUpdate();
}

void LeftArm::SubDraw(void)
{
	if (!unit_.isAlive_) { return; }

	DrawSphere3D(unit_.pos_, unit_.para_.radius, 16, 0xfff000, 0xfff000, false);

	ArmBase::SubDraw();
}

void LeftArm::SubRelease(void)
{
	ArmBase::SubRelease();
}


