#pragma once
#include "../UnitBase.h"
#include <map>
#include <DxLib.h>

class AnimationController;

class Player : public UnitBase
{
public:

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, 0.0f }; //初期座標
	static constexpr float RADIUS_SIZE = 100.0f;				//プレイヤーの半径（仮）

	static constexpr int MAX_MUSCLE = 100;

	static constexpr float MOVE_SPEED = 16.0f;

	enum class STATE
	{
		IDLE,
		MOVE,
		ATTACK,
	};

	Player();
	~Player() override;

	void Load(void) override;
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void OnCollision(UnitBase* other) override;

	void Muscle(void);

	const VECTOR GetPos(void) { return unit_.pos_; }
	const VECTOR GetAngle(void) { return unit_.angle_; }

private:

	AnimationController* animation_;

	STATE state_;

	VECTOR move_;

	bool attackScaleApplied_ = false;

	// 関数ポインタ
	using StateFunc = void(Player::*)();
	std::map<STATE, StateFunc> stateFuncs_;

	void Idle(void);
	void Move(void);
	void Attack(void);

	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };

	// 状態遷移用の関数
	void StateManager(void);
	void DoWalk(void);
	void DoIdle(void);
	void DoAttack(void);

	int prevSpace, nowSpace;
};