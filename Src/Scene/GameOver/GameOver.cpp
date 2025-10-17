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
}
void GameOver::Init(void)
{
}
void GameOver::Update(void)
{
	auto& input = InputManager::GetInstance();


	// どれかのキーが「押された瞬間」なら遷移
		if (input.IsTrgDown(KEY_INPUT_SPACE)) {
			SceneManager::GetInstance().ChangeScene(SCENE_ID::TITLE);
			return;
	}
}
void GameOver::Draw(void)
{
	SetFontSize(32);
	DrawString(0, 0, "ゲームオーバー", 0xffffff);
	SetFontSize(16);
}
void GameOver::Release(void)
{
}