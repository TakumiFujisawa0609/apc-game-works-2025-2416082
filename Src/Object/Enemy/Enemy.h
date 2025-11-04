#pragma once

#include "EnemyBase.h"

class Enemy : public EnemyBase
{
public:

	static constexpr int SPAWN_RADIUS = 2000;

	// プレイヤーのステート管理
	enum class STATE
	{
		SPAWN,
		MOVE,
		ATTACK,
		DEATH,

		MAX
	};

	Enemy(const VECTOR& target);
	~Enemy()override;


	void UIDraw(void);				// UI描画]

protected:
	void SubLoad(void) override;		// 最初に呼び出す関数
	void SubInit(void) override;		// 初期化処理
	void SubUpdate(void) override;		// 更新処理
	void SubDraw(void) override;		// 描画処理
	void SubRelease(void) override;	// 解放処理
private:

	STATE state_;

#pragma region ステート関数
	void Spawn(void);
	void Move(void);
	void Attack(void);
	void Death(void);
#pragma endregion 

	const VECTOR& target_;
};