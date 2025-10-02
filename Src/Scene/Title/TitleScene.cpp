#include"TitleScene.h"

#include<DxLib.h>

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"
#include"../../Manager/Input/InputManager.h"
#include"../../Utility/Utility.h"


TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Load(void)
{
}
void TitleScene::Init(void)
{
}
void TitleScene::Update(void)
{
	auto& input = InputManager::GetInstance();
	if (input.IsTrgDown(KEY_INPUT_SPACE)) {
		SceneManager::GetInstance().ChangeScene(SCENE_ID::GAME);
	}
}
void TitleScene::Draw(void)
{
	SetFontSize(32);
	DrawString(0, 0, "ƒ^ƒCƒgƒ‹", 0xffffff);
	SetFontSize(16);
}
void TitleScene::Release(void)
{
}