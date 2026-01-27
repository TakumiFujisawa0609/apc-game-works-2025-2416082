#pragma once
#include <DxLib.h>

#include "../../UnitBase.h"


#include "../../../Utility/Utility.h"

class AttackBase : public UnitBase
{
public:

	enum class PARRY
	{
		YES,
		NO
	};

	AttackBase(const VECTOR& boss, const VECTOR& player, const int& voiceLevel);
	~AttackBase() override;

	void SubLoad() override;
	void SubInit() override;
	void SubUpdate() override;
	void SubDraw() override;
	void SubRelease() override;

	bool IsEnd(void) { return attack_.end_; }



private:

	void ParryUpdate(void);

protected:

	//virtual void DefaultLoad(void) = 0;

	virtual void DefaultInit(void) = 0;

	virtual void DefaultUpdate(void) = 0;
	
	virtual void DebugDraw(void) = 0;

	struct AttackInfo
	{
		// モデルのハンドル
		int modelId_ = -1;

		// 座標・アングル・スケール
		VECTOR pos_ = Utility::VECTOR_ZERO;
		VECTOR angle_ = Utility::VECTOR_ZERO;
		VECTOR scale_ = Utility::VECTOR_ONE;

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