#include "GameOverPlayer.h"

#include "../../Manager/Animation/AnimationController.h"

GameOverPlayer::GameOverPlayer()
{
}

GameOverPlayer::~GameOverPlayer()
{
}

void GameOverPlayer::SubLoad(void)
{
	animation_ = new AnimationController(unit_.model_);

	// パスの省略
	std::string playerModelPath = "Data/Model/Player/";

	// モデルのロード
	unit_.model_ = MV1LoadModel((playerModelPath + "Player1.mv1").c_str());
}

void GameOverPlayer::SubInit(void)
{
	int gameOverAnimation = static_cast<int>(ANIM_TYPE::GAME_OVER_PLAYER);
	animation_->Add(gameOverAnimation, 30.0f, "Data/Model/Player/Animation/GameOverPlayer.mv1");

	unit_.scale_ = Utility::VECTOR_ONE;
	unit_.angle_ = { 0.0f, Utility::Deg2RadF(180.0f),0.0f };
	unit_.pos_ = Utility::VECTOR_ZERO;
}

void GameOverPlayer::SubUpdate(void)
{
	int gameOverAnimation = static_cast<int>(ANIM_TYPE::GAME_OVER_PLAYER);

	animation_->Play(gameOverAnimation, true);
}

void GameOverPlayer::SubDraw(void)
{
	MATRIX mat = MGetIdent();

	mat = MMult(MGetScale(unit_.scale_), mat);
	Utility::MatrixRotMult(mat, unit_.angle_);
	Utility::MatrixPosMult(mat, unit_.pos_);

	MV1SetMatrix(unit_.model_, mat);
}

// 解放処理
void GameOverPlayer::SubRelease(void)
{
	// アニメーションクラス
	if (animation_) {
		animation_->Release();
		delete animation_;
		animation_ = nullptr;
	}

	MV1DeleteModel(unit_.model_);
}
