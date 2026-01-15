#pragma once
#include "../../../UnitBase.h"

class BossShot : public UnitBase
{
public:

	static constexpr int MOVE_SPEED = 10;	// 移動速度
	static constexpr int RADIUS = 200;		// 半径

	// ステート管理用列挙型定義
	enum class STATE
	{
		WAIT,
		MOVE,

		MAX
	};

	BossShot(VECTOR bossPos, const VECTOR& targetPos);
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

	// ボスショットのステート管理用enum変数
	STATE state_;

	//ボスショットのステート管理用関数-----
	void Wait(void);
	void Move(void);
	// ------------------------------------

	// 攻撃が終了したかどうか
	bool isEnd_;

	// 攻撃発射位置（ボスの位置）
	VECTOR bossPos_;

	// ターゲットの現在位置（プレイヤー）
	const VECTOR& targetPos_;
};