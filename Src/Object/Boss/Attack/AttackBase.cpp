#include "AttackBase.h"



AttackBase::AttackBase(const VECTOR& boss, const VECTOR& player, const int& voiceLevel) :
	boss_(boss),
	player_(player),
	voiceLevel_(voiceLevel)
{
}

AttackBase::~AttackBase()
{
}

void AttackBase::SubLoad()
{
}

void AttackBase::SubInit()
{
}

void AttackBase::SubUpdate()
{
}

void AttackBase::SubDraw()
{
    if (attack_.end_ || !unit_.isAlive_) return;

    MATRIX mat = MGetIdent();

    mat = MMult(MGetScale(unit_.scale_), mat);

    Utility::MatrixRotMult(mat, unit_.angle_);
    Utility::MatrixPosMult(mat, unit_.pos_);

    // モデル描画
    MV1SetMatrix(unit_.model_, mat);

    // デバッグ描画
    DebugDraw();

    if (!unit_.isAlive_ && attack_.end_) { return; }

    MV1DrawModel(unit_.model_);
}

void AttackBase::SubRelease()
{
    if (unit_.model_ != -1) {
        MV1DeleteModel(unit_.model_);
    }
}


void AttackBase::ParryUpdate(void)
{
}


