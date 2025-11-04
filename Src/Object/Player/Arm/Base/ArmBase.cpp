#include "ArmBase.h"
#include <random>

#include "../../../../Manager/Sound/SoundManager.h"
#include "../../../../Manager/Input/InputManager.h"
#include "../../../../Manager/FlashEffectManager/FlashEffectManager.h"

#include "../../Player.h"

#include "../../../Boss/Boss.h"
#include "../../../Boss/Hand/HandSlap.h"


ArmBase::ArmBase(int modelId)
{
	unit_.model_ = modelId;
}

ArmBase::~ArmBase()
{
}

void ArmBase::SubLoad(void)
{
	flash_ = new FlashEffectManager();
}

void ArmBase::SubInit(void)
{
	// プレイヤーモデルのボーンの名前を
// 羅列させるためのデバッグ用変数
	frameScrollIndex_ = 0;

	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.colliType = CollisionType::ALLY;

	unit_.para_.radius = RADIUS;

	unit_.isAlive_ = false;

	cnt_ = 0;

	state_ = 0;

	isHit_ = false;
}

void ArmBase::SubUpdate(void)
{
	if (cnt_ > 0) {
		cnt_--;
	}
	if (cnt_ <= 0) {
		cnt_ = 0;
	}

	unit_.isAlive_ = (cnt_ > 0);


	if (CheckHitKey(KEY_INPUT_0)) {
		AddArmScale({ -0.05, -0.05, -0.05 });
	}
	if (CheckHitKey(KEY_INPUT_9)) {
		AddBoneScale(4,{ 0.05, 0.05, 0.05 });
	}

	auto randf = [](float min, float max)
		{
			static std::mt19937 mt(std::random_device{}());
			std::uniform_real_distribution<float> dist(min, max);
			return dist(mt);
		};

	for (int i = 0; i < 10; i++)
	{
		VECTOR start = VAdd(unit_.pos_, VGet(randf(-30, 30), randf(-30, 30), randf(-30, 30)));
		VECTOR end = VAdd(start, VGet(randf(-30, 30), randf(-30, 30), randf(-30, 30)));
		flash_->Add(start, end);
	}

	flash_->Update();
}

void ArmBase::SubDraw(void)
{
	flash_->Draw();
}

void ArmBase::SubRelease(void)
{
}

void ArmBase::OnCollision(UnitBase* other)
{
	auto& sound = SoundManager::GetIns();


	if (dynamic_cast<Boss*>(other))
	{
		//AddArmScale(BONE_UP);
		isHit_ = true;
		return;
	}

	//if (dynamic_cast<BossRightHand*>(other))
	//{
	//	if (state_ == (int*)Player::STATE::ROLL) {
	//		ArmBase::AddArmScale(BONE_UP);
	//		return;
	//	}
	//	ArmBase::AddArmScale({ -0.3f, -0.3f, -0.3f });
	//}
}



void ArmBase::UIDraw(void)
{
	auto& input = InputManager::GetInstance();
	int frameNum = MV1GetFrameNum(unit_.model_);

	if (input.IsTrgDown(KEY_INPUT_UP))
	{
		frameScrollIndex_--;
		if (frameScrollIndex_ < 0) frameScrollIndex_ = 0;
	}

	if (input.IsTrgDown(KEY_INPUT_DOWN))
	{
		frameScrollIndex_++;
		if (frameScrollIndex_ > frameNum - 1) frameScrollIndex_ = frameNum - 1;
	}
	// ===== 画面に描画 =====
	int y = 200;
	const int maxLines = 20; // 一度に表示する行数

	for (int i = 0; i < maxLines; i++)
	{
		int idx = frameScrollIndex_ + i;
		if (idx >= frameNum) break;

		const char* name = MV1GetFrameName(unit_.model_, idx);
		DrawFormatString(0, y, GetColor(255, 255, 255),
			"Frame %d : %s", idx, name ? name : "(null)");
		y += 16;
	}
}

void ArmBase::SetAttackTime(int collTime)
{
	cnt_ = collTime;
}



void ArmBase::AddArmScale(VECTOR scale)
{
	AddBoneScale(LEFT_ARM_INDEX, scale);
	AddBoneScale(RIGHT_ARM_INDEX, scale);

	//// 行列からスケール成分を抽出
	//float currentScale[3];
	//for (int i = 0; i < 3; i++) {
	//    currentScale[i] = VSize(VGet(mat.m[i][0], mat.m[i][1], mat.m[i][2]));
	//}

	//// スケール加算
	//VECTOR newScale = VAdd(scale, { currentScale[0], currentScale[1], currentScale[2] });
}