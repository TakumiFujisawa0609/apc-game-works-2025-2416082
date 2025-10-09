#pragma once
#include "../../UnitBase.h"

#include <functional>

class LeftArm : public UnitBase
{
public:
	static constexpr int LEFT_ARM_INDEX = 11;   // ç∂òr
	static constexpr int LEFT_HAND_INDEX = 13;   // ç∂òr

	LeftArm(int modelId);
	~LeftArm()override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	void SetAttackTime(int collTime);

	void SetKinniku(std::function<void(int index, VECTOR scale)> func) { boneScaleChange_ = std::move(func); }
private:

	int cnt_;

	std::function<void(int index, VECTOR scale)> boneScaleChange_;
};