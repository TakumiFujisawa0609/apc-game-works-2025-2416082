#pragma once
#include "../../UnitBase.h"

class RightArm : public UnitBase
{
public:

	static constexpr int RIGHT_ARM_INDEX = 38;

	RightArm(int modelId);
	~RightArm(void)override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

private:

};