#pragma once
#include "../AttackBase.h"

class BossShot : public AttackBase
{
public:

	// ステート管理用列挙型定義
	enum class STATE
	{
		WAIT,
		MOVE,

		MAX
	};

	BossShot(const Base& boss, const VECTOR& player, const int& voiceLevel);
	~BossShot() override;

	// 当たり判定したときの処理
	void OnCollision(UnitBase* other) override;

	const bool isEnd(void) { return attack_.isEnd_; }



	void DefaultLoad(void) override;
	void ParamInit(void) override;
	void DefaultUpdate(void) override;
	void DefaultDraw(void) override;
private:
	const int MOVE_SPEED = 10;	// 移動速度
	const float RADIUS = 200.0f;		// 半径

	const unsigned char SHOT_TIME = 60;

	const VECTOR LOCAL_POS = { -60.0f, -250.0f, 150.0f };


#pragma region ショットの状態管理関係
	STATE state_;

	void Wait(void);
	void Move(void);
#pragma endregion 


	const Base& boss_;

	// ターゲットの現在位置（プレイヤー）
	const VECTOR& player_;
};