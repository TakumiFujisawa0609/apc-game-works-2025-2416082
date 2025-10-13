#pragma once
#include "../../UnitBase.h"

class BossRightHand : public UnitBase
{
public:
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };
	static constexpr VECTOR LOCAL_POS = { 400.0f, 100.0f, -100.0f };

	BossRightHand();
	~BossRightHand() override;

	void Load(void) override;
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void OnCollision(UnitBase* other) override;

	void SetBaseMat(MATRIX mat) { baseMat_ = mat; }
private:
	MATRIX baseMat_;
};