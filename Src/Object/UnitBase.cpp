#include "UnitBase.h"

#include <DxLib.h>
#include <algorithm>

#include"../Scene/SceneManager/SceneManager.h"

UnitBase::UnitBase() {}
UnitBase::~UnitBase() {}

void UnitBase::Load(void)
{
	SubLoad();
}

void UnitBase::Init(void)
{
	SubInit();
}

void UnitBase::Update(void)
{
	SubUpdate();
}

void UnitBase::Draw(void)
{
	SubDraw();

	if (unit_.inviciCounter_ > 0)
	{
		float t = sinf(GetNowCount() * 0.03f);
		float intensity = (t > 0.0f) ? 1.0f : 0.1f;

		MV1SetDifColorScale(unit_.model_, { 1.0f, intensity * 0.5f, intensity * 0.5f, 1.0f });
	}
	else
	{
		MV1SetDifColorScale(unit_.model_, { 1.0f, 1.0f, 1.0f, 1.0f });
	}
}

void UnitBase::Release(void)
{
	stateFuncs_.clear();
	SubRelease();
}

// 子クラスで定義した関数を関数ポインタに登録
void UnitBase::StateAdd(int state, StateFunc func)
{
	stateFuncs_[state] = func;
}


void UnitBase::StateUpdate(int state)
{
	auto it = stateFuncs_.find(state);
	if (it != stateFuncs_.end()) {
		it->second();
	}
}

void UnitBase::Invi(void)
{
	if (unit_.inviciCounter_ > 0) { unit_.inviciCounter_--; }
}



void UnitBase::AddBoneScale(int index, VECTOR scale)
{
	MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, index);

	// 行列からスケール成分を抽出
	float currentScale[3];
	for (int i = 0; i < 3; i++) {
		currentScale[i] = VSize(VGet(mat.m[i][0], mat.m[i][1], mat.m[i][2]));
	}

	// スケール加算
	VECTOR newScale = VAdd(scale, { currentScale[0], currentScale[1], currentScale[2] });

	// 最大値の制限
	if (newScale.x > MAX_MUSCLE.x) newScale.x = MAX_MUSCLE.x;
	if (newScale.y > MAX_MUSCLE.y) newScale.y = MAX_MUSCLE.y;
	if (newScale.z > MAX_MUSCLE.z) newScale.z = MAX_MUSCLE.z;

	// 最低値の制限
	if (newScale.x < MIN_MUSCLE.x) newScale.x = MIN_MUSCLE.x;
	if (newScale.y < MIN_MUSCLE.y) newScale.y = MIN_MUSCLE.y;
	if (newScale.z < MIN_MUSCLE.z) newScale.z = MIN_MUSCLE.z;

#ifdef _DEBUG
	// 筋肉量を確認する用の処理(デバッグ用)
	float avgScale = (newScale.x + newScale.y + newScale.z) / 3.0f;
	float avgMin = (MIN_MUSCLE.x + MIN_MUSCLE.y + MIN_MUSCLE.z) / 3.0f;
	float avgMax = (MAX_MUSCLE.x + MAX_MUSCLE.y + MAX_MUSCLE.z) / 3.0f;

	float muscleRatio_ = (avgScale - avgMin) / (avgMax - avgMin);
#endif // _DEBUG

	// スケール行列を作成
	MATRIX scaleMat = MGetScale(newScale);

	// 適用
	MV1SetFrameUserLocalMatrix(unit_.model_, index, scaleMat);
}

void UnitBase::DrawBar(float sX, float sY, float eX, float eY, int hp, int maxHp, COLORREF color, COLORREF frameColor, COLORREF backColor, float frameSize)
{
	DrawBoxAA(sX, sY, eX, eY, frameColor, true);

	sX += frameSize;
	sY += frameSize;
	eX -= frameSize;
	eY -= frameSize;

	DrawBoxAA(sX, sY, eX, eY, backColor, true);

	float oneSize = ((float)(eX - sX) / (float)maxHp);

	DrawBoxAA(sX, sY, sX + (oneSize * hp), eY, color, true);
}
