#include"TitleScene.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"

#include "../../Manager/MicInput/MicInput.h"
#include"../../Manager/Input/InputManager.h"
#include "../../Manager/Animation/AnimationController.h"
#include "../../Manager/Sound/SoundManager.h"

#include"../../Utility/Utility.h"

// コンストラクタ
TitleScene::TitleScene():
	pos(),
	scale(),
	angle(),
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
	image_ = LoadGraph("Data/Image/脳筋の拳_ロゴ.png");			// タイトル画像
	model_ = MV1LoadModel("Data/Model/Player/Player1.mv1");		// タイトル用のプレイヤー

	SoundManager::GetIns().Load(SOUND::TITLE_BGM);

	animation_ = new AnimationController(model_);
	animation_->Add((int)(ANIM_TYPE::IDLE), 30, "Data/Model/Player/Animation/Idle.mv1");	//タイトル用のプレイヤーに使うアイドルアニメーション
	animation_->Add((int)(ANIM_TYPE::ATTACK), 60, "Data/Model/Player/Animation/Punching.mv1");	//タイトル用のプレイヤーに使う攻撃アニメーション

	mic_ = new MicInput();
}

// 初期化処理
void TitleScene::Init(void)
{
	// プレイヤーの初期化
	pos = { 1000.0f,100.0f,0.0f, };
	angle = Utility::VECTOR_ZERO; 
	scale = { 2.0f,2.0f,2.0f };

	mic_->Init();
	mic_->Start();

	startCounter_ = 0;
	isStart_ = false;

	//SoundManager::GetIns().Play(SOUND::TITLE_BGM, true, 155, true, true);
}

// 更新処理
void TitleScene::Update(void)
{
	auto& input = InputManager::GetInstance();
	auto& scene = SceneManager::GetInstance();

	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		Application::GetInstance().GameEnd();
		return;
	}

	// どれかのキーが「押された瞬間」なら遷移
	if (input.IsTrgDown(KEY_INPUT_SPACE)) {
		isStart_ = true;
	}
	if (isStart_) {
		startCounter_++;
		animation_->Play((int)ANIM_TYPE::ATTACK,false);
		if (startCounter_ > 120) {
			scene.ChangeScene(SCENE_ID::GAME);
			return;
		}
	}

	AddBoneScale(4, { -0.03f,-0.03f,-0.03f });

	if (mic_->GetPlayGameLevel() > 4000) {
		AddBoneScale(4, { 0.04f,0.04f,0.04f });
	}

	if (!isStart_) {
		animation_->Play((int)ANIM_TYPE::IDLE, true);
	}

	animation_->Update();
	mic_->Update();
}

// 描画処理
void TitleScene::Draw(void)
{
	// タイトルロゴの描画
	VECTOR center = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
	DrawRotaGraph(
		center.x - 500,
		center.y,
		0.7f,0.0f,
		image_,
		true
		);

	// タイトル専用のプレイヤーの描画
	MATRIX mat = MGetIdent();

	mat = MMult(mat, MGetRotX(angle.x));
	mat = MMult(mat, MGetRotX(angle.y));
	mat = MMult(mat, MGetRotZ(angle.z));

	mat = MMult(MGetScale(scale), mat);

	mat.m[3][0] = pos.x;
	mat.m[3][1] = pos.y;
	mat.m[3][2] = pos.z;

	MV1SetMatrix(model_, mat);
	MV1DrawModel(model_);

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
	DeleteGraph(image_);
	MV1DeleteModel(model_);
	SoundManager::GetIns().Stop(SOUND::TITLE_BGM);
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
	MATRIX mat = MV1GetFrameLocalMatrix(model_, index);

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
	MV1SetFrameUserLocalMatrix(model_, index, scaleMat);
}

