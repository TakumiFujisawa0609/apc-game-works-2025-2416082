#pragma once
#include "../UnitBase.h"

#include <Vector>

class HandSlap;

class Boss : public UnitBase
{
public:
	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };

	static constexpr float RADIUS = 300.0f;
	static constexpr VECTOR SCALE = { 5.0f,5.0f,5.0f };
	static constexpr float HALF_LEN = 300.0f;

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 600.0f, 0.0f };

	static constexpr int HP_MAX = 300;

	static constexpr int INVI_TIME = 30;


	struct DamageText {
		VECTOR pos_;      // 表示位置
		int damage_;       // ダメージ量
		int drawTime_;        // 表示時間（フレーム数）
	};

	enum STATE
	{
		IDLE,
		ATTACK,
		DAMAGE,
		DEATH,
	};

	enum  ATTACK
	{
		NON,

		SLAP,

		MAX
	};

	Boss();
	~Boss() override;

	void UIDraw(void);

	void OnCollision(UnitBase* other) override;

	HandSlap* GetRightHand(void) { return hand_; }
	void SetMuscleRatio(float ratio) { playerMuscleRatio_ = ratio; }
	void SetPlayerPos(VECTOR pos) { target_ = pos; }

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:
	HandSlap* hand_;

	VECTOR target_;
	float playerMuscleRatio_;

	STATE state_;

	int color1;

	std::vector<DamageText> damageTexts_;



	void SetMatrix(void);

#pragma region ステート関数
	void Attack(void);
	void Idle(void);
	void Damage(void);
	void Death(void);
#pragma endregion
};