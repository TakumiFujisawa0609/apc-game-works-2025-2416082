#pragma once

#include "../../../UnitBase.h"

class BossShotManager : public UnitBase
{
public:

	BossShotManager();
	~BossShotManager() override;

	void OnCollision(UnitBase* other) override;

	bool End(void) { return end_; }

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:

	bool end_;

};