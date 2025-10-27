#pragma once

#include "../UnitBase.h"

class Enemy : public UnitBase
{
public:

	Enemy();
	~Enemy()override;

	void SubLoad(void) override;		// Å‰‚ÉŒÄ‚Ño‚·ŠÖ”
	void SubInit(void) override;		// ‰Šú‰»ˆ—
	void SubUpdate(void) override;		// XVˆ—
	void SubDraw(void) override;		// •`‰æˆ—
	void SubRelease(void) override;	// ‰ğ•úˆ—

	void UIDraw(void);				// UI•`‰æ

	void OnCollision(UnitBase* other) override;		// “–‚½‚è”»’èˆ—

private:

	void Spawn(void);

};