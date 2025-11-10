#pragma once
#include "../../UnitBase.h"

class HandSlap : public UnitBase
{
public:

	enum class HAND_STATE
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

	static constexpr VECTOR COLL_SIZE = { 800.0f,100.0f, 500.0f };
	static constexpr float OFFSET_Y = 500.0f;
	static constexpr int COUNT_DOWN = 120.0f;
	static constexpr float GRAVITY = 3.0f;

	HandSlap(const VECTOR& target, const int& voiceLevel);
	~HandSlap() override;

	void OnCollision(UnitBase* other) override;

	const bool isEnd(void) { return end_; }
	const bool isHit(void) { return isHit_; }

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:

#pragma region ハンドの状態管理用関数
	void Wait(void);
	void Fall(void);
	void Stop(void);
	void End(void);
#pragma endregion 

	HAND_STATE handState_;

	bool end_;	// 終了判定(true : 終了 / false : 攻撃中)
	bool isHit_;	// プレイヤーに当たったらそれ以降true

	int counter_;
	float fallSpeed_;

	const VECTOR& target_;

	const int& voiceLevel_;
};