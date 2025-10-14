#pragma once
#include "../UnitBase.h"

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


	struct DamageText {
		VECTOR pos;      // 表示位置
		int value;       // ダメージ量
		int life;        // 表示時間（フレーム数）
	};

	enum  ATTACK
	{
		NON,



		MAX
	};

	Boss();
	~Boss() override;

	void Load(void) override;
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

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

	std::vector<DamageText> damageTexts_;  // ←ここです
};