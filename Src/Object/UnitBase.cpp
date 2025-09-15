#include "UnitBase.h"

#include <DxLib.h>

#include"../Scene/SceneManager/SceneManager.h"

UnitBase::UnitBase() {}
UnitBase::~UnitBase() {}

void UnitBase::Invi(void)
{
	if (unit_.inviciCounter_ > 0) { unit_.inviciCounter_--; }
}