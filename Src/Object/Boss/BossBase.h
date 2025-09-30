#pragma once
#include "../UnitBase.h"

class BossBase : public UnitBase
{
public:

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, 0.0f }; //èâä˙ç¿ïW

	BossBase();
	~BossBase()override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other);

private:
	VECTOR MakeBoxByCenter(VECTOR center, float sizeX, float sizeY, float sizeZ);


};