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
}

void Stage::SubUpdate(void)
{
}

void Stage::SubDraw(void)
{
	MV1SetPosition(unit_.model_, unit_.pos_);
	MV1SetRotationXYZ(unit_.model_, unit_.angle_);
	MV1SetScale(unit_.model_, unit_.scale_);
}

void Stage::SubRelease(void)
{
	MV1DeleteModel(unit_.model_);
	DeleteGraph(tex);
}

void Stage::OnCollision(UnitBase* other)
{
}
