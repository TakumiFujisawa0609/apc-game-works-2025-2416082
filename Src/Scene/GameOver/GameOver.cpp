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

	static char key[256];
	static char prevKey[256];

	// 前フレームの状態を保存
	memcpy(prevKey, key, 256);

	// 現在のキー状態を取得
	GetHitKeyStateAll(key);

	// どれかのキーが「押された瞬間」なら遷移
	for (int i = 0; i < 256; i++) {
		if (key[i] != 0 && prevKey[i] == 0 || input.IsTrgMouseLeft() || input.IsTrgMouseRight()) {
			SceneManager::GetInstance().ChangeScene(SCENE_ID::TITLE);
			break; // 1つでも押されたらOK
		}
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