#pragma once
#include <DxLib.h>

#include "../../UnitBase.h"


#include "../../../Utility/Utility.h"

class AttackBase : public UnitBase
{
public:

	AttackBase(const VECTOR& boss, const VECTOR& player, const int& voiceLevel);
	~AttackBase() override;

	void SubLoad() override;
	void SubInit() override;
	void SubUpdate() override;
	void SubDraw() override;
	void SubRelease() override;

	// 攻撃が終わったかどうか
	bool IsEnd(void) { return attack_.end_; }

	// 跳ね返せるかどうか
	virtual bool IsChanceNow(void) = 0;

private:

	// パリィしたときの更新処理
	void ParryUpdate(void);

protected:

	virtual void DefaultInit(void) = 0;
	virtual void DefaultUpdate(void) = 0;
	virtual void DebugDraw(void) = 0;

	// 攻撃のパラメータ
	struct AttackInfo
	{
		// 攻撃用のカウンタ
		int attackCounter_ = 0;

		// 攻撃中か否か
		bool end_ = false;
	};

	// 攻撃の情報
	AttackInfo attack_;

	// プレイヤーとボスの座標
	const VECTOR& boss_;
	const VECTOR& player_;
	const int& voiceLevel_;

};