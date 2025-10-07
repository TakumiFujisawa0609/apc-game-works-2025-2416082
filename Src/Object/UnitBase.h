#pragma once

#include <DxLib.h>
#include<vector>

#include"../Utility/Utility.h"

enum class CollisionShape { NON = -1, SPHERE, OBB, CAPSULE, MAX };
enum class CollisionType { NON = -2, ALLY/*味方*/, ENEMY/*敵*/ };

struct Base
{
	struct {
		CollisionShape colliShape = CollisionShape::NON;
		CollisionType colliType = CollisionType::NON;

		VECTOR size = { -1.0f, -1.0f, -1.0f };	  // AABB/ELLIPSOID の直径(各軸の長さ)
		float  radius = -1.0f;                    // SPHERE 半径
		float capsuleHalfLen = -1.0f;
		VECTOR center = { 0.0f, 0.0f, 0.0f };     // 原点からの中心オフセット
		float  speed = -1.0f;
	} para_;

	bool   isAlive_ = false; // 生存フラグ
	bool   aliveCollision_ = true;  // 生存フラグによる当たり判定早期リターン
	VECTOR pos_ = { -1.0f, -1.0f, -1.0f }; // ワールド座標（中心）
	VECTOR angle_ = { 0.0f,0.0f,0.0f };
	VECTOR scale_ = { 1.0f, 1.0f, 1.0f };

	int    hp_ = -1;

	bool   isInvici_ = true; // 無敵
	int    inviciCounter_ = 0;

	int model_ = -1;
};

class UnitBase
{
public:
	UnitBase();
	virtual ~UnitBase() = 0;

	virtual void Load(void) = 0;
	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void Release(void) = 0;

	const Base& GetUnit(void) const { return unit_; }
	virtual void OnCollision(UnitBase* other) = 0;

protected:
	Base unit_;

	// 無敵処理(派生先の更新処理で呼び出す用)
	void Invi(void);
};