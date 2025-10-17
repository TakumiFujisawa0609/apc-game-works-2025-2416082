#include "FlashEffectManager.h"

FlashEffectManager::FlashEffectManager()
{
}

FlashEffectManager::~FlashEffectManager()
{
}

void FlashEffectManager::Add(const VECTOR start, const VECTOR end)
{
	Flash flash;
	flash.start_ = start;
	flash.end_ = end;
	flash.life_ = 0.0f;
	flash.maxLife_ = 0.2f;

	flashes_.push_back(flash);
}

void FlashEffectManager::Update(void)
{
	for (auto& f : flashes_) {
		f.life_ += 1.0f / 60.0f;
	}

	flashes_.remove_if([](const Flash& f) {return f.life_ >= f.maxLife_; });
}

void FlashEffectManager::Draw(void)
{
	SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

	for (auto& f : flashes_) {
		float alpha = 1.0f - (f.life_ / f.maxLife_);
		int color = GetColor(255, 100 + int(155 * alpha), 100);
		DrawLine3D(f.start_, f.end_, color);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
