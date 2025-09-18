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
	//Animation animation;
	//animation.animIndex = animIndex;
	//animation.speed = speed;

	//if (animations_.count(type) == 0)
	//{
	//	// 動的配列に追加
	//	animations_.emplace(type, animation);
	//}

	Animation animation;
	animation.model = -1;
	animation.animIndex = animIndex;

	Add(type, speed, animation);
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

	if (playAnim_.model == -1) {
		// モデルと同じファイルからアニメーションをアタッチする
		playAnim_.attachNo = MV1AttachAnim(modelId_, playAnim_.animIndex);
	}
	else {
		int animIndex = 0;
		playAnim_.attachNo = MV1AttachAnim(modelId_, animIndex, playAnim_.model);
	}

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
	// 外部FBXのモデル（アニメーション）解放
	for (const std::pair<int, Animation>& pair : animations_)
	{
		if (pair.second.model != -1)
		{
			MV1DeleteModel(pair.second.model);
		}
	}

	// 可変長配列をクリアする
	animations_.clear();
}

bool AnimationController::IsEnd(int type) const
{
	if (playType_ != type) return false; // 再生中でない
	return playAnim_.step >= playAnim_.totalTime;
}

void AnimationController::Add(int type, float speed, const std::string path)
{
	Animation animation;
	animation.model = MV1LoadModel(path.c_str());
	animation.animIndex = -1;

	Add(type, speed, animation);
}

void AnimationController::Add(int type, float speed, Animation& animation)
{
	if (animations_.count(type) == 0)
	{
		animation.speed = speed;
		animations_.emplace(type, animation);
	}
}
