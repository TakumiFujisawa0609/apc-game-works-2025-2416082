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

// ‰Šú‰»ˆ—
void SkyDome::SubInit(void)
{
	unit_.pos_ = POS;
	unit_.scale_ = SCALE;
	unit_.angle_ = ANGLE;
}

// XVˆ—
void SkyDome::SubUpdate(void)
{
}

// •`‰æˆ—
void SkyDome::SubDraw(void)
{
	MATRIX mat = MGetIdent();

	mat = MMult(MGetScale(unit_.scale_), mat);

	Utility::MatrixRotMult(mat, unit_.angle_);
	Utility::MatrixPosMult(mat, unit_.pos_);

	MV1SetMatrix(unit_.model_, mat);

	MV1DrawModel(unit_.model_);
}

// ‰ğ•úˆ—
void SkyDome::SubRelease(void)
{
	MV1DeleteModel(unit_.model_);
}
