#pragma once
#include "../UnitBase.h"

#include <Vector>

class HandSlap;
class RotateHand;

class Boss : public UnitBase
{
public:
	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };

	static constexpr float RADIUS = 250.0f;
	static constexpr VECTOR SCALE = { 5.0f,5.0f,5.0f };
	static constexpr float HALF_LEN = 300.0f;

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 600.0f, 0.0f };

	static constexpr int HP_MAX = 300;

	static constexpr int INVI_TIME = 30;

	static constexpr int NEXT_ATTACK_TIME = 120;

	enum class STATE
	{
		IDLE,
		ATTACK,
		DAMAGE,
		DEATH,
	};

	enum class ATTACK
	{
		NON,

		SLAP,
		ROTA_HAND,
		BALL,

		MAX
	};

	Boss(const VECTOR& target);
	~Boss() override;

	void UIDraw(void);

	void OnCollision(UnitBase* other) override;

	HandSlap* GetRightHand(void) { return slap_; }

	void SetMuscleRatio(float ratio) { playerMuscleRatio_ = ratio; }
	void SetVoiceLevel(float voiceLevel) { voiceLevel_ = voiceLevel; }

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:
	HandSlap* slap_;
	RotateHand* rotaHnad_;

	// ボスの手用のモデルハンドル
	int handModel_;

	float playerMuscleRatio_;

	STATE state_;
	ATTACK attackState_;

	int attackCounter_;		// 攻撃用カウンタ
	bool isAttackInit_;		// (true : 攻撃開始後 / false : 攻撃開始前)
	bool isAttackEnd_;		// (true : 攻撃終了後 / false : 攻撃終了前)

	int color1;

	const VECTOR& target_;
	int voiceLevel_;

	void SetMatrix(void);

#pragma region ステート関数
	void Attack(void);
	void Idle(void);
	void Damage(void);
	void Death(void);
#pragma endregion

#pragma region 攻撃関係の処理を別で管理
	Boss::ATTACK AttackLottery(void);

	void AttackLoad(void);
	void AttackInit(void);
	void AttackDraw(void);
	void AttackRelease(void);
#pragma endregion
};