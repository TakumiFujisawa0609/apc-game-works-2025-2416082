#include"TitleScene.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"

#include"../../Manager/Input/InputManager.h"
#include "../../Manager/Animation/AnimationController.h"

#include"../../Utility/Utility.h"

// コンストラクタ
TitleScene::TitleScene():
	pos(),
	scale(),
	angle()
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

	animation_ = new AnimationController(model_);
	animation_->Add((int)(ANIM_TYPE::IDLE), 30, "Data/Model/Player/Animation/Idle.mv1");	//タイトル用のプレイヤーに使うアイドルアニメーション
	animation_->Add((int)(ANIM_TYPE::ATTACK), 60, "Data/Model/Player/Animation/Punching.mv1");	//タイトル用のプレイヤーに使う攻撃アニメーション
}

// 初期化処理
void TitleScene::Init(void)
{
	// プレイヤーの初期化
	pos = { 1000.0f,200.0f,0.0f, };
	angle = Utility::VECTOR_ZERO; 
	scale = { 2.0f,2.0f,2.0f };
}

// 更新処理
void TitleScene::Update(void)
{
	auto& input = InputManager::GetInstance();
	static bool is = false;

	// どれかのキーが「押された瞬間」なら遷移
	if (input.IsTrgDown(KEY_INPUT_SPACE)) {
		animation_->Play((int)ANIM_TYPE::ATTACK, false);
		is = true;
	}

	if (animation_->IsEnd((int)ANIM_TYPE::ATTACK)) {
		SceneManager::GetInstance().ChangeScene(SCENE_ID::GAME);
		return;
	}

	if (!is) {
		animation_->Play((int)ANIM_TYPE::IDLE, true);
	}

	animation_->Update();

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

	//const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };
	//MATRIX localMat = MGetIdent();

	//localMat = MMult(localMat, MGetRotX(LOCAL_ANGLE.x));
	//localMat = MMult(localMat, MGetRotY(LOCAL_ANGLE.y));
	//localMat = MMult(localMat, MGetRotZ(LOCAL_ANGLE.z));

	//mat = MMult(mat, localMat);

	mat = MMult(MGetScale(scale), mat);

	mat.m[3][0] = pos.x;
	mat.m[3][1] = pos.y;
	mat.m[3][2] = pos.z;

	MV1SetMatrix(model_, mat);
	MV1DrawModel(model_);

#ifdef _DEBUG
	SetFontSize(32);
	DrawString(0, 0, "タイトル", 0xffffff);
	SetFontSize(16);
#endif // _DEBUG
}

// 解放処理
void TitleScene::Release(void)
{
	DeleteGraph(image_);
	MV1DeleteModel(model_);

	if (animation_) {
		animation_->Release();
		delete animation_;
		animation_ = nullptr;
	}
}