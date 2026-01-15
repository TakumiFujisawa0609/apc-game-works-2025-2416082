#pragma once

#include "../../../UnitBase.h"

class BossShot;

class BossShotManager
{
public:

	BossShotManager(const Base& boss, const VECTOR& targetPos);
	~BossShotManager();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

 	BossShot* GetShot(void) { return shot_; }
private:

	const int SHOT_MAX = 3;

	BossShot* shot_;

	const Base& boss_;
	const VECTOR& targetPos_;
};