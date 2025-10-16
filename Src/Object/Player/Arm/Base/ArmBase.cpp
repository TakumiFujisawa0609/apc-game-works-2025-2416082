#include "ArmBase.h"

#include "../../../../Manager/Sound/SoundManager.h"
#include "../../../../Manager/Input/InputManager.h"

#include "../../Player.h"

#include "../../../Boss/Boss.h"
#include "../../../Boss/Hand/BossRightHand.h"


ArmBase::ArmBase(int modelId)
{
	unit_.model_ = modelId;
}

ArmBase::~ArmBase()
{
}

void ArmBase::SubLoad(void)
{
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
}

void ArmBase::SubUpdate(void)
{
	if (cnt_ > 0) {
		cnt_--;
		unit_.isAlive_ = true;
		if (cnt_ <= 0) {
			unit_.isAlive_ = false;
			cnt_ = 0;
		}
	}
}

void ArmBase::SubDraw(void)
{

}

void ArmBase::SubRelease(void)
{
}

void ArmBase::OnCollision(UnitBase* other)
{

	if (!unit_.isAlive_) { return; }
	auto& sound = SoundManager::GetIns();
	if (dynamic_cast<Boss*>(other))
	{
		AddArmScale(BONE_UP);
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


void ArmBase::AddBoneScale(int index, VECTOR scale)
{
	MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, index);

	// 行列からスケール成分を抽出
	float currentScale[3];
	for (int i = 0; i < 3; i++) {
		currentScale[i] = VSize(VGet(mat.m[i][0], mat.m[i][1], mat.m[i][2]));
	}

	// スケール加算
	VECTOR newScale = VAdd(scale, { currentScale[0], currentScale[1], currentScale[2] });

	// 最大値の制限
	if (newScale.x > MAX_ARM_MUSCLE.x) newScale.x = MAX_ARM_MUSCLE.x;
	if (newScale.y > MAX_ARM_MUSCLE.y) newScale.y = MAX_ARM_MUSCLE.y;
	if (newScale.z > MAX_ARM_MUSCLE.z) newScale.z = MAX_ARM_MUSCLE.z;

	// 最低値の制限
	if (newScale.x < MIN_ARM_MUSCLE.x) newScale.x = MIN_ARM_MUSCLE.x;
	if (newScale.y < MIN_ARM_MUSCLE.y) newScale.y = MIN_ARM_MUSCLE.y;
	if (newScale.z < MIN_ARM_MUSCLE.z) newScale.z = MIN_ARM_MUSCLE.z;

#ifdef _DEBUG
	// 筋肉量を確認する用の処理(デバッグ用)
	float avgScale = (newScale.x + newScale.y + newScale.z) / 3.0f;
	float avgMin = (MIN_ARM_MUSCLE.x + MIN_ARM_MUSCLE.y + MIN_ARM_MUSCLE.z) / 3.0f;
	float avgMax = (MAX_ARM_MUSCLE.x + MAX_ARM_MUSCLE.y + MAX_ARM_MUSCLE.z) / 3.0f;

	float muscleRatio_ = (avgScale - avgMin) / (avgMax - avgMin);
#endif // _DEBUG

	// スケール行列を作成
	MATRIX scaleMat = MGetScale(newScale);

	// 適用
	MV1SetFrameUserLocalMatrix(unit_.model_, index, scaleMat);
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

