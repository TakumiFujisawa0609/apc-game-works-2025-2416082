#include "EnemyBase.h"

#include "../../Player/Player.h"

EnemyBase::EnemyBase(VECTOR target) :
	target_(target)
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::SubInit(void)
{
	unit_.para_.colliShape = CollisionShape::CAPSULE;
	unit_.para_.colliType = CollisionType::ENEMY;
	unit_.para_.capsuleHalfLen = 20.0f;
	unit_.para_.radius = 30.0f;

	unit_.isAlive_ = true;
	unit_.hp_ = 100;
	unit_.pos_ = { 100.0f,100.0f,100.0f };
	unit_.scale_ = { 1.0f,1.0f,1.0f };
	unit_.angle_ = Utility::VECTOR_ZERO;

}

void EnemyBase::SubUpdate(void)
{
}

void EnemyBase::SubDraw(void)
{
}

void EnemyBase::SubRelease(void)
{
}

void EnemyBase::SubLoad(void)
{
}

void EnemyBase::OnCollision(UnitBase* other)
{

}
