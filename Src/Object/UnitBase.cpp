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

void UnitBase::HpBarDraw(float currentHp, float maxHp, VECTOR pos1, VECTOR pos2, COLOR16 color)
{
	// static 変数で前回の表示HPを保持（関数を呼ぶたびに滑らかに変化）
	float displayHp = maxHp;

	// HP割合
	currentHp = std::clamp(currentHp, 0.0f, maxHp);
	float rate = displayHp / maxHp;

	// 徐々に追従
	const float speed = 0.1f;
	displayHp += (currentHp - displayHp) * speed;

	// 枠線
	DrawBox(pos1.x - 5, pos1.y - 5, pos2.x + 5, pos2.y + 5, 0xffffff, true);

	// 背景バー
	DrawBox(pos1.x, pos1.y, pos2.x, pos2.y, 0x000000, true);

	// 現在のバー
	int barWidth = static_cast<int>((pos2.x - pos1.x) * rate);
	DrawBox(pos1.x, pos1.y, pos1.x + barWidth, pos2.y, color, true);
}
