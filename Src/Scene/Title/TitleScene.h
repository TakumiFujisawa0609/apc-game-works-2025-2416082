#pragma once
#include"../SceneBase.h"
#include<DxLib.h>

class AnimationController;
class MicInput;

class TitleScene : public SceneBase
{
public:
	enum  class ANIM_TYPE
	{
		IDLE,
		ATTACK,

		MAX
	};

	static constexpr VECTOR MAX_SIZE = { 2.0f,2.0f,2.0f };	// 筋肉のスケールの最大値
	static constexpr VECTOR MIN_ARM_MUSCLE = { 1.0f,1.0f,1.0f };	// 筋肉のスケールの最低値

	static constexpr int VOICE_GAUGE_MAX = 10;;

	TitleScene();
	~TitleScene()override;

	// 読み込み
	void Load(void)override;
	// 初期化処理
	void Init(void)override;
	// 更新ステップ
	void Update(void)override;
	// 描画処理
	void Draw(void)override;
	// 解放処理
	void Release(void)override;

private:
	AnimationController* animation_;
	MicInput* mic_;

	int image_;

	int model_;

	VECTOR pos;
	VECTOR scale;
	VECTOR angle;

	int startCounter_;
	bool isStart_;
	int	voiceLevel_;

	void AddBoneScale(int index, VECTOR scale);

};