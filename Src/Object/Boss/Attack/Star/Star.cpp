#include "Star.h"

#include "../../../Player/Player.h"

#include "../../Boss.h"

#include "../../../../Scene/Game/GameScene.h"


Star::Star(const VECTOR& boss, const VECTOR& player, const int& voiceLevel) :
	AttackBase(boss, player, voiceLevel)
{
}

Star::~Star()
{
}

void Star::DefaultLoad(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Boss/Star.mv1");

	StateAdd((int)STATE::MOVE, [this](void) { Move(); });
	StateAdd((int)STATE::ROTATE, [this](void) { Rotate(); });
}

void Star::ParamInit(void)
{
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.colliType = CollisionType::ENEMY;

	unit_.isAlive_ = true;
	unit_.angle_ = Utility::VECTOR_ZERO;

	unit_.pos_ = Utility::VECTOR_ZERO;
	unit_.pos_.y = -500;
	unit_.para_.radius = 200;

	attack_.isEnd_ = false;
	attack_.isParried_ = false;
	attack_.attackCounter_ = 0;
	attack_.parryCounter_ = 10;
	attack_.parryCollRadius_ = 200 + 30;

	state_ = STATE::ROTATE;
}

void Star::DefaultUpdate(void)
{
	unit_.angle_.y += Utility::Deg2RadF(10.0f);
	StateUpdate((int)state_);
}

void Star::DefaultDraw(void)
{
}

void Star::OnCollision(UnitBase* other)
{
	if (dynamic_cast<Player*>(other)) {
		attack_.isEnd_ = true;
		unit_.isAlive_ = false;
	}

	if (!attack_.isParried_) { return; }

	if (dynamic_cast<Boss*> (other)) {
		attack_.isEnd_ = true;  // 攻撃終了
		unit_.isAlive_ = false; // 生存判定
	}
}

void Star::Rotate(void)
{
	attack_.attackCounter_++;
	if (attack_.attackCounter_ < 90)
	{
		float angle = attack_.attackCounter_ * 0.1f;
		float radius = 300.0f;

		unit_.pos_.x = boss_.x + cosf(angle) * radius;
		unit_.pos_.z = boss_.z + sinf(angle) * radius;
		unit_.pos_.y = boss_.y;

	}
	else {
		state_ = STATE::MOVE;
	}

	VECTOR dir = VNorm(VSub(player_, unit_.pos_));

}

void Star::Move(void)
{
	if (attack_.isParried_) { return; }
	if (IsChanceNow() == true) {
		// プレイヤーのボイスが一定以上なら吹っ飛ぶ
		if (voiceLevel_ > 2500) {
			attack_.isParried_ = true;
			GameScene::HitStop(10);
			GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 20);
		}
#ifdef _DEBUG
		// プレイヤーのボイスが一定以上なら吹っ飛ぶ
		if (CheckHitKey(KEY_INPUT_1)) {
			attack_.isParried_ = true;
			GameScene::HitStop(10);
			GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 20);
		}
#endif // _DEBUG
	}

	VECTOR dir = VNorm(VSub(player_, unit_.pos_));
	unit_.pos_ = VAdd(unit_.pos_, VScale(dir, 30.0f));
}
