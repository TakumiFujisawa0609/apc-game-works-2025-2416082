#include "ShoutEvent.h"

ShoutEvent::ShoutEvent(const int& micLevel) :
	end_(false),
	micLv_(micLevel)
{
}

ShoutEvent::~ShoutEvent()
{
}

void ShoutEvent::Load()
{
}

void ShoutEvent::Init()
{
	end_ = false;
}

void ShoutEvent::Update()
{
}

void ShoutEvent::Draw()
{
}

void ShoutEvent::Release()
{
}
