#pragma once
#include "../UnitBase.h"

class AnimationController;

class GameOverPlayer : public UnitBase
{
public:

	enum class ANIM_TYPE
	{
		GAME_OVER_PLAYER,
	};

	GameOverPlayer();
	~GameOverPlayer() override;

protected:

	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

	void OnCollision(UnitBase* other) override {}

private:

	AnimationController* animation_;

};