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
	image_ = LoadGraph("Data/Image/脳筋の拳_ロゴ.png");
}
void TitleScene::Init(void)
{
}
void TitleScene::Update(void)
{
	auto& input = InputManager::GetInstance();

	static char key[256];
	static char prevKey[256];

	// 前フレームの状態を保存
	memcpy(prevKey, key, 256);

	// 現在のキー状態を取得
	GetHitKeyStateAll(key);

	// どれかのキーが「押された瞬間」なら遷移
	for (int i = 0; i < 256; i++) {
		if (key[i] != 0 && prevKey[i] == 0 || input.IsTrgMouseLeft() || input.IsTrgMouseRight()) {
			SceneManager::GetInstance().ChangeScene(SCENE_ID::GAME);
			break; // 1つでも押されたらOK
		}
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
	DrawString(0, 0, "タイトル", 0xffffff);
	SetFontSize(16);
}
void TitleScene::Release(void)
{
	DeleteGraph(image_);
}