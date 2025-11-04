#pragma once
#include <vector>

#include "../EnemyBase.h"
#include "../Enemy.h"

class EnemyManager
{
public:
	static constexpr int ENEMY_MAX = 10;

	EnemyManager(const VECTOR& target);
	~EnemyManager();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	std::vector<EnemyBase*> GetEnemy(void) { return enemys_; }

private:
	std::vector<EnemyBase*> enemys_;

	void CreateEnemy(void);

	const VECTOR& target_;
};