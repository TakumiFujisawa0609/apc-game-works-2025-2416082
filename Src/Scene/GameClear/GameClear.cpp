#include "GameClear.h"

#include "../../Application/Application.h"
#include "../../Manager/Input/KeyManager.h"
#include "../SceneManager/SceneManager.h"

#include "../../Utility/Utility.h"

GameClear::GameClear() :
	image_(-1)
{

}

GameClear::~GameClear()
{

}

// 最初の一回しか呼ばれない処理
void GameClear::Load()
{
	Utility::LoadImg(image_, "Data/Image/GameClear/GameClear.png");
}

// 初期化処理
void GameClear::Init()
{
}

// 更新処理
void GameClear::Update()
{
	// 決定ボタンを押したらタイトルに戻る
	if (KeyManager::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetInstance().ChangeScene(SCENE_ID::TITLE);
		return;
	}
}

// 描画処理
void GameClear::Draw()
{
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, image_, true);

	SetFontSize(32);
	DrawString(Application::SCREEN_SIZE_X / 2 - (32 * 8), Application::SCREEN_SIZE_Y - 100, "PRESS SPACE OR X BUTTON", 0xffffff);
	SetFontSize(16);
}

// 解放処理
void GameClear::Release()
{
	DeleteGraph(image_);
}
