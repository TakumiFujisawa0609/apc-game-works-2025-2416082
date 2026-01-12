#include "BossShotManager.h"

#include "BossShot.h"

BossShotManager::BossShotManager(VECTOR bossPos, const VECTOR& targetPos) :
	bossPos_(bossPos),
	targetPos_(targetPos)
	
{
}

BossShotManager::~BossShotManager()
{
}

void BossShotManager::Load(void)
{
	Utility::ClassNew(shot_, bossPos_, targetPos_)->Load();
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

