#include "BossBase.h"

#include <DxLib.h>

BossBase::BossBase()
{
}

BossBase::~BossBase()
{
}

void BossBase::Load(void)
{
}

void BossBase::Init(void)
{
    unit_.pos_ = DEFAULT_POS;
}

void BossBase::Update(void)
{
}

void BossBase::Draw(void)
{
    MakeBoxByCenter(unit_.pos_, 50.0f, 50.0f, 50.0f);
}

void BossBase::Release(void)
{
}

void BossBase::OnCollision(UnitBase* other)
{
}

VECTOR BossBase::MakeBoxByCenter(VECTOR center, float sizeX, float sizeY, float sizeZ)
{
    // 中心から半分の長さ
    float hx = sizeX * 0.5f;
    float hy = sizeY * 0.5f;
    float hz = sizeZ * 0.5f;

    // ボックスの左下手前と右上奥の座標を計算
    VECTOR min = { center.x - hx, center.y - hy, center.z - hz };
    VECTOR max = { center.x + hx, center.y + hy, center.z + hz };

    // DXライブラリのボックス描画関数を呼び出し（戻り値は無視）
    DrawCube3D(min, max, 0xffffff, 0xffffff, true);

    // 中心座標を返す
    return center;
}