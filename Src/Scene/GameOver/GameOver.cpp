#include"GameOver.h"

#include<DxLib.h>

#include "../../Manager/Input/KeyManager.h"
#include"../SceneManager/SceneManager.h"
#include"../../Application/Application.h"

#include"../../Utility/Utility.h"


GameOver::GameOver()
{
}

GameOver::~GameOver()
{
}

void GameOver::Load(void)
{
	image_ = LoadGraph("Data/Image/GameOver/GameOver.png");
}
void GameOver::Init(void)
{
}
void GameOver::Update(void)
{
	if (KeyManager::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetInstance().ChangeScene(SCENE_ID::TITLE);
		return;
	}



}
void GameOver::Draw(void)
{
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, image_, true);

	SetFontSize(32);
	DrawString(Application::SCREEN_SIZE_X / 2 - (32 * 8), Application::SCREEN_SIZE_Y - 100, "Press Enter or X Button", 0xffffff);
	SetFontSize(16);
}
void GameOver::Release(void)
{
	DeleteGraph(image_);
}