#pragma once
#include "../UnitBase.h"

class Stage : public UnitBase
{
public:

	static constexpr VECTOR DEFAULT_POS = { 1000.0f,3200.0f,0.0f };
	static constexpr VECTOR SCALE = { 100.0f,150.0f,100.0f };

	Stage();
	~Stage()override;

	void SubLoad(void) override;		// Å‰‚ÉŒÄ‚Ño‚·ŠÖ”
	void SubInit(void) override;		// ‰Šú‰»ˆ—
	void SubUpdate(void) override;		// XVˆ—
	void SubDraw(void) override;		// •`‰æˆ—
	void SubRelease(void) override;	// ‰ğ•úˆ—

	void OnCollision(UnitBase* other) override;		// “–‚½‚è”»’èˆ—

private:
	int tex;
};