#include "GameClear.h"

#include "../../Application/Application.h"
#include "../../Manager/Input/KeyManager.h"
#include "../SceneManager/SceneManager.h"

#include "../../Object/SkyDome/SkyDome.h"

#include "../../Utility/Utility.h"

GameClear::GameClear() :
	image_(-1)
{

}

GameClear::~GameClear()
{

}

void GameClear::Load()
{
	Utility::LoadImg(image_, "Data/Image/GameClear/GameClear.png");

	Utility::ClassNew(skyDome_)->Load();
}

void GameClear::Init()
{
	skyDome_->Init();
}

void GameClear::Update()
{
	skyDome_->Update();

	if (KeyManager::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetInstance().ChangeScene(SCENE_ID::TITLE);
		return;
	}
}

void GameClear::Draw()
{
	skyDome_->Draw();
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, image_, true);
}

void GameClear::Release()
{
	skyDome_->Release();
	DeleteGraph(image_);
}
