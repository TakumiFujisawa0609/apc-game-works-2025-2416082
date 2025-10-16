#pragma once
#include "Base/ArmBase.h"

#include <functional>

class LeftArm : public ArmBase
{
public:


	LeftArm(int modelId);
	~LeftArm()override;

	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override;
	void SubRelease(void)override;


	void SetAttackTime(int collTime);

	//void SetAddArmScaleFunc(std::function<void(VECTOR scale)> func) { addArmScale_ = std::move(func); }

private:


	//std::function<void(VECTOR scale)> addArmScale_;
};