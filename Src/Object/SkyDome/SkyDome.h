#pragma once
#include "../UnitBase.h"
class SkyDome : public UnitBase
{
private:

	static constexpr float RADIUS = 100.0f;		// スカイドームの半径
	static constexpr VECTOR POS = { 0.0f, 0.0f, 0.0f };
	static constexpr VECTOR SCALE = { 100.0f,100.f,100.0f };
	static constexpr VECTOR ANGLE = { 0.0f,0.0f,0.0f };
public:

	SkyDome();
	~SkyDome() override;

	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;

	void OnCollision(UnitBase* other) {};

private:

	int tex_;
};