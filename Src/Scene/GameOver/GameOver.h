#pragma once
#include "../../Application/Application.h"
#include"../SceneBase.h"

class GameOver : public SceneBase
{
private:

	// 「PRESS_BUTTON」の描画位置
	static constexpr VECTOR PRESS_BUTTON_POS = { Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 + 300, 0.0f };
	static constexpr int PRESS_BUTTON_EX_RATE = 2.0f;

	enum class GAME_OVER_IMAGE
	{
		BACK_GROUND,
		PRESS_BUTTON,

		MAX
	};

public:
	GameOver();
	~GameOver()override;

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

	int gameOverImages_[(int)GAME_OVER_IMAGE::MAX];

};