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
		STOP,
		END,

		MAX
	};

	static constexpr VECTOR SCALE = { 2.0f,2.0f,2.0f };
	static constexpr VECTOR LOCAL_POS = { 400.0f, 100.0f, -100.0f };

	static constexpr float FALL_SPEED = 20.0f;

	static constexpr VECTOR SIZE = { 800.0f, 500.0f, 100.0f };
	static constexpr float OFFSET_Y = 800.0f;
	static constexpr int COUNT_DOWN = 120.0f;
	static constexpr float GRAVITY = 3.0f;
	
	HandSlap(const VECTOR& boss, const VECTOR& player, const int& voiceLevel);
	~HandSlap() override;

	void DefaultInit(void) override;

	void OnCollision(UnitBase* other) override;

	const bool isEnd(void) { return attack_.end_; }

	const STATE GetState(void) { return state_; }

	void LinesDraw(void);

	// íµÇÀï‘ÇπÇÈÇ©Ç«Ç§Ç©
	bool IsChanceNow(void) override;

private:

	void DefaultUpdate(void) override;

	void DebugDraw(void) override;

#pragma region ÉnÉìÉhÇÃèÛë‘ä«óùópä÷êî
	void Wait(void);
	void Fall(void);
	void Fly(void);
	void End(void);
#pragma endregion 

	STATE state_;

	bool isHit_;

	float fallSpeed_;

};