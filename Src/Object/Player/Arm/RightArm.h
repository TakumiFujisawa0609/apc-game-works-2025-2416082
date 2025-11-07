#pragma once
#include "Base/ArmBase.h"

#include <functional>

class RightArm : public ArmBase
{
public:

	RightArm(int modelId);
	~RightArm()override;


protected:
	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override;
	void SubRelease(void)override;
private:
};