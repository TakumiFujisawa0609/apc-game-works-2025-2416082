#pragma once
#include "../UnitBase.h"
#include <map>

class AnimationController;

class Player : public UnitBase
{
public:

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, 0.0f }; //初期座標
	static constexpr float RADIUS_SIZE = 100.0f;				//プレイヤーの半径（仮）

	static constexpr int MAX_MUSCLE = 100;
	static constexpr VECTOR SMALL_MUSCLE = { 1.0f, 1.0f, 1.0f };
	static constexpr VECTOR MEDIUM_MUSCLE = { 2.0f, 2.0f, 2.0f };
	static constexpr VECTOR BIG_MUSCLE = { 3.0f, 3.0f, 3.0f };

	static constexpr float MOVE_SPEED = 16.0f;

	enum class STATE
	{
		IDLE,
		MOVE,

		MAX,
	};

	enum class MUSCLE
	{
		SMALL,
		MEDIUM,
		BIG,
		 
		MAX
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

private:

	AnimationController* animation_;

	STATE state_;
	MUSCLE muscleStat_;

	// 関数ポインタ
	using StateFunc = void(Player::*)();
	std::map<STATE, StateFunc> stateFuncs_;

	void Idle(void);
	void Move(void);

	
};