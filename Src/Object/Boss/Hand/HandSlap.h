#pragma once
#include "../../UnitBase.h"

class HandSlap : public UnitBase
{
public:
	static constexpr VECTOR SCALE = { 2.0f,2.0f,2.0f };
	static constexpr VECTOR LOCAL_POS = { 400.0f, 100.0f, -100.0f };

	static constexpr float FALL_SPEED = 20.0f;

	static constexpr VECTOR COLL_SIZE = { 800.0f,500.0f,500.0f };
	static constexpr float OFFSET_Y = 500.0f;
	static constexpr int COUNT_DOWN = 20.0f;

	HandSlap(const VECTOR& target);
	~HandSlap() override;

	void OnCollision(UnitBase* other) override;

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

private:
	bool end_;

	bool isSlap_;

	int cnt_;

	const VECTOR& target_;
};