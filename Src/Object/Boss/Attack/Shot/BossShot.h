#pragma once
#include "../../../UnitBase.h"

class BossShot : public UnitBase
{
public:



	// ステート管理用列挙型定義
	enum class STATE
	{
		WAIT,
		MOVE,

		MAX
	};

	BossShot(int modelId_, const Base& boss, const VECTOR& targetPos);
	~BossShot() override;

	// 当たり判定したときの処理
	void OnCollision(UnitBase* other) override;

	bool End(void) { return isEnd_; }

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:
	const int MOVE_SPEED = 10;	// 移動速度
	const int RADIUS = 200;		// 半径

	const unsigned char SHOT_TIME = 60;

	const VECTOR LOCAL_POS = { -60.0f, -250.0f, 150.0f };


	// ボスショットのステート管理用enum変数
	STATE state_;

	//ボスショットのステート管理用関数-----
	void Wait(void);
	void Move(void);
	// ------------------------------------

	// 攻撃が終了したかどうか
	bool isEnd_;

	const Base& boss_;

	// ターゲットの現在位置（プレイヤー）
	const VECTOR& targetPos_;

	unsigned char attackCounter_;
};