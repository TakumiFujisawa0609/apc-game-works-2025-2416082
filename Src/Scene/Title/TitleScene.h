#pragma once
#include"../SceneBase.h"
#include<DxLib.h>

class AnimationController;


class TitleScene : public SceneBase
{
public:
	enum  class ANIM_TYPE
	{
		IDLE,
		ATTACK,

		MAX
	};

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

	int image_;

	int model_;

	VECTOR pos;
	VECTOR scale;
	VECTOR angle;
};