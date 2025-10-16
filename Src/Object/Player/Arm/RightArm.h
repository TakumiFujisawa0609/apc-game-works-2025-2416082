#pragma once
#include "Base/ArmBase.h"

#include <functional>

class RightArm : public ArmBase
{
public:



	static constexpr VECTOR BONE_UP = { 0.01f, 0.01f, 0.01 };

	RightArm(int modelId);
	~RightArm(void)override;

	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override;
	void SubRelease(void)override;

	void SetAttackTime(int collTime);

private:

};