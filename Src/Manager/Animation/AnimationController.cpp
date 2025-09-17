#include <DxLib.h>

#include "../../Scene/SceneManager/SceneManager.h"

#include "AnimationController.h"

AnimationController::AnimationController(int modelId)
{
	modelId_ = modelId;
	playType_ = -1;
}

AnimationController::~AnimationController(void)
{
}

void AnimationController::AddInFbx(int type, float speed, int animIndex)
{
	Animation animation;
	animation.animIndex = animIndex;
	animation.speed = speed;
	if (animations_.count(type) == 0)
	{
		// 動的配列に追加
		animations_.emplace(type, animation);
	}
}

void AnimationController::Play(int type, bool isLoop)
{
	isLoop_ = isLoop;

	//アニメーションが終わったか確認
	if (playAnim_.step >= playAnim_.totalTime)
	{
		if (isLoop_)
		{
			playAnim_.step = 0.0f;  //最初からループ
		}
		else
		{
			playAnim_.step = playAnim_.totalTime; //終了フレームで止める
			return;
		}
	}

	if (playType_ == type)
	{
		// 同じアニメーションだったら再生を継続する
		return;
	}

	if (playType_ != -1)
	{
		// モデルからアニメーションを外す
		MV1DetachAnim(modelId_, playAnim_.attachNo);
	}
	// アニメーション種別を変更
	playType_ = type;
	playAnim_ = animations_[type];
	// 初期化
	playAnim_.step = 0.0f;

	// モデルと同じファイルからアニメーションをアタッチする
	playAnim_.attachNo = MV1AttachAnim(modelId_, playType_);

	//アニメーション総時間の取得
	playAnim_.totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnim_.attachNo);
}

void AnimationController::Update(void)
{
	//経過時間の取得
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	//再生
	playAnim_.step += (deltaTime * playAnim_.speed);



	//アニメーションの再生位置をモデルに適用
	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);
}

void AnimationController::Release(void)
{

}

bool AnimationController::IsEnd(int type) const
{
	if (playType_ != type) return false; // 再生中でない
	return playAnim_.step >= playAnim_.totalTime;
}