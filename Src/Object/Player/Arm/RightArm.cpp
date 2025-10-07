#include "RightArm.h"

#include "../../Boss/Boss.h"

#include "../../../Manager/Sound/SoundManager.h"

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

	unit_.isAlive_ = true;

	unit_.para_.radius = 30;
}

void RightArm::Update(void)
{
}

void RightArm::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	MATRIX mat = MV1GetFrameLocalWorldMatrix(unit_.model_, RIGHT_ARM_INDEX);

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
		sound.Play(SOUND::HIT);
		return;
	}
}
