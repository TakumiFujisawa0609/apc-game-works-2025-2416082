#include "EnemyManager.h"

EnemyManager::EnemyManager()
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Load(void)
{
	enemys_.resize(ENEMY_MAX);
	for (size_t i = 0; i < enemys_.size(); ++i) {
		enemys_[i] = new Enemy(); // ¡Œã‚±‚±‚ÅŽí—Þ•ª‚¯‚Å‚«‚é
		enemys_[i]->Load();
	}
}

void EnemyManager::Init(void)
{
	for (auto& e : enemys_) {
		e->Init();
	}
}

void EnemyManager::Update(void)
{
	for (auto& e : enemys_) {
		e->Update();
	}
}

void EnemyManager::Draw(void)
{
	for (auto& e : enemys_) {
		e->Draw();
	}
}

void EnemyManager::Release(void)
{
	for (auto& e : enemys_) {
		if (e) {
			e->Release();
			delete e;
			e = nullptr;
		}
	}
}

void EnemyManager::CreateEnemy(void)
{

}
