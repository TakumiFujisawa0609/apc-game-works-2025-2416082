#include "BossShotManager.h"

#include "BossShot.h"

BossShotManager::BossShotManager(const Base& boss, const VECTOR& targetPos) :
	boss_(boss),
	targetPos_(targetPos)
	
{
}

BossShotManager::~BossShotManager()
{
}

void BossShotManager::Load(void)
{
	int modelId = MV1LoadModel("Data/Model/Boss/FireBall/FireBall.mv1");

	Utility::ClassNew(shot_, modelId, boss_, targetPos_)->Load();

}

void BossShotManager::Init(void)
{
	shot_->Init();
}

void BossShotManager::Update(void)
{
	shot_->Update();
}

void BossShotManager::Draw(void)
{
	shot_->Draw();
}

void BossShotManager::Release(void)
{
	Utility::SafeDeleteInstance(shot_);
}

