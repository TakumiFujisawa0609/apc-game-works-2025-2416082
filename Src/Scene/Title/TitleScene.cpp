#include"TitleScene.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"

#include "../../Manager/MicInput/MicInput.h"
#include"../../Manager/Input/InputManager.h"
#include "../../Manager/Input/KeyManager.h"
#include "../../Manager/Animation/AnimationController.h"
#include "../../Manager/Sound/SoundManager.h"

#include"../../Utility/Utility.h"

// コンストラクタ
TitleScene::TitleScene():
	mic_(nullptr)
{
}

// デストラクタ
TitleScene::~TitleScene()
{

}

// 最初に一度だけ呼び出す処理
void TitleScene::Load(void)
{
	//titleLogoImage_ = LoadGraph("Data/Image/Title/脳筋の拳_ロゴ.png");			// タイトル画像
	Utility::LoadImg(titleLogoImage_, "Data/Image/Title/脳筋の拳_ロゴ.png");

	Utility::LoadImg(nikuImage_, "Data/Image/Title/NIKU.png");
	Utility::LoadImg(kinImage_, "Data/Image/Title/KIN.png");
	Utility::LoadImg(haikeiImage_, "Data/Image/Title/haikei.jpg");
	
	unit_.model_ = MV1LoadModel("Data/Model/Player/Player1.mv1");		// タイトル用のプレイヤー

	SoundManager::GetIns().Load(SOUND::TITLE_BGM);

	animation_ = new AnimationController(unit_.model_);
	animation_->Add((int)(ANIM_TYPE::IDLE), 30, "Data/Model/Player/Animation/Idle.mv1");	//タイトル用のプレイヤーに使うアイドルアニメーション
	animation_->Add((int)(ANIM_TYPE::ATTACK), 60, "Data/Model/Player/Animation/Punching.mv1");	//タイトル用のプレイヤーに使う攻撃アニメーション

	mic_ = new MicInput();
}

// 初期化処理
void TitleScene::Init(void)
{
	// プレイヤーの初期化
	unit_.pos = { 1000.0f,100.0f,0.0f, };
	unit_.angle = Utility::VECTOR_ZERO;
	unit_.scale = { 2.0f,2.0f,2.0f };

	mic_->Init();
	mic_->Start();

	startCounter_ = 0;
	isStart_ = false;

	imageScale_ = 0.5f;

	frameCounter_ = 0;

	// 初期化（最初の一回だけ）
	for (int i = 0; i < 10; i++) {
		imagePos_[i] = VGet(
			GetRand(Application::SCREEN_SIZE_X),
			GetRand(Application::SCREEN_SIZE_Y),
			0.0f
		);
	}

	//SoundManager::GetIns().Play(SOUND::TITLE_BGM, true, 155, true, true);
}

// 更新処理
void TitleScene::Update(void)
{
	if (Application::GetInstance().GetGameEnd()) { return; }

	InputManager& input = InputManager::GetInstance();
	SceneManager& scene = SceneManager::GetInstance();

	if (KeyManager::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		Application::GetInstance().GameEnd();
		SoundManager::GetIns().Stop(SOUND::TITLE_BGM);
	}

	// どれかのキーが「押された瞬間」なら遷移
	if (input.IsTrgDown(KEY_INPUT_SPACE) || 
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,InputManager::JOYPAD_BTN::TOP) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT)) {
		isStart_ = true;
	}
	if (isStart_) {
		startCounter_++;
		animation_->Play((int)ANIM_TYPE::ATTACK,false);
		scene.JumpScene(SCENE_ID::GAME);
		return;
	}

	// プレイヤーのモデルのスケールを減らし続ける
	AddBoneScale(MUSCLE_INDEX, { -0.03f,-0.03f,-0.03f });


	// 音量が4000を超えたときの処理
	// プレイヤーと「筋」「肉」の画像のスケールを一定の大きさまで増やす
	if (mic_->GetPlayGameLevel() > 4000)
	{
		AddBoneScale(MUSCLE_INDEX, { 0.04f,0.04f,0.04f });

		if (imageScale_ <= KINNIKU_IMAGE_SCALE_MAX) {
			imageScale_ += 0.5f;
		}
	}

#pragma region 画像関係の更新処理
	// 10フレームごとに座標を更新
	frameCounter_++;
	if (frameCounter_ % 10 == 0) {
		for (int i = 0; i < 10; i++) {
			imagePos_[i] = VGet(
				GetRand(Application::SCREEN_SIZE_X),
				GetRand(Application::SCREEN_SIZE_Y),
				0.0f
			);
		}
	}

	// 画像の大きさを減らし続ける
	imageScale_ -= 0.03f;

	// 画像の大きさが最大値に達したときの制限
	if (imageScale_ > KINNIKU_IMAGE_SCALE_MAX) {
		imageScale_ = KINNIKU_IMAGE_SCALE_MAX;
	}

	// 画像の大きさが最低値に達したときの制限
	if (imageScale_ <= KINNIKU_IMAGE_SCALE_MIN) {
		imageScale_ = 3.0f;
	}
#pragma endregion 


	if (!isStart_) {
		animation_->Play((int)ANIM_TYPE::IDLE, true);
	}

	animation_->Update();
	mic_->Update();
}

// 描画処理
void TitleScene::Draw(void)
{
	VECTOR center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };


	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, haikeiImage_, true);


	// 描画
	for (int i = 0; i < 10; i++) {
		DrawRotaGraph(
			static_cast<int>(imagePos_[i].x),
			static_cast<int>(imagePos_[i].y),
			imageScale_, 0.0f,
			((i % 2) == 1) ? kinImage_ : nikuImage_,
			true
		);
	}

	// タイトルロゴの描画
	DrawRotaGraph(
		center.x - 500,
		center.y,
		0.5f, 0.0f,
		titleLogoImage_,
		true
		);

	// タイトル専用のプレイヤーの描画
	MATRIX mat = MGetIdent();

	mat = MMult(mat, MGetRotX(unit_.angle.x));
	mat = MMult(mat, MGetRotX(unit_.angle.y));
	mat = MMult(mat, MGetRotZ(unit_.angle.z));

	mat = MMult(MGetScale(unit_.scale), mat);

	//mat.m[3][0] = pos.x;
	//mat.m[3][1] = pos.y;
	//mat.m[3][2] = pos.z;

	Utility::MatrixPosMult(mat, unit_.pos);

	MV1SetMatrix(unit_.model_, mat);
	MV1DrawModel(unit_.model_);

#ifdef _DEBUG
	SetFontSize(32);
	DrawFormatString(0, 0, 0xffffff,"マイクレベル %i", mic_->GetLevel());
	SetFontSize(16);

	mic_->VoiceLevelDraw();
#endif // _DEBUG

}

// 解放処理
void TitleScene::Release(void)
{
	DeleteGraph(titleLogoImage_);
	DeleteGraph(nikuImage_);
	DeleteGraph(kinImage_);
	DeleteGraph(haikeiImage_);
	MV1DeleteModel(unit_.model_);
	SoundManager::GetIns().Delete(SOUND::TITLE_BGM);

	if (animation_) {
		animation_->Release();
		delete animation_;
		animation_ = nullptr;
	}

	if (mic_) {
		mic_->Stop();
		delete mic_;
		mic_ = nullptr;
	}
}

void TitleScene::AddBoneScale(int index, VECTOR scale)
{
	MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, index);

	// 行列からスケール成分を抽出
	float currentScale[3];
	for (int i = 0; i < 3; i++) {
		currentScale[i] = VSize(VGet(mat.m[i][0], mat.m[i][1], mat.m[i][2]));
	}

	// スケール加算
	VECTOR newScale = VAdd(scale, { currentScale[0], currentScale[1], currentScale[2] });

	// 最大値の制限
	if (newScale.x > MAX_SIZE.x) newScale.x = MAX_SIZE.x;
	if (newScale.y > MAX_SIZE.y) newScale.y = MAX_SIZE.y;
	if (newScale.z > MAX_SIZE.z) newScale.z = MAX_SIZE.z;

	// 最低値の制限
	if (newScale.x < MIN_ARM_MUSCLE.x) newScale.x = MIN_ARM_MUSCLE.x;
	if (newScale.y < MIN_ARM_MUSCLE.y) newScale.y = MIN_ARM_MUSCLE.y;
	if (newScale.z < MIN_ARM_MUSCLE.z) newScale.z = MIN_ARM_MUSCLE.z;

#ifdef _DEBUG
	// 筋肉量を確認する用の処理(デバッグ用)
	float avgScale = (newScale.x + newScale.y + newScale.z) / 3.0f;
	float avgMin = (MIN_ARM_MUSCLE.x + MIN_ARM_MUSCLE.y + MIN_ARM_MUSCLE.z) / 3.0f;
	float avgMax = (MAX_SIZE.x + MAX_SIZE.y + MAX_SIZE.z) / 3.0f;

	float muscleRatio_ = (avgScale - avgMin) / (avgMax - avgMin);
#endif // _DEBUG

	// スケール行列を作成
	MATRIX scaleMat = MGetScale(newScale);

	// 適用
	MV1SetFrameUserLocalMatrix(unit_.model_, index, scaleMat);
}

