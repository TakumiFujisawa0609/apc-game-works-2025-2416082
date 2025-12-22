#include "Boss.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Scene/SceneManager/SceneManager.h"
#include "../../Scene/Game/GameScene.h"

#include "../../Manager/Sound/SoundManager.h"

#include "Attack/Hand/HandSlap.h"
#include "Attack/Hand/RotateHand.h"

#include "../Player/Arm/LeftArm.h"
#include "../Player/Arm/RightArm.h"

Boss::Boss(const VECTOR& target) :
	target_(target),
	playerMuscleRatio_()
{
}

Boss::~Boss()
{
}

void Boss::SubLoad(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Boss/BossHead.mv1");

	SoundManager::GetIns().Load(SOUND::HIT);
	SoundManager::GetIns().Load(SOUND::GOGOGO);

	AttackLoad();
}

void Boss::SubInit(void)
{
	unit_.para_.colliShape = CollisionShape::CAPSULE;

	unit_.para_.radius = RADIUS / 3 * 4;
	unit_.para_.capsuleHalfLen = HALF_LEN * 2;
	unit_.pos_ = DEFAULT_POS;

	unit_.hp_ = HP_MAX;
	unit_.scale_ = SCALE;
	unit_.angle_ = LOCAL_ANGLE;

	unit_.isAlive_ = true;

	color1 = 0xfff000;

	isAttackInit_ = false;
	isAttackEnd_ = false;

	state_ = STATE::ATTACK;
	attackState_ = ATTACK::NON;

	AttackInit();

	StateAdd((int)STATE::ATTACK, [this](void) {Attack(); });
	StateAdd((int)STATE::DEATH, [this](void)  {Death();  });
}

void Boss::SubUpdate(void)
{
	auto& scene = SceneManager::GetInstance();

	ToDeath();		// ボスの死亡処理の遷移

	StateUpdate(static_cast<int>(state_));

	if (state_ == STATE::DEATH) {
		SoundManager::GetIns().Play(SOUND::GOGOGO, false, 150, true, true);
		return;
	}

	LookTarget();	// プレイヤーを見る処理

	Invi();

#ifdef _DEBUG
	//if (CheckHitKey(KEY_INPUT_UP)) { unit_.pos_.z += 5; }
	//if (CheckHitKey(KEY_INPUT_DOWN)) { unit_.pos_.z -= 5; }
	//if (CheckHitKey(KEY_INPUT_RIGHT)) { unit_.pos_.x += 5; }
	//if (CheckHitKey(KEY_INPUT_LEFT)) { unit_.pos_.x -= 5; }

	if (CheckHitKey(KEY_INPUT_P)) { state_ = STATE::DEATH; }
#endif // _DEBUG

}

void Boss::SubDraw(void)
{
	if (!unit_.isAlive_)return;

	SetMatrix();

	AttackDraw();

	MV1DrawModel(unit_.model_);

	slap_->MarkerDraw();

	UIDraw();
}

void Boss::SubRelease(void)
{
	//モデルの解放
	MV1DeleteModel(unit_.model_);
	MV1DeleteModel(handModel_);

	AttackRelease();

	// 音声の開放
	SoundManager::GetIns().AllStop();
	SoundManager::GetIns().Delete(SOUND::HIT);
	SoundManager::GetIns().Delete(SOUND::GOGOGO);
}

void Boss::SetMatrix(void)
{

	VECTOR offset = { 0.0f, -150.0f, 0.0f };

	// 行列の作成
	MATRIX mat = MGetIdent();

	// 行列に向きの適用
	Utility::MatrixRotMult(mat, unit_.angle_);

	// モデルの向きを修正
	MATRIX localMat = MGetIdent();
	Utility::MatrixRotMult(localMat, LOCAL_ANGLE);

	// 合体
	mat = MMult(localMat, mat);

	// スケール情報を合成
	mat = MMult(MGetScale(unit_.scale_), mat);

	// ワールド座標をゲット
	VECTOR worldPos = VTransform(offset, mat);

	VECTOR pos = VAdd(worldPos, unit_.pos_);
	Utility::MatrixPosMult(mat, pos);

	// モデルに行列の適用
	// モデルの描画
	MV1SetMatrix(unit_.model_, mat);
}

void Boss::ToDeath(void)
{
	if (unit_.hp_ <= 0) {
		unit_.hp_ = 0;
		state_ = STATE::DEATH;
	}
}

void Boss::LookTarget(void)
{
	//target_ の方向に向く
	VECTOR dir = VSub(target_, unit_.pos_);
	float targetAngleY = atan2f(dir.x, dir.z);

	float rotationSpeed = Utility::Deg2RadF(1.0f);
	float deltaAngle = targetAngleY - unit_.angle_.y;
	while (deltaAngle > DX_PI_F) deltaAngle -= 2 * 3.14159f;
	while (deltaAngle < -DX_PI_F) deltaAngle += 2 * 3.14159f;

	if (fabsf(deltaAngle) < rotationSpeed) {
		unit_.angle_.y = targetAngleY;
	}
	else {
		unit_.angle_.y += (deltaAngle > 0 ? rotationSpeed : -rotationSpeed);
	}
}

#pragma region ステート処理
void Boss::Attack(void)
{
	switch (attackState_)
	{
	case ATTACK::NON:
		attackCounter_++;
		if (attackCounter_ > NEXT_ATTACK_TIME)
		{
			attackCounter_ = 0;
			// 次の攻撃を抽選
			attackState_ = AttackLottery();
			AttackInit();
		}
		break;

	case ATTACK::SLAP:
		slap_->Update();
		if (slap_->isEnd()) {
			attackState_ = ATTACK::NON; // 終了したら戻る
		}
		break;

	case ATTACK::ROTA_HAND:
		rotaHnad_->Update();
		if (rotaHnad_->IsEnd()) {
			attackState_ = ATTACK::NON;
		}
		break;

	default:
		break;
	}
}

void Boss::Idle(void)
{
}

void Boss::Damage(void)
{
}

void Boss::Death(void)
{
	unit_.pos_.y--;
	GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG, 100);
	if (unit_.pos_.y < (DEFAULT_POS.y - 180)) {
		unit_.isAlive_ = false;
	}
}
#pragma endregion 

#pragma region 攻撃関係

Boss::ATTACK Boss::AttackLottery(void)
{
	return ATTACK::SLAP; /* (ATTACK)GetRand((int)ATTACK::BALL - 1);*/
}

void Boss::AttackLoad(void)
{
	handModel_ = MV1LoadModel("Data/Model/Boss/hand.mv1");
	Utility::ClassNew(slap_, handModel_, target_, voiceLevel_)->Load();
	Utility::ClassNew(rotaHnad_, unit_.pos_);
}

void Boss::AttackInit(void)
{
	switch (attackState_)
	{
	case Boss::ATTACK::NON:
		break;
	case Boss::ATTACK::SLAP:
		slap_->Init();
		break;
	case Boss::ATTACK::ROTA_HAND:
		rotaHnad_->Init();
		break;
	case Boss::ATTACK::BALL:
		break;
	case Boss::ATTACK::MAX:
		break;
	default:
		break;
	}
}

void Boss::AttackDraw(void)
{
	slap_->Draw();
	rotaHnad_->Draw();
}

void Boss::AttackRelease(void)
{
	//右手の解放
	Utility::SafeDeleteInstance(slap_);

	// 回転手解放
	Utility::SafeDeleteInstance(rotaHnad_);

	//if (slap_)
	//{
	//	slap_->Release();
	//	delete slap_;
	//	slap_ = nullptr;
	//}

	//if (rotaHnad_) {
	//	rotaHnad_->Release();
	//	delete rotaHnad_;
	//	rotaHnad_ = nullptr;
	//}

}
#pragma endregion 

void Boss::UIDraw(void)
{

	DrawBar(
		(Application::SCREEN_SIZE_X / 10) * 2,
		(Application::SCREEN_SIZE_Y / 10) * 9,
		(Application::SCREEN_SIZE_X / 10) * 8,
		(Application::SCREEN_SIZE_Y / 10) * 8 + 120,
		unit_.hp_, HP_MAX,
		RGB(50, 50, 255),
		RGB(0, 0, 0));

#ifdef _DEBUG

	// 真ん中の座標から半分の大きさ分、減産と加算
	VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
	VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });

	//当たり判定の範囲を可視化
	DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, color1, color1, false);

	DrawFormatString(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 0xffffff, "BossHP(%i)", unit_.hp_);



#endif // _DEBUG
}

void Boss::OnCollision(UnitBase* other)
{
	if (unit_.inviciCounter_ > 0) return;

	int damage = 0;
	if (dynamic_cast<LeftArm*>(other) || dynamic_cast<RightArm*>(other)) {
		SoundManager::GetIns().Play(SOUND::HIT, true);
		unit_.inviciCounter_ = INVI_TIME;
		if (playerMuscleRatio_ >= 0.0f) {
			if (playerMuscleRatio_ > 0.6f) damage = 25;
			else if (playerMuscleRatio_ > 0.4f) damage = 15;
			else damage = 10;

			unit_.hp_ -= damage;
			GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 15);
			GameScene::HitStop(5);
		}
		return;
	}
}
