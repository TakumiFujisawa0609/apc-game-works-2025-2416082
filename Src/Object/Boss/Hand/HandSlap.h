#pragma once
#include "../../UnitBase.h"

class HandSlap : public UnitBase
{
public:
	static constexpr VECTOR SCALE = { 2.0f,2.0f,2.0f };
	static constexpr VECTOR LOCAL_POS = { 400.0f, 100.0f, -100.0f };

	HandSlap(const VECTOR& target);
	~HandSlap() override;

	void OnCollision(UnitBase* other) override;

	void SetBaseMat(MATRIX mat) { baseMat_ = mat; }

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

private:
	MATRIX baseMat_;
	bool end_;

	const VECTOR& target_;
};