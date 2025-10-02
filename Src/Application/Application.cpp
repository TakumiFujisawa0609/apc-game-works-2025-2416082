#include "Application.h"

#include"../Manager/FPS/FPS.h"
#include"../Scene/SceneManager/SceneManager.h"

Application* Application::instance_ = nullptr;

// コンストラクタ
Application::Application(void):
	isInitFail_(false),
	isReleaseFail_(false),

	gameEnd_(false),

	fps_(nullptr)
{
}

// デストラクタ
Application::~Application(void)
{
}

// 初期化
void Application::Init(void)
{
	// アプリケーションの初期設定
	SetWindowText("");

	// ウィンドウ関連
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);	// サイズ変更
	ChangeWindowMode(true);	// false = フルスクリーン

	SetMouseDispFlag(false);

	// DxLibの初期化
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		// 初期化失敗
		isInitFail_ = true;
		return;
	}

	// 描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	// シーン管理初期化
	SceneManager::CreateInstance();
	SceneManager::GetInstance().Init();

	// FPS初期化
	fps_ = new FPS;
	fps_->Init();
}

// ゲームループ
void Application::Run(void)
{
	// ゲームループ
	while (ProcessMessage() == 0 && !gameEnd_)
	{
		// フレームレート更新
		// 1/60秒経過していないなら再ループさせる
		if (!fps_->UpdateFrameRate()) continue;

		SceneManager::GetInstance().Update();	// シーン管理更新
		fps_->CalcFrameRate();					// フレームレート計算

		ClearDrawScreen();

		SceneManager::GetInstance().Draw();		// シーン管理描画
		fps_->DrawFrameRate();					// フレームレート描画

		ScreenFlip();
	}
}

// 解放
void Application::Release(void)
{
	// シーン管理解放・削除	
	SceneManager::GetInstance().Release();
	SceneManager::DeleteInstance();

	// フレームレート解放
	delete fps_;

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}
}
