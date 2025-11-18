#pragma once

#include "../../../UnitBase.h"

class BossShot : public UnitBase
{

	BossShot();
	~BossShot() override;

	void OnCollision(UnitBase* other) override;

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:

};