#pragma once
#include "../AttackBase.h"

#include "../../../Player/Player.h"

class HandSlap : public AttackBase
{
public:

	enum class STATE
	{
		WAIT,
		FALL,
		END,

		MAX
	};

	HandSlap(const VECTOR& boss, const VECTOR& player, const int& voiceLevel);
	~HandSlap() override;

	void OnCollision(UnitBase* other) override;

	const STATE GetState(void) { return state_; }

private:

	static constexpr VECTOR SCALE = { 2.0f,2.0f,2.0f };
	static constexpr VECTOR LOCAL_POS = { 400.0f, 100.0f, -100.0f };

	static constexpr float FALL_SPEED = 20.0f;

	static constexpr VECTOR SIZE = { 800.0f, 500.0f, 100.0f };
	static constexpr float OFFSET_Y = 800.0f;
	static constexpr int COUNT_DOWN = 120.0f;
	static constexpr float GRAVITY = 3.0f;

private:

	void DefaultLoad(void) override;
	void ParamInit(void) override;
	void DefaultUpdate(void) override;
	void DefaultDraw(void) override;

#pragma region ÉnÉìÉhÇÃèÛë‘ä«óùä÷åW
	void Wait(void);
	void Fall(void);
	void End(void);
#pragma endregion 

	STATE state_;

	bool isHit_;

	float fallSpeed_;

};