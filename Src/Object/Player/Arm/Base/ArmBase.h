#pragma once

#include "../../../UnitBase.h"

class ArmBase : public UnitBase
{
public:

	static constexpr VECTOR MAX_ARM_MUSCLE = { 4.0f,4.0f,4.0f };	// 筋肉のスケールの最大値
	static constexpr VECTOR MIN_ARM_MUSCLE = { 1.0f,1.0f,1.0f };	// 筋肉のスケールの最低値

	static constexpr int RIGHT_ARM_INDEX = 35;
	static constexpr int RIGHT_HAND_INDEX = 38;
	static constexpr int LEFT_ARM_INDEX = 11;   
	static constexpr int LEFT_HAND_INDEX = 13;  

	static constexpr float RADIUS = 30;

	static constexpr VECTOR BONE_UP = { 0.01f, 0.01f, 0.01 };



	ArmBase(int modelId);
	~ArmBase() override;

	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

	void OnCollision(UnitBase* other) override;

	void AddArmScale(VECTOR scale);
	void AddBoneScale(int index, VECTOR scale);

	void UIDraw(void);


protected:
	
	int *state_;

	// デバッグ用変数
	int frameScrollIndex_;
	float muscleRatio_;
	int cnt_;
};