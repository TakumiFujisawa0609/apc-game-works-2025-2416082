#pragma once
#include <vector>

#include "../EnemyBase/EnemyBase.h"
#include "../Enemy.h"

class EnemyManager
{
public:
	static constexpr int ENEMY_MAX = 10;

	EnemyManager();
	~EnemyManager();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

private:
	std::vector<EnemyBase*> enemys_;

	void CreateEnemy(void);
};