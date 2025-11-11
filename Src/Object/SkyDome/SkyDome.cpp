#include "SkyDome.h"

#include "../../Utility/Utility.h"

SkyDome::SkyDome()
{
}

SkyDome::~SkyDome()
{
}

void SkyDome::SubLoad(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Stage/Skydome.mv1");
	tex_ = LoadGraph("Data/Model/Stage/Skydome.png");
	MV1SetTextureGraphHandle(unit_.model_, 0, tex_, true);
}

void SkyDome::SubInit(void)
{
	unit_.pos_ = { 0.0f,0.0f,0.0f };
	unit_.scale_ = { 100.0f,100.f,100.0f };
	unit_.angle_ = { 0.0f,0.0f,0.0f };
}

void SkyDome::SubUpdate(void)
{
}

void SkyDome::SubDraw(void)
{
	MATRIX mat = MGetIdent();

	mat = MMult(MGetScale(unit_.scale_), mat);

	Utility::MatrixRotMult(mat, unit_.angle_);
	Utility::MatrixPosMult(mat, unit_.pos_);

	MV1SetMatrix(unit_.model_, mat);

	MV1DrawModel(unit_.model_);
}

void SkyDome::SubRelease(void)
{
	MV1DeleteModel(unit_.model_);
}
