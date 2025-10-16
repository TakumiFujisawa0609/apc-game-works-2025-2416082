#pragma once

#include <DxLib.h>

#include<functional>
#include<unordered_map>

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

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	const Base& GetUnit(void) const { return unit_; }
	virtual void OnCollision(UnitBase* other) = 0;

protected:
	virtual void SubLoad(void) = 0;
	virtual void SubInit(void) = 0;
	virtual void SubUpdate(void) = 0;
	virtual void SubDraw(void) = 0;
	virtual void SubRelease(void) = 0;



	// 関数ポインタ
	using StateFunc = std::function<void()>;
	std::unordered_map<int, StateFunc> stateFuncs_;

	// 子クラスで定義した関数を関数ポインタに登録
	void StateAdd(int state, StateFunc func);

	void ChangeState(int state);

	static constexpr int INVI_TIME = 40;

	Base unit_;

	// 無敵処理(派生先の更新処理で呼び出す用)
	void Invi(void);
};