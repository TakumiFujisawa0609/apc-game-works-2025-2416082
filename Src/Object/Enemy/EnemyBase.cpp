#include "EnemyBase.h"

#include "../Player/Arm/Base/ArmBase.h"
#include "../Player/Player.h"

EnemyBase::EnemyBase()
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Load(void)
{
	SubLoad();
}

void EnemyBase::Init(void)
{
	SubInit();

}

void EnemyBase::Update(void)
{
	SubUpdate();
}

void EnemyBase::Draw(void)
{
	SubDraw();
}

void EnemyBase::Release(void)
{
	SubRelease();
}

void EnemyBase::OnCollision(UnitBase* other)
{
	if (dynamic_cast<ArmBase*> (other)) {
		unit_.isAlive_ = false;
	}

	//if (dynamic_cast<Player*> (other)) {
	//	unit_.isAlive_ = false;
	//}
}
