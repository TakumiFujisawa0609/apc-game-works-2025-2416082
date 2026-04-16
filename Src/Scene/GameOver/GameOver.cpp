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

// Å‰‚Ìˆê‰ñ‚µ‚©ŒÄ‚Î‚ê‚È‚¢ˆ—
void GameOver::Load(void)
{
	Utility::LoadImg(gameOverImages_[(int)GAME_OVER_IMAGE::BACK_GROUND], "Data/Image/GameOver/GameOver.png");
	Utility::LoadImg(gameOverImages_[(int)GAME_OVER_IMAGE::PRESS_BUTTON], "Data/Image/Title/PRESS_BUTTON_UI.png");
}

// ‰Šú‰»ˆ—
void GameOver::Init(void)
{
}

// XVˆ—
void GameOver::Update(void)
{
	if (KeyManager::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetInstance().ChangeScene(SCENE_ID::TITLE);
		return;
	}
}

// •`‰æˆ—
void GameOver::Draw(void)
{
	// ”wŒi‚Ì•`‰æ
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, gameOverImages_[(int)GAME_OVER_IMAGE::BACK_GROUND], true);
	
	//uPRESS_BUTTONv‚Ì•`‰æ
	DrawRotaGraph(
		PRESS_BUTTON_POS.x,
		PRESS_BUTTON_POS.y,
		PRESS_BUTTON_EX_RATE, 0.0f,
		gameOverImages_[(int)GAME_OVER_IMAGE::PRESS_BUTTON],
		true
	);
}

// ‰ğ•úˆ—
void GameOver::Release(void)
{
	for(int i = 0; i < (int)GAME_OVER_IMAGE::MAX; i++) {
		DeleteGraph(gameOverImages_[i]);
	}
}