#pragma once
#include "../UnitBase.h"

class Boss : public UnitBase
{
public:

	static constexpr int HP_MAX = 100;

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

	int color1;
};