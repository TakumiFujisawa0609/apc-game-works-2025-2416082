#pragma once
#include "../UnitBase.h"

class BossRightHand;

class Boss : public UnitBase
{
public:
	static constexpr float RADIUS = 200.0f;
	static constexpr VECTOR SCALE = { 2.0f,2.0f,2.0f };
	static constexpr float HALF_LEN = 300.0f;

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 300.0f, 200.0f };

	static constexpr int HP_MAX = 300;

	enum  ATTACK
	{
		NON,



		MAX
	};

	Boss();
	~Boss() override;

	void Load(void) override;
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void OnCollision(UnitBase* other) override;


private:
	BossRightHand* rHand_;

	int color1;
};