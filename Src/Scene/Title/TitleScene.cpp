#include"TitleScene.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"

#include"../../Manager/Input/InputManager.h"
#include "../../Manager/Animation/AnimationController.h"

#include"../../Utility/Utility.h"

TitleScene::TitleScene():
	pos(),
	scale(),
	angle()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Load(void)
{
	image_ = LoadGraph("Data/Image/”]‹Ø‚ÌŒ_ƒƒS.png");

	model_ = MV1LoadModel("Data/Model/Player/Player1.mv1");

	animation_ = new AnimationController(model_);
	animation_->Add((int)(ANIM_TYPE::IDLE), 120, "Data/Model/Player/Animation/Idle.mv1");
}
void TitleScene::Init(void)
{
	pos = { 1000.0f,0.0f,0.0f, };
	angle = Utility::VECTOR_ZERO; 
	scale = { 1.0f,1.0f,1.0f };

	animation_->Play((int)ANIM_TYPE::IDLE, true);
}
void TitleScene::Update(void)
{
	auto& input = InputManager::GetInstance();

	// ‚Ç‚ê‚©‚ÌƒL[‚ªu‰Ÿ‚³‚ê‚½uŠÔv‚È‚ç‘JˆÚ
	if (input.IsTrgDown(KEY_INPUT_SPACE)) {
		SceneManager::GetInstance().ChangeScene(SCENE_ID::GAME);
		return;
	}

	animation_->Update();
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

	mat = MMult(mat, MGetRotX(angle.x));
	mat = MMult(mat, MGetRotX(angle.y));
	mat = MMult(mat, MGetRotZ(angle.z));

	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };
	MATRIX localMat = MGetIdent();

	localMat = MMult(localMat, MGetRotX(LOCAL_ANGLE.x));
	localMat = MMult(localMat, MGetRotY(LOCAL_ANGLE.y));
	localMat = MMult(localMat, MGetRotZ(LOCAL_ANGLE.z));

	mat = MMult(mat, localMat);

	mat = MMult(MGetScale(scale), mat);

	mat.m[3][0] = pos.x;
	mat.m[3][1] = pos.y;
	mat.m[3][2] = pos.z;

	MV1SetMatrix(model_, mat);

	MV1DrawModel(model_);

	SetFontSize(32);
	DrawString(0, 0, "ƒ^ƒCƒgƒ‹", 0xffffff);
	SetFontSize(16);
}
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