#include "Boss.h"

Boss::Boss()
{
}

Boss::~Boss()
{
}

void Boss::Load(void)
{
}

void Boss::Init(void)
{
    unit_.pos_ = DEFAULT_POS;
}

void Boss::Update(void)
{
}

void Boss::Draw(void)
{
    MakeBoxByCenter(unit_.pos_, 50.0f, 50.0f, 50.0f);
}

void Boss::Release(void)
{
}

void Boss::OnCollision(UnitBase* other)
{
}

VECTOR Boss::MakeBoxByCenter(VECTOR center, float sizeX, float sizeY, float sizeZ)
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
