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
    flash.start_ = start;          // 始点
    flash.end_ = end;              // 終点
    flash.aliveTime_ = 0.0f;       // 経過時間をリセット
    flash.maxAliveTime_ = 0.2f;    // 寿命（0.2秒）

    // リストに登録
    flashes_.push_back(flash);
}

void FlashEffectManager::Update(void)
{
    for (auto& f : flashes_) {
        f.aliveTime_ += 1.0f / 60.0f;   // 1フレーム分進行（60fps想定）
    }

    // 寿命を超えたエフェクトを消すよん
    flashes_.remove_if([](const Flash& f) {
        return f.aliveTime_ >= f.maxAliveTime_;
    });
}

void FlashEffectManager::Draw(void)
{
    // 光ってる感じにする
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

    for (auto& f : flashes_) {
        float t = f.aliveTime_ / f.maxAliveTime_;

        // 時間経過に応じて透明度を減少
        float alpha = 1.0f - t;

        // 徐々に赤みが増すような色
        int color = GetColor(255, 100 + int(155 * alpha), 100);

        // 3D空間上に線を描画
        DrawLine3D(f.start_, f.end_, color);
    }

    // 描画モードを通常に戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}