#pragma once

#include"../ActorBase.h"

class SkyDome : public ActorBase
{
public:
	SkyDome() {}
	~SkyDome() {}

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void AlphaDraw(void)override {}

	void SetPos(const Vector3& pos) { trans_.pos = pos; }
private:
	// ï\é¶Ç∑ÇÈëÂÇ´Ç≥
	const float SCALE = 30.0f;

	// ç¿ïW
	const Vector3 POS = { 0.0f,0.0f,0.0f };
};