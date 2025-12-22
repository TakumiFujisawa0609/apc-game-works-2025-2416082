#include"GameScene.h"

#include<DxLib.h>
#include<cmath>

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Input/KeyManager.h"
#include "../../Manager/Sound/SoundManager.h"

#include "../../Application/Application.h"
#include "../../scene/SceneManager/SceneManager.h"

#include "../../Utility/Utility.h"

#include "../../Object/Camera/Camera.h"
#include "../../Object/Player/Player.h"
#include "../../Object/Player/Arm/LeftArm.h"
#include "../../Object/Player/Arm/RightArm.h"

#include "../../Object/Boss/Boss.h"
#include "../../Object/Boss/Attack/Hand/HandSlap.h"

#include "../../Object/Enemy/EnemyManager/EnemyManager.h"
#include "../../Object/Enemy/EnemyBase.h"

#include "../../Object/Stage/Stage.h"
#include "../../Object/SkyDome/SkyDome.h"

#include "../Title/TitleScene.h"
#include "../../Scene/PauseScene/PauseScene.h"

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
	stage_(nullptr),
	enemy_(nullptr),
	skyDome_(nullptr)
{

}

GameScene::~GameScene()
{
}



void GameScene::Load(void)
{
	this->Release();
	Camera::CreateInstance();

	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	// クラスの初期化--------------------------------------------------------------------
	auto ObjNewAndLoad = [&]<class T, class... Args>(T * &instance, Args && ... args)->void {
		Utility::ClassNew(instance, std::forward<Args>(args)...)->Load();
	};

	Utility::ClassNew(collision_);
	ObjNewAndLoad(player_);
	ObjNewAndLoad(boss_, player_->GetUnit().pos_);
	ObjNewAndLoad(enemy_, player_->GetUnit().pos_);
	ObjNewAndLoad(stage_);
	ObjNewAndLoad(skyDome_);
	// -------------------------------------------------------------------------------------


	// 当たり判定クラスに情報を渡す-------------
	collision_->AddEnemy(boss_);
	collision_->AddEnemy(boss_->GetRightHand());

	collision_->AddObject(player_);
 	collision_->AddObject(player_->GetLeftArm());
	collision_->AddObject(player_->GetRightArm());

	for (auto& enemy : enemy_->GetEnemy()) {
		collision_->AddEnemy(enemy);
	}
	// -------------------------------------------

	SoundManager& sound = SoundManager::GetIns();
	sound.Load(SOUND::GAME_BGM);
}

void GameScene::Init(void)
{
	player_->Init();
	boss_->Init();
	stage_->Init();
	enemy_->Init();
	skyDome_->Init();

	SetMouseDispFlag(false);

	Camera::GetInstance().SetTarget(&player_->GetCameraLocalPos(),&boss_->GetUnit().pos_);

	SoundManager::GetIns().Play(SOUND::GAME_BGM, false, 120, true, true);

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

	SceneManager& scene = SceneManager::GetInstance();
	InputManager& input = InputManager::GetInstance();
	Camera& camera = Camera::GetInstance();

	if (KeyManager::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		scene.PushScene(std::make_shared<PauseScene>());
	}

	if (boss_->GetState() == Boss::STATE::DEATH) {
		camera.SetBossDeathCamera();
	}

	if (!boss_->GetUnit().isAlive_) {
		this->Release();
		scene.JumpScene(SCENE_ID::TITLE);
		return;
	}

	if (!player_->GetUnit().isAlive_) {
		this->Release();
		scene.JumpScene(SCENE_ID::OVER);
		return;
	}

	player_->Update();

	boss_->Update();
	boss_->SetMuscleRatio(player_->GetMuscleRatio(4));
	boss_->SetVoiceLevel(player_->GetVoiceLevel());

	enemy_->Update();

	stage_->Update();

	skyDome_->Update();

	// 当たり判定
	collision_->Check();
	camera.Update();

#pragma endregion

}

void GameScene::Draw(void)
{
	// 画面演出のために描画先を自前で用意したスクリーンに設定
	SetDrawScreen(mainScreen_);
	ClearDrawScreen();

#pragma region 描画処理
	Camera::GetInstance().Apply();

	skyDome_->Draw();

	stage_->Draw();
	player_->Draw();
	enemy_->Draw();
	boss_->Draw();
#ifdef _DEBUG
	//grid_->Draw()

	SetFontSize(32);
	DrawString(0, 0, "ゲーム", 0xffffff);
	SetFontSize(16);
#endif // _DEBUG

	//UIの描画
	player_->UIDraw();

	boss_->UIDraw();

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

	Utility::SafeDeleteInstance(player_);
	Utility::SafeDeleteInstance(boss_);
	Utility::SafeDeleteInstance(enemy_);
	Utility::SafeDeleteInstance(stage_);
	Utility::SafeDeleteInstance(skyDome_);

	DeleteGraph(mainScreen_);
 	Camera::DeleteInstance();

	SoundManager& sound = SoundManager::GetIns();

	sound.AllStop();
	sound.Delete(SOUND::GAME_BGM);
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