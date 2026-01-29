#include "Boss.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Scene/SceneManager/SceneManager.h"
#include "../../Scene/Game/GameScene.h"

#include "../../Manager/Sound/SoundManager.h"

#include "Attack/AttackBase.h"
#include "Attack/Hand/HandSlap.h"
#include "Attack/Shot/BossShot.h"
#include "Attack/Star/Star.h"

#include "../Player/Arm/LeftArm.h"
#include "../Player/Arm/RightArm.h"

Boss::Boss(const VECTOR& target) :
	player_(target),
	playerMuscleRatio_()
{
}

Boss::~Boss()
{
}

// 最初の一回しか呼ばれない処理
void Boss::SubLoad(void)
{
	// ボスのモデルのロード
	unit_.model_ = MV1LoadModel("Data/Model/Boss/BossHead.mv1");

	// 音声のロード
	SoundManager::GetIns().Load(SOUND::HIT);
	SoundManager::GetIns().Load(SOUND::GOGOGO);

	// 攻撃のロード
	AttackLoad();

	// ステートの登録
	StateAdd((int)STATE::IDLE, [this](void) { Idle();  });
	StateAdd((int)STATE::ATTACK, [this](void) { Attack(); });
	StateAdd((int)STATE::DEATH, [this](void) { Death();  });
}

void Boss::SubInit(void)
{
	// ボスの情報の初期化	
	unit_.para_.colliShape = CollisionShape::CAPSULE;

	unit_.para_.radius = RADIUS / 3 * 4;
	unit_.para_.capsuleHalfLen = HALF_LEN * 2;
	unit_.pos_ = DEFAULT_POS;

	unit_.hp_ = HP_MAX;
	unit_.scale_ = SCALE;
	unit_.angle_ = LOCAL_ANGLE;

	unit_.isAlive_ = true;
	
	// 攻撃関係の初期化
	isAttackInit_ = false;
	isAttackEnd_ = false;

	state_ = STATE::IDLE;

    attackState_ = AttackLottery();

	AttackInit();
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
	if (CheckHitKey(KEY_INPUT_P)) { state_ = STATE::DEATH; }
#endif // _DEBUG

}

void Boss::SubDraw(void)
{
	if (!unit_.isAlive_)return;

	SetMatrix();

	AttackDraw();

	MV1DrawModel(unit_.model_);

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

	// モデルの向きを修正
	Utility::MatrixRotMult(mat, unit_.angle_);
	MATRIX localMat = MGetIdent();
	Utility::MatrixRotMult(localMat, LOCAL_ANGLE);

	// 向き修正した行列と合成
	mat = MMult(localMat, mat);

	// スケール情報を合成
	mat = MMult(MGetScale(unit_.scale_), mat);

	// ワールド座標を適用
	VECTOR worldPos = VTransform(offset, mat);
	VECTOR pos = VAdd(worldPos, unit_.pos_);
	Utility::MatrixPosMult(mat, pos);

	// モデルに行列の情報を渡す
	// モデルの描画
	MV1SetMatrix(unit_.model_, mat);
}

void Boss::ToDeath(void)
{
	// ボスのHPが0以下になったらDEATHに遷移
	if (unit_.hp_ <= 0) {
		unit_.hp_ = 0;
		state_ = STATE::DEATH;
	}
}

void Boss::LookTarget(void)
{
	//target_ の方向に向く
	VECTOR dir = VSub(player_, unit_.pos_);
	float targetAngleY = atan2f(dir.x, dir.z);

	float rotationSpeed = Utility::Deg2RadF(1.0f);
	float deltaAngle = targetAngleY - unit_.angle_.y;
	while (deltaAngle > DX_PI_F) deltaAngle -= 2 * DX_PI_F;
	while (deltaAngle < -DX_PI_F) deltaAngle += 2 * DX_PI_F;

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
	int idx = (int)attackState_;

	attacks_[idx]->Update();
	if (attacks_[idx]->IsEnd() == true) {
		state_ = STATE::IDLE;
	}
}

void Boss::Idle(void)
{
	attackCounter_++;
	if (attackCounter_ > NEXT_ATTACK_TIME)
	{
		attackCounter_ = 0;

		// 次の攻撃を抽選
		state_ = STATE::ATTACK;
		attackState_ = AttackLottery();
		AttackInit();
		return;
	}
}

void Boss::Damage(void)
{
}

void Boss::Death(void)
{
	// ボスの死亡演出
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
	if (attackTable_.empty()) return ATTACK::STAR;

	ATTACK nextAttack = attackTable_[attackTableIndex_];

	// 次回のためにインデックスを進める
	attackTableIndex_++;
	if (attackTableIndex_ >= attackTable_.size()) {
		attackTableIndex_ = 0; // ループさせたい場合
	}

	return nextAttack;
}

void Boss::AttackLoad(void)
{
	// 攻撃関連のロード
	attacks_.reserve(10);
	attacks_.emplace_back(new HandSlap(unit_.pos_, player_, voiceLevel_));
	attacks_.emplace_back(new BossShot(unit_, player_, voiceLevel_));
	attacks_.emplace_back(new Star(unit_.pos_, player_, voiceLevel_));

	for (AttackBase*& attack : attacks_)
	{
		attack->Load();
	}
}

void Boss::AttackInit(void)
{
	// 攻撃テーブルの設定（順番に攻撃）
	attackTable_ = {
		ATTACK::SLAP,
		ATTACK::SLAP,
		ATTACK::SHOT,
		ATTACK::STAR,
		ATTACK::SHOT
	};

	attacks_[(int)attackState_]->Init();
}

void Boss::AttackDraw(void)
{
	attacks_[(int)attackState_]->Draw();
}

void Boss::AttackRelease(void)
{
	//右手の解放
	for (AttackBase*& attack : attacks_) {
		Utility::SafeDeleteInstance(attack);
	}
}
#pragma endregion 

void Boss::UIDraw(void)
{
	// ボスのHPバーの描画
	DrawBar(
		(Application::SCREEN_SIZE_X / 10) * 2,
		(Application::SCREEN_SIZE_Y / 10) * 9,
		(Application::SCREEN_SIZE_X / 10) * 8,
		(Application::SCREEN_SIZE_Y / 10) * 8 + 120,
		unit_.hp_, HP_MAX,
		RGB(50, 50, 255),
		RGB(0, 0, 0));

	attacks_[(int)attackState_]->UIDraw();

#ifdef _DEBUG
	VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
	VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });

	int color = 0xfff000;

	//当たり判定の範囲を可視化
	DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, color, color, false);
#endif // _DEBUG
}

void Boss::OnCollision(UnitBase* other)
{
	if (unit_.inviciCounter_ > 0) return;

	// other が AttackBase か確認
	if (auto* attack = dynamic_cast<AttackBase*>(other)) {
		// パリィされていなければボスに当たらない
		if (!attack->GetAttackInfo().isParried_) return;

		// ボスに当たる処理
		SoundManager::GetIns().Play(SOUND::HIT, true);
		unit_.hp_ -= 30;
		GameScene::Shake(ShakeKinds::DIAG, ShakeSize::SMALL, 15);
		GameScene::HitStop(5);
	}
}

const std::vector<AttackBase*> Boss::GetAttackIns(void)
{
	return attacks_;
}

