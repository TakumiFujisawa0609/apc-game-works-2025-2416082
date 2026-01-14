#pragma once
#include "../../../UnitBase.h"

class BossShot : public UnitBase
{
public:

	enum class STATE
	{
		WAIT,
		MOVE,

		MAX
	};

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

	STATE state_;

	void Wait(void);
	void Move(void);

	bool isEnd_;

	VECTOR bossPos_;
	const VECTOR& targetPos_;
};