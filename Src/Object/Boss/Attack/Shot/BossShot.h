#pragma once
#include "../../../UnitBase.h"

class BossShot : public UnitBase
{
public:

	BossShot(VECTOR bossPos, const VECTOR& targetPos);
	~BossShot() override;

	void OnCollision(UnitBase* other) override;

	bool End(void) { return isEnd_; }

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:
	bool isEnd_;

	VECTOR bossPos_;
	const VECTOR& targetPos_;
};