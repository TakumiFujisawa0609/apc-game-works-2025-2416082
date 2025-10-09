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
	image_ = LoadGraph("Data/Image/î]ãÿÇÃåù_ÉçÉS.png");
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
	VECTOR center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
	DrawRotaGraph(
		center.x - 500,
		center.y,
		0.7f,0.0f,
		image_,
		true
		);

	SetFontSize(32);
	DrawString(0, 0, "É^ÉCÉgÉã", 0xffffff);
	SetFontSize(16);
}
void TitleScene::Release(void)
{
	DeleteGraph(image_);
}