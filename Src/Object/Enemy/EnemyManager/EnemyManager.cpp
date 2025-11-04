#include "EnemyManager.h"

#include "../../../Manager/Input/InputManager.h"

EnemyManager::EnemyManager(const VECTOR& target) :
	target_(target)
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Load(void)
{
	enemys_.resize(ENEMY_MAX);
	for (size_t i = 0; i < enemys_.size(); ++i) {
		enemys_[i] = new Enemy(target_); // ¡Œã‚±‚±‚Åí—Ş•ª‚¯‚Å‚«‚é
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

	// ---------- “G“¯m‚Ì‰Ÿ‚µo‚µˆ— ----------
	for (size_t i = 0; i < enemys_.size(); ++i) {
		EnemyBase* e1 = enemys_[i];
		if (!e1->GetUnit().isAlive_) continue;

		for (size_t j = i + 1; j < enemys_.size(); ++j) {
			EnemyBase* e2 = enemys_[j];
			if (!e2->GetUnit().isAlive_) continue;

			VECTOR diff = VSub(e2->GetUnit().pos_, e1->GetUnit().pos_);
			float dist = VSize(diff);

			float r1 = e1->GetUnit().para_.radius;
			float r2 = e2->GetUnit().para_.radius;

			float minDist = r1 + r2;

			if (dist < minDist && dist > 0.0001f) {
				// d‚È‚è—Ê
				float overlap = minDist - dist;

				// ³‹K‰»•ûŒü
				VECTOR dir = VScale(diff, 1.0f / dist);

				// ‰Ÿ‚µo‚µi”¼•ª‚¸‚Â“®‚©‚·j
				e1->SetPos(VSub(e1->GetUnit().pos_, VScale(dir, overlap * 0.5f)));
				e2->SetPos(VAdd(e2->GetUnit().pos_, VScale(dir, overlap * 0.5f)));
			}
		}
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
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_E)) {
	}
}
