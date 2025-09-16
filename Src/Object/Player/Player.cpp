#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Load(void)
{
}

void Player::Init(void)
{
	unit_.isAlive_ = true;
	unit_.pos_ = DEFAULT_POS;
}

void Player::Update(void)
{
    VECTOR move = VGet(0.0f, 0.0f, 0.0f);

    // 入力で方向ベクトルを作る
    if (CheckHitKey(KEY_INPUT_LEFT))  move.x -= 1.0f;
    if (CheckHitKey(KEY_INPUT_RIGHT)) move.x += 1.0f;
    if (CheckHitKey(KEY_INPUT_UP))    move.z += 1.0f;
    if (CheckHitKey(KEY_INPUT_DOWN))  move.z -= 1.0f;

    // 移動量があるときだけ正規化して移動
    if (move.x != 0.0f || move.z != 0.0f)
    {
        // 斜め移動の補正（正規化）
        float len = sqrtf(move.x * move.x + move.z * move.z);
        move.x = (move.x / len) * MOVE_SPEED;
        move.z = (move.z / len) * MOVE_SPEED;

        // 位置を更新
        unit_.pos_.x += move.x;
        unit_.pos_.z += move.z;
    }

	Invi();
}

void Player::Draw(void)
{
	//if (!unit_.isAlive_)return;

	DrawSphere3D(unit_.pos_, RADIUS_SIZE, 30, 0xffffff, 0xffffff, true);
}

void Player::Release(void)
{
}

void Player::OnCollision(UnitBase* other)
{
}
