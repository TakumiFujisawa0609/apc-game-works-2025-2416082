#pragma once
#include "Base/ArmBase.h"

#include <functional>

class LeftArm : public ArmBase
{
public:


	LeftArm(int modelId);
	~LeftArm()override;


protected:
	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override;
	void SubRelease(void)override;
private:
};