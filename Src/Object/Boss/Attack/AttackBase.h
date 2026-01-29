#pragma once
#include <DxLib.h>

#include "../../UnitBase.h"
#include <map>

#include "../../../Utility/Utility.h"

class AttackBase : public UnitBase
{
protected:
	static constexpr int PARRY_COUNT_TIME = 10;

	// 攻撃のパラメータ
	struct AttackInfo
	{
		// パリィされた後かどうか(true : された後 / false : される前)
		bool isParried_ = false;

		// 攻撃用のカウンタ
		int attackCounter_ = 0;

		// パリィの受付時間用カウンタ
		int parryCounter_ = PARRY_COUNT_TIME;

		int parryCollRadius_ = 0;

		// 攻撃中か否か(true : 攻撃終了 / false : 攻撃中)
		bool isEnd_ = false;
	};

public:


	AttackBase(const VECTOR& boss, const VECTOR& player, const int& voiceLevel);
	~AttackBase() override;

	void SubLoad() override;
	void SubInit() override;
	void SubUpdate() override;
	void SubDraw() override;
	void SubRelease() override;

	// 攻撃が終わったかどうか
	const bool IsEnd(void) { return attack_.isEnd_; }
	
	const AttackInfo GetAttackInfo(void) { return attack_; }

	// 跳ね返せるかどうか
	bool IsChanceNow(void);
	
	bool isChanceNow_;

	void UIDraw(void);

private:

	// パリィしたときの更新処理
	void ParryUpdate(void);

	void SetModelMatrix(void);

protected:

	virtual void DefaultLoad(void) = 0;
	virtual void ParamInit(void) = 0;
	virtual void DefaultUpdate(void) = 0;
	virtual void DefaultDraw(void) = 0;

	// 攻撃の情報
	AttackInfo attack_;

	// プレイヤーとボスの座標
	const VECTOR& boss_;
	const VECTOR& player_;
	const int& voiceLevel_;

};