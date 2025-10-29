#pragma once
#include "../../UnitBase.h"

class EnemyBase : public UnitBase
{
public:
	EnemyBase(VECTOR target);
	~EnemyBase()override;

	virtual void SubInit(void) override;		// ‰Šú‰»ˆ—
	virtual void SubUpdate(void) override;		// XVˆ—
	virtual void SubDraw(void) override;		// •`‰æˆ—
	virtual void SubRelease(void) override;	// ‰ğ•úˆ—
	virtual void SubLoad(void) override;		// Å‰‚ÉŒÄ‚Ño‚·ŠÖ”

	virtual void OnCollision(UnitBase* other) override;		// “–‚½‚è”»’èˆ—
private:
	VECTOR target_;
};