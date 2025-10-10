#include"GameScene.h"

#include<DxLib.h>
#include<cmath>

#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Input/InputManager.h"

#include"../../Application/Application.h"
#include"../../scene/SceneManager/SceneManager.h"

#include"../../Utility/Utility.h"

#include"../../Object/Player/Player.h"
#include"../../Object/Player/Arm/LeftArm.h"
#include"../../Object/Player/Arm/RightArm.h"

#include"../../Object/Boss/Boss.h"
#include"../../Object/Grid/Grid.h"

#include"../Title/TitleScene.h"
#include"../../Scene/PauseScene/PauseScene.h"

int GameScene::hitStop_ = 0;

int GameScene::slow_ = 0;
int GameScene::slowInter_ = 0;

int GameScene::shake_ = 0;
ShakeKinds GameScene::shakeKinds_ = ShakeKinds::DIAG;
ShakeSize GameScene::shakeSize_ = ShakeSize::MEDIUM;

GameScene::GameScene() :
	mainScreen_(-1),
	collision_(nullptr),
	player_(nullptr),
	boss_(nullptr),
	grid_(nullptr)
{
}

GameScene::~GameScene()
{
}

void GameScene::Load(void)
{
	this->Release();

	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	collision_ = new Collision();

	player_ = new Player();
	player_->Load();

	boss_ = new Boss();
	boss_->Load();

	grid_ = new Grid();

	collision_->AddEnemy(boss_);

	collision_->AddObject(player_);
 	collision_->AddObject(player_->GetLeftArm());
	collision_->AddObject(player_->GetRightArm());

	Camera::CreateInstance();
}

void GameScene::Init(void)
{
	player_->Init();
	boss_->Init();
	grid_->Init();

	SetMouseDispFlag(false);

	Camera::GetInstance().SetTarget(&player_->GetCameraLocalPos());

#pragma region 画面演出
	// ヒットストップカウンターの初期化
	hitStop_ = 0;

	// スローカウンターの初期化
	slow_ = 0;
	slowInter_ = 5;

	// 画面揺れ関係の初期化-----------------------------------------------------------
	shake_ = 0;
	shakeKinds_ = ShakeKinds::DIAG;
	shakeSize_ = ShakeSize::MEDIUM;
	//--------------------------------------------------------------------------------
#pragma endregion
}



void GameScene::Update(void)
{
	// 画面演出更新
	if (ScreenProduction()) { return; }


#pragma region オブジェクト更新処理

	//if (CheckHitKey(KEY_INPUT_SPACE) == 1) {

	//	//シーン遷移の書き方2種類
	//	
	//	//今まで通りの
	//	SceneManager::GetInstance().ChangeScene(SCENE_ID::TITLE);

	//	//列挙型(enum)とかswitchとか書かないやり方
	//	SceneManager::GetInstance().ChangeScene(std::make_shared<TitleScene>());
	//	
	//	//まあ実際、上のは使いやすいように下をオーバーロードしたものだから内部的には一緒

	//	return;
	//}

	Camera::GetInstance().Update();
	player_->Update();
	boss_->Update();
	grid_->Update();

	auto& input = InputManager::GetInstance();
	auto& scene = SceneManager::GetInstance();
	if (input.IsTrgDown(KEY_INPUT_ESCAPE))
	{
		scene.PushScene(std::make_shared<PauseScene>());
	}

	// 当たり判定
	collision_->Check();

#pragma endregion

	if (!boss_->GetUnit().isAlive_) {
		scene.ChangeScene(SCENE_ID::TITLE);
	}
}



void GameScene::Draw(void)
{

	// 画面演出のために描画先を自前で用意したスクリーンに設定
	SetDrawScreen(mainScreen_);
	ClearDrawScreen();

#pragma region 描画処理
	Camera::GetInstance().Apply();

	//DrawCube3D({ 10000,0,10000 }, { -10000,0,-10000 }, 0x000000, 0x000000, true);

	using app = Application;
	int x = app::SCREEN_SIZE_X / 2;
	int y = app::SCREEN_SIZE_Y / 2;

	player_->Draw();
	boss_->Draw();
	grid_->Draw();

	SetFontSize(32);
	DrawString(0, 0, "ゲーム", 0xffffff);
	SetFontSize(16);

#pragma endregion

	// 演出を含めた描画をもとのDX_SCREEN_BACKに書き込む

	SetDrawScreen(DX_SCREEN_BACK);

	Vector2I s = ShakePoint();
	DrawGraph(s.x, s.y, mainScreen_, true);
}

void GameScene::Release(void)
{
	if (collision_) {
		collision_->Clear();
		delete collision_;
		collision_ = nullptr;
	}

	if (player_) {
		player_->Release();
		delete player_;
		player_ = nullptr;
	}

	if (boss_)
	{
		boss_->Release();
		delete boss_;
		boss_ = nullptr;
	}

	if (grid_)
	{
		grid_->Release();
		delete grid_;
		grid_ = nullptr;
	}

	DeleteGraph(mainScreen_);
 	Camera::DeleteInstance();
}

void GameScene::Shake(ShakeKinds kinds, ShakeSize size, int time)
{
	if ((abs(shake_ - time) > 10) || shake_ <= 0)shake_ = time;
	shakeKinds_ = kinds;
	shakeSize_ = size;
}

bool GameScene::ScreenProduction(void)
{
	if (hitStop_ > 0) { hitStop_--; return true; }
	if (shake_ > 0) { shake_--; }
	if (slow_ > 0) {
		if (--slow_ % slowInter_ != 0) { return true; }
	}

	return false;
}

Vector2I GameScene::ShakePoint(void)
{
	Vector2I ret = {};

	if (shake_ > 0) {
		int size = shake_ / 5 % 2;
		size *= 2;
		size -= 1;
		switch (shakeKinds_)
		{
		case GameScene::WID:ret.x = size;
			break;
		case GameScene::HIG:ret.y = size;
			break;
		case GameScene::DIAG:ret = size;
			break;
		case GameScene::ROUND:
			size = shake_ / 3 % 12; size++;
			ret = { (int)(((int)shakeSize_ * 1.5f) * cos(size * 30.0f)),(int)(((int)shakeSize_ * 1.5f) * sin(size * 30.0f)) };
			break;
		}

		if (shakeKinds_ != ShakeKinds::ROUND) { ret *= shakeSize_; }

		DrawGraph(0, 0, mainScreen_, true);
	}

	return ret;
}