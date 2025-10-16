#include "UnitBase.h"

#include <DxLib.h>

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

	// モデル描画
	MV1DrawModel(unit_.model_);
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

void UnitBase::ChangeState(int state)
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