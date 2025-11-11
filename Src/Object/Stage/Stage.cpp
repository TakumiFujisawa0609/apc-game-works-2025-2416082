#include "Stage.h"
#include "../../Utility/Utility.h"

Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::SubLoad(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Stage/Stage.mv1");
	tex = LoadGraph("Data/Model/Stage/Stage.png");
	MV1SetTextureGraphHandle(unit_.model_, 0, tex, true);
}

void Stage::SubInit(void)
{
	unit_.pos_ = DEFAULT_POS;
	unit_.angle_ = Utility::VECTOR_ZERO;
	unit_.scale_ = SCALE;
	unit_.isAlive_ = true;
}

void Stage::SubUpdate(void)
{
}

void Stage::SubDraw(void)
{
	MATRIX mat = MGetIdent();

	mat = MMult(MGetScale(unit_.scale_), mat);

	Utility::MatrixRotMult(mat, unit_.angle_);
	Utility::MatrixPosMult(mat, unit_.pos_);

	MV1SetMatrix(unit_.model_, mat);
	MV1DrawModel(unit_.model_);
}

void Stage::SubRelease(void)
{
	MV1DeleteModel(unit_.model_);
	DeleteGraph(tex);
}

void Stage::OnCollision(UnitBase* other)
{
}
