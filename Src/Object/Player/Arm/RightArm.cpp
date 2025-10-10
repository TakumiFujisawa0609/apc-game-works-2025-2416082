#include "RightArm.h"

#include "../../Boss/Boss.h"

#include "../../../Manager/Sound/SoundManager.h"
#include "LeftArm.h"

RightArm::RightArm(int modelId)
{
	unit_.model_ = modelId;
}

RightArm::~RightArm(void)
{
}

void RightArm::Load(void)
{
	auto& sound = SoundManager::GetIns();
	sound.Load(SOUND::HIT);
}

void RightArm::Init(void)
{
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.colliType = CollisionType::ALLY;

	unit_.isAlive_ = false;

	unit_.para_.radius = RADIUS;
}

void RightArm::Update(void)
{
	if (cnt_ > 0) {
		cnt_--;
		unit_.isAlive_ = true;

		if (cnt_ < 0) {
			unit_.isAlive_ = false;
			cnt_ = 0;
		}
	}

	// --- スケール連動処理 ---
	MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, RIGHT_ARM_INDEX);

	// スケール抽出（各軸ベクトルの長さを取る）
	float scale[3];
	for (int i = 0; i < 3; i++) {
		scale[i] = sqrtf(mat.m[i][0] * mat.m[i][0] + mat.m[i][1] * mat.m[i][1] + mat.m[i][2] * mat.m[i][2]);
	}

	// 腕の大きさに応じて当たり判定を大きくする
	unit_.para_.radius = RADIUS * ((scale[0] + scale[1] + scale[2]) / 3.0f);
}

void RightArm::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	MATRIX mat = MV1GetFrameLocalWorldMatrix(unit_.model_, RIGHT_HAND_INDEX);

	unit_.pos_ = VGet(mat.m[3][0], mat.m[3][1], mat.m[3][2]);

	DrawSphere3D(unit_.pos_, unit_.para_.radius, 16, 0xfff000, 0xfff000, false);
}

void RightArm::Release(void)
{
	auto& sound = SoundManager::GetIns();
	for (int i = 0; i < SOUND::MAX; i++) {
		sound.Delete((SOUND)i);
	}
}

void RightArm::OnCollision(UnitBase* other)
{
	auto& sound = SoundManager::GetIns();
	if (dynamic_cast<Boss*>(other))
	{
		sound.Stop(SOUND::HIT);
		sound.Play(SOUND::HIT);
		addArmScale_(BONE_UP);
		return;
	}
}

// 呼び出して時間を設定した瞬間から、攻撃判定が出てくる
void RightArm::SetAttackTime(int collTime)
{
	cnt_ = collTime;
}