#pragma once
#include "../UnitBase.h"

class EnemyBase : public UnitBase
{
public:
	EnemyBase();
	~EnemyBase()override;

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	virtual void OnCollision(UnitBase* other) override;		// “–‚½‚è”»’èˆ—

	void SetPos(VECTOR pos) { unit_.pos_ = pos; }
protected:

	virtual void SubInit(void)override = 0;		// ‰Šú‰»ˆ—
	virtual void SubUpdate(void)override = 0;		// XVˆ—
	virtual void SubDraw(void)override = 0;		// •`‰æˆ—
	virtual void SubRelease(void)override = 0;	// ‰ğ•úˆ—
	virtual void SubLoad(void)override = 0;		// Å‰‚ÉŒÄ‚Ño‚·ŠÖ”
};