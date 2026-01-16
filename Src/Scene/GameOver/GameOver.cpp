#include"GameOver.h"

#include<DxLib.h>

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"
#include"../../Manager/Input/InputManager.h"
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
	auto& input = InputManager::GetInstance();

	if (input.IsTrgDown(KEY_INPUT_SPACE)) {
		SceneManager::GetInstance().ChangeScene(SCENE_ID::TITLE);
		return;
	}
}
void GameOver::Draw(void)
{
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, image_, true);
}
void GameOver::Release(void)
{
	DeleteGraph(image_);
}