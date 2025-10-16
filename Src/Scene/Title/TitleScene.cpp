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

	model_ = MV1LoadModel("Data/Model/Player/Player1.mv1");
}
void TitleScene::Init(void)
{
	pos_ = Utility::VECTOR_ZERO;
	angle_ = Utility::VECTOR_ZERO; 
	scale_ = { 1.0f,1.0f,1.0f };
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

	MATRIX mat = MGetIdent();

	mat = MMult(mat, MGetRotX(angle_.x));
	mat = MMult(mat, MGetRotX(angle_.y));
	mat = MMult(mat, MGetRotZ(angle_.z));

	MATRIX localMat;

	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };

	localMat = MMult(localMat, MGetRotX(LOCAL_ANGLE.x));
	localMat = MMult(localMat, MGetRotY(LOCAL_ANGLE.y));
	localMat = MMult(localMat, MGetRotZ(LOCAL_ANGLE.z));

	mat = MMult(mat, localMat);

	mat = MMult(MGetScale(scale_), mat);

	mat.m[3][0] = pos_.x;
	mat.m[3][1] = pos_.y;
	mat.m[3][2] = pos_.z;

	MV1SetMatrix(model_, mat);

	MV1DrawModel(model_);

	SetFontSize(32);
	DrawString(0, 0, "タイトル", 0xffffff);
	SetFontSize(16);
}
void TitleScene::Release(void)
{
	DeleteGraph(image_);
}