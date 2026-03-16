#pragma once
#include"../SceneBase.h"
#include<DxLib.h>
#include<vector>

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

	enum class TITLE_IMAGE
	{
		TITLE_LOGO,
		NIKU,
		KIN,
		BACK_GROUND,
		PRESS_BUTTON,

		MAX
	};

	static constexpr VECTOR MAX_SIZE = { 2.0f,2.0f,2.0f };	// 筋肉のスケールの最大値
	static constexpr VECTOR MIN_ARM_MUSCLE = { 1.0f,1.0f,1.0f };	// 筋肉のスケールの最低値

	static constexpr int VOICE_GAUGE_MAX = 10;
	static constexpr float MUSCLE_IMAGE_SCALE_MAX = 8.0f;
	static constexpr float MUSCLE_IMAGE_SCALE_MIN = 3.0f;

	static constexpr int MUSCLE_INDEX = 4;

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

	// タイトル画面で表示する画像のハンドルID
	std::vector<int> titleImages_;

	// 画像の座標
	VECTOR imagePos_[10];
	int frameCounter_;

	struct SubBase
	{
		int model_;

		VECTOR pos;
		VECTOR scale;
		VECTOR angle;
	};

	SubBase unit_;

	int startCounter_;
	bool isStart_;
	int	voiceLevel_;

	float imageScale_;

	void AddBoneScale(int index, VECTOR scale);

};