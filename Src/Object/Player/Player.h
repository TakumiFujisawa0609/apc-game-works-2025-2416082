#pragma once
#include "../UnitBase.h"

class Player : public UnitBase
{
public:

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, 0.0f }; //初期座標
	static constexpr float RADIUS_SIZE = 100.0f;				//プレイヤーの半径（仮）

	static constexpr int MAX_MUSCLE = 100;
	static constexpr int DEFAULT_MUSCLE = 50;

	static constexpr float MOVE_SPEED = 16.0f;

	Player();
	~Player() override;

	void Load(void) override;
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void Muscle(void);

	void OnCollision(UnitBase* other) override;

private:

};