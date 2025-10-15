#include "Boss.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Scene/SceneManager/SceneManager.h"
#include "../../Scene/Game/GameScene.h"

#include "../../Manager/Sound/SoundManager.h"

#include "Hand/BossRightHand.h"

#include "../Player/Arm/LeftArm.h"
#include "../Player/Arm/RightArm.h"

Boss::Boss()
{
}

Boss::~Boss()
{
}

void Boss::Load(void)
{
	rHand_ = new BossRightHand();
	rHand_->Load();

	unit_.model_ = MV1LoadModel("Data/Model/Boss/BossHead.mv1");

	SoundManager::GetIns().Load(SOUND::HIT);
}

void Boss::Init(void)
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

	rHand_->Init();

}

void Boss::Update(void)
{
	auto& scene = SceneManager::GetInstance();
	if (unit_.hp_ <= 0) {
		unit_.hp_ = 0;
		unit_.isAlive_ = false;
	}

	// target_ の方向に向く
	VECTOR dir = VSub(target_, unit_.pos_);
	float targetAngleY = atan2f(dir.x, dir.z);

	float rotationSpeed = Utility::Deg2RadF(1.0f);
	float deltaAngle = targetAngleY - unit_.angle_.y;
	while (deltaAngle > 3.14159f) deltaAngle -= 2 * 3.14159f;
	while (deltaAngle < -3.14159f) deltaAngle += 2 * 3.14159f;

	if (fabsf(deltaAngle) < rotationSpeed)
		unit_.angle_.y = targetAngleY;
	else
		unit_.angle_.y += (deltaAngle > 0 ? rotationSpeed : -rotationSpeed);

	rHand_->Update();
	Invi();

	// ダメージテキストの更新
	for (auto it = damageTexts_.begin(); it != damageTexts_.end(); ) {
		it->pos_.y += 1.0f;  // 上に浮かせる
		it->drawTime_--;
		if (it->drawTime_ <= 0) {
			it = damageTexts_.erase(it);
		}
		else {
			++it;
		}
	}
}

void Boss::Draw(void)
{
	if (!unit_.isAlive_)return;

	VECTOR offset = { 0.0f, -150.0f, 0.0f };

	// 行列の作成
	MATRIX mat = MGetIdent();
	
	// 行列に向きの適用
	mat = MMult(mat, MGetRotX(unit_.angle_.x));
	mat = MMult(mat, MGetRotY(unit_.angle_.y));
	mat = MMult(mat, MGetRotZ(unit_.angle_.z));

	// モデルの向きを修正
	MATRIX localMat = MGetIdent();
	localMat = MMult(localMat, MGetRotX(LOCAL_ANGLE.x));
	localMat = MMult(localMat, MGetRotY(LOCAL_ANGLE.y));
	localMat = MMult(localMat, MGetRotZ(LOCAL_ANGLE.z));

	// 合体
	mat = MMult(localMat, mat);

	// スケール情報を合成
	mat = MMult(MGetScale(unit_.scale_), mat);

	// ワールド座標をゲット
	VECTOR worldPos = VTransform(offset, mat);

	// 座標の情報を行列に渡す
	mat.m[3][0] = worldPos.x + unit_.pos_.x;
	mat.m[3][1] = worldPos.y + unit_.pos_.y;
	mat.m[3][2] = worldPos.z + unit_.pos_.z;

	// モデルに行列の適用
	// モデルの描画
	MV1SetMatrix(unit_.model_, mat);
	MV1DrawModel(unit_.model_);

	//ボスの右手の描画
	rHand_->SetBaseMat(mat);
	rHand_->Draw();


}

void Boss::Release(void)
{
	//モデルの解放
	MV1DeleteModel(unit_.model_);

	//右手の開放
	if (rHand_)
	{
		rHand_->Release();
		delete rHand_;
		rHand_ = nullptr;
	}

	// 音声の開放
	for (int i = 0; i < (int)SOUND::MAX; i++) {
		SoundManager::GetIns().Delete((SOUND)i);
	}
}

void Boss::UIDraw(void)
{
#ifdef _DEBUG

	// 真ん中の座標から半分の大きさ分、減産と加算
	VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
	VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });

	//当たり判定の範囲を可視化
	DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, color1, color1, false);

	// HPのデバッグ表示
	for (int i = 0; i < unit_.hp_; i++) {
		DrawBox(50 + (i * 5), Application::SCREEN_SIZE_Y - 100, 60 + (i * 5), Application::SCREEN_SIZE_Y - 100 + 50, 0xff0000, true);
	}
	DrawSphere3D(unit_.pos_, 20, 16, 0xff00ff, 0xff00ff, true);

	// ダメージテキスト描画
	for (auto& dt : damageTexts_) {
		DrawFormatString((int)dt.pos_.x + 64 + Application::SCREEN_SIZE_X / 2, (int)dt.pos_.y + 64 - 16, 0xffff00, "damage");
		SetFontSize(64);
		DrawFormatString((int)dt.pos_.x + Application::SCREEN_SIZE_X / 2, (int)dt.pos_.y, 0xffff00, "%d", dt.damage_);
		SetFontSize(16);
	}

#endif // _DEBUG
}

void Boss::OnCollision(UnitBase* other)
{
	if (unit_.inviciCounter_ > 0) return;

	int damage = 0;
	if (dynamic_cast<LeftArm*>(other) || dynamic_cast<RightArm*>(other)) {
		SoundManager::GetIns().Play(SOUND::HIT, true);

		if (playerMuscleRatio_ > 0.0f) {
			if (playerMuscleRatio_ > 0.6f) damage = 25;
			else if (playerMuscleRatio_ > 0.4f) damage = 15;
			else damage = 10;

			unit_.hp_ -= damage;
			unit_.inviciCounter_ = INVI_TIME;
			GameScene::Shake(ShakeKinds::DIAG, ShakeSize::MEDIUM, 15);
			GameScene::HitStop(5);

			// ダメージテキスト生成
			DamageText dt;
			dt.pos_ = unit_.pos_;      // ボスの頭上に表示
			dt.pos_.y += 200.0f;
			dt.damage_ = damage;
			dt.drawTime_ = 60;             // 1秒表示（60フレーム想定）
			damageTexts_.push_back(dt);
		}
		return;
	}
}
