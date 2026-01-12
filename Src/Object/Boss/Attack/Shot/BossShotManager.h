#pragma once

#include "../../../UnitBase.h"

class BossShot;

class BossShotManager
{
public:

	BossShotManager(VECTOR bossPos, const VECTOR& targetPos);
	~BossShotManager();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	BossShot* GetShot(void) { return shot_; }
private:
	BossShot* shot_;

	VECTOR bossPos_;
	const VECTOR& targetPos_;
};