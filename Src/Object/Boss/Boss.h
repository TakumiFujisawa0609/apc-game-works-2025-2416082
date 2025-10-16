#pragma once
#include "../UnitBase.h"

#include <Vector>

class BossRightHand;

class Boss : public UnitBase
{
public:
	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };

	static constexpr float RADIUS = 200.0f;
	static constexpr VECTOR SCALE = { 4.0f,4.0f,4.0f };
	static constexpr float HALF_LEN = 300.0f;

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 300.0f, 1000.0f };

	static constexpr int HP_MAX = 300;

	static constexpr int INVI_TIME = 30;


	struct DamageText {
		VECTOR pos_;      // 表示位置
		int damage_;       // ダメージ量
		int drawTime_;        // 表示時間（フレーム数）
	};

	enum STATE
	{
		NONE,
		IDLE,
		ATTACK,
		DAMAGE,
		DEATH,
	};

	enum  ATTACK
	{
		NON,



		MAX
	};

	Boss();
	~Boss() override;

	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

	void UIDraw(void);

	void OnCollision(UnitBase* other) override;

	BossRightHand* GetRightHand(void) { return rHand_; }

	void SetTarget(VECTOR target) { target_ = target; }
	void SetMuscleRatio(float ratio) { playerMuscleRatio_ = ratio; }

private:
	BossRightHand* rHand_;

	VECTOR target_;
	float playerMuscleRatio_;

	int color1;

	std::vector<DamageText> damageTexts_;
};