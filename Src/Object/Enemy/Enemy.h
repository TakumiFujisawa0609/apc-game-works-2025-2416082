#pragma once

#include "EnemyBase/EnemyBase.h"

class Enemy : public EnemyBase
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


private:

	void Spawn(void);

};