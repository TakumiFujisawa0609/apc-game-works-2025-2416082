#include "LeftArm.h"
#include <DxLib.h>

#include "../../Boss/Boss.h"

#include "../../../Scene/Game/GameScene.h"

#include "../../../Manager/Sound/SoundManager.h"

LeftArm::LeftArm(int modelId)
{
	unit_.model_ = modelId;
}

LeftArm::~LeftArm()
{
}

void LeftArm::Load(void)
{
	auto& sound = SoundManager::GetIns();
	sound.Load(SOUND::HIT);

	mat_ = MV1GetFrameLocalMatrix(unit_.model_, LEFT_ARM_INDEX);

}

void LeftArm::Init(void)
{
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.colliType = CollisionType::ALLY;

	unit_.para_.radius = RADIUS;

	unit_.isAlive_ = false;
}

void LeftArm::Update(void)
{
	if (cnt_ > 0) {
		cnt_--;
		unit_.isAlive_ = true;

		if (cnt_ <= 0) {
			unit_.isAlive_ = false;
			cnt_ = 0;
		}
	}

	// スケール抽出（各軸ベクトルの長さを取る）
	float scale[3];
	for (int i = 0; i < 3; i++) {
		scale[i] = sqrtf(mat_.m[i][0] * mat_.m[i][0] + mat_.m[i][1] * mat_.m[i][1] + mat_.m[i][2] * mat_.m[i][2]);
	}

	// 腕の大きさに応じて当たり判定を大きくする
	unit_.para_.radius = RADIUS * ((scale[0] + scale[1] + scale[2]) / 3.0f);
}

void LeftArm::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	mat_ = MV1GetFrameLocalWorldMatrix(unit_.model_, LEFT_HAND_INDEX);

	// 行列から位置（座標）を取り出す
	unit_.pos_ = VGet(mat_.m[3][0], mat_.m[3][1], mat_.m[3][2]);

	DrawSphere3D(unit_.pos_, unit_.para_.radius, 16, 0xfff000, 0xfff000, false);
}

void LeftArm::Release(void)
{
	auto& sound = SoundManager::GetIns();
	for (int i = 0; i < SOUND::MAX; i++) {
		sound.Delete((SOUND)i);
	}
}

void LeftArm::OnCollision(UnitBase* other)
{
	if (!unit_.isAlive_) { return; }
	auto& sound = SoundManager::GetIns();
	if (dynamic_cast<Boss*>(other))
	{
		addArmScale_(BONE_UP);
		return;
	}
}

// 呼び出して時間を設定した瞬間から、攻撃判定が出てくる
void LeftArm::SetAttackTime(int collTime)
{
	cnt_ = collTime;
}

