#include "Boss.h"
#include "../../Utility/Utility.h"

#include "../Player/Arm/LeftArm.h"

Boss::Boss()
{
}

Boss::~Boss()
{
}

void Boss::Load(void)
{
}

void Boss::Init(void)
{
	unit_.para_.colliShape = CollisionShape::CAPSULE;

	unit_.para_.size = { 200.0f, 600.0f,200.0f };
	unit_.para_.radius = 100.0f;
	unit_.para_.capsuleHalfLen = unit_.para_.size.y - (unit_.para_.radius * 2);
	unit_.pos_ = { 0.0f, 50.0f, 200.0f};

	unit_.hp_ = HP_MAX;

	unit_.isAlive_ = true;

	color1 = 0xfff000;
}

void Boss::Update(void)
{
	Invi();
}

void Boss::Draw(void)
{
	if (!unit_.isAlive_)return;
	VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen / 2,0.0f });
	VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen / 2,0.0f });
	DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, color1, color1, false);
}

void Boss::Release(void)
{
}

void Boss::OnCollision(UnitBase* other)
{
	if (dynamic_cast<LeftArm*>(other))
	{
		unit_.hp_ -= 10;
		unit_.inviciCounter_ = 30;
		return;
	}
}
