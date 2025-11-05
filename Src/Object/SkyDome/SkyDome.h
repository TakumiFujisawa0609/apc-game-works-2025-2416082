#pragma once
#include "../UnitBase.h"
class SkyDome : public UnitBase
{
public:

	SkyDome();
	~SkyDome() override;

	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

	void OnCollision(UnitBase* other) {};

private:
	int tex_;
};