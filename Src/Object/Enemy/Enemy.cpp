#include "Enemy.h"
#include <cmath>
#include <cstdlib>

Enemy::Enemy(const VECTOR& target) :
	target_(target)
{

}

Enemy::~Enemy()
{
}

void Enemy::SubLoad(void)
{
	StateAdd((int)STATE::SPAWN, [this]() { Spawn();  });
	StateAdd((int)STATE::MOVE, [this]() { Move();   });
	StateAdd((int)STATE::ATTACK, [this]() { Attack(); });
	StateAdd((int)STATE::DEATH, [this]() { Death();  });
}

void Enemy::SubInit(void)
{
    state_ = STATE::MOVE;

    unit_.para_.colliShape = CollisionShape::CAPSULE;
    unit_.para_.colliType = CollisionType::ENEMY;
    unit_.para_.capsuleHalfLen = 100.0f;
    unit_.para_.radius = 30.0f;

    unit_.isAlive_ = true;
    unit_.hp_ = 100;
    unit_.scale_ = { 1.0f,1.0f,1.0f };
    unit_.angle_ = Utility::VECTOR_ZERO;

	// ランダム初期位置
	float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * DX_PI_F;
	//float x = cosf(theta) * SPAWN_RADIUS;
	//float y = 100.0f;
	//float z = sinf(theta) * SPAWN_RADIUS;

	unit_.pos_ = { 
		cosf(theta) * SPAWN_RADIUS,
		unit_.para_.capsuleHalfLen,
		sinf(theta) * SPAWN_RADIUS
	};
}


void Enemy::SubUpdate(void)
{
	if (!unit_.isAlive_) { return; }

	// プレイヤーとの距離をチェック
	float dist = VSize(VSub(target_, unit_.pos_));

	switch (state_)
	{
	case STATE::MOVE:
		// 近づいたら攻撃に移行
		if (dist < 300.0f) {
			state_ = STATE::ATTACK;
		}
		break;
	case STATE::ATTACK:
		// 離れたら移動に戻す
		if (dist > 500.0f) {
			state_ = STATE::MOVE;
		}
		break;
	default:
		break;
	}
	StateUpdate(static_cast<int>(state_));
}

void Enemy::SubDraw(void)
{
    if (!unit_.isAlive_) { return; }
#ifdef _DEBUG
	// 真ん中の座標から半分の大きさ分、減産と加算
	VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
	VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });

	//当たり判定の範囲を可視化
	DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, 0xfff000, 0xfff000, false);
#endif // _DEBUG
}

void Enemy::SubRelease(void)
{
}

void Enemy::Move(void)
{
	if (!unit_.isAlive_) return;

	// ターゲットへの方向ベクトルを求める
	VECTOR toTarget = VSub(target_, unit_.pos_);
	float dist = VSize(toTarget);

	// 距離が近すぎる場合は移動しない
	if (dist < 10.0f) return;

	// 正規化（向き）
	VECTOR dir = VScale(toTarget, 1.0f / dist);

	// 移動速度
	const float speed = 3.0f;

	// 移動
	unit_.pos_ = VAdd(unit_.pos_, VScale(dir, speed));

	// 向き更新（任意）
	unit_.angle_.y = atan2f(dir.x, dir.z);

	// 移動範囲制限（円形）
	float radius = 2000.0f;  // 行動範囲の半径
	float distance = sqrtf(unit_.pos_.x * unit_.pos_.x + unit_.pos_.z * unit_.pos_.z);

	if (distance > radius)
	{
		// 原点方向に戻す
		float nx = unit_.pos_.x / distance;
		float nz = unit_.pos_.z / distance;
		unit_.pos_.x = nx * radius;
		unit_.pos_.z = nz * radius;
	}
}

void Enemy::Attack(void)
{
}

void Enemy::Death(void)
{
}

void Enemy::UIDraw(void)
{
}

void Enemy::Spawn(void)
{
}
