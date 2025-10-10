#pragma once
#include "../../UnitBase.h"

#include <functional>

class RightArm : public UnitBase
{
public:

	static constexpr int RIGHT_ARM_INDEX = 35;
	static constexpr int RIGHT_HAND_INDEX = 38;

	static constexpr float RADIUS = 30;

	static constexpr VECTOR BONE_UP = { 0.01f, 0.01f, 0.01 };

	RightArm(int modelId);
	~RightArm(void)override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	void SetAttackTime(int collTime);

	void SetAddArmScaleFunc(std::function<void(VECTOR scale)> func) { addArmScale_ = std::move(func); }
private:

	int cnt_;

	std::function<void(VECTOR scale)> addArmScale_;
};