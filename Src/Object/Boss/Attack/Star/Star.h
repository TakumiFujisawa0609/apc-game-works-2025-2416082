#pragma once

#include "../AttackBase.h"

class Star : public AttackBase
{
public:
	Star(const VECTOR& boss, const VECTOR& player, const int& voiceLevel);
	~Star() override;

	void OnCollision(UnitBase* other) override;
private:

	enum class STATE
	{
		ROTATE,
		MOVE,

		MAX
	};

	STATE state_;

	void DefaultLoad(void) override;
	void ParamInit(void) override;
	void DefaultUpdate(void) override;
	void DefaultDraw(void) override;

#pragma region ステート管理関係
	void Rotate(void);
	void Move(void);
#pragma endregion
};