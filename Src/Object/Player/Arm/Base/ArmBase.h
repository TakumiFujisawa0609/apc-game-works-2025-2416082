#pragma once

#include "../../../UnitBase.h"

class ArmBase : public UnitBase
{
public:



	static constexpr int RIGHT_ARM_INDEX = 4;//35;
	static constexpr int RIGHT_HAND_INDEX = 38;
	static constexpr int LEFT_ARM_INDEX = 4;//11;   
	static constexpr int LEFT_HAND_INDEX = 13;  

	static constexpr float RADIUS = 30;

	static constexpr VECTOR BONE_UP = { 0.01f, 0.01f, 0.01 };

	ArmBase(int modelId);
	~ArmBase() override;

	void OnCollision(UnitBase* other) override;
	void UIDraw(void);

	// 呼び出して時間を設定した瞬間から、攻撃判定が出てくる
	void SetAttackTime(int collTime);

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:
	void AddArmScale(VECTOR scale);

	int* state_;

	bool isHit_;

	// デバッグ用変数
	int frameScrollIndex_;
	float muscleRatio_;
	int cnt_;
	
};