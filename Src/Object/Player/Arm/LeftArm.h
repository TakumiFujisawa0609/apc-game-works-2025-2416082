#pragma once
#include "../../UnitBase.h"

class LeftArm : public UnitBase
{
public:
	static constexpr int LEFT_ARM_INDEX = 14;   // ç∂òr

	LeftArm(int modelId);
	~LeftArm()override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

private:
};