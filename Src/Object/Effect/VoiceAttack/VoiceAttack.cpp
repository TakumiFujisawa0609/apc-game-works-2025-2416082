#include "VoiceAttack.h"
#include <algorithm>

VoiceAttack::VoiceAttack()
{
}

VoiceAttack::~VoiceAttack()
{
}

void VoiceAttack::Load(int texHandle)
{
    texHandle_ = texHandle;

    spawnTimer_ = 0.0f;
    spawnInterval_ = 6.0f;   // 6フレームごと

    maxEnergy_ = 100.0f;
    energy_ = maxEnergy_;
}

void VoiceAttack::Update(bool isVoice,
    float voicePower,
    const VECTOR& mouthPos,
    const VECTOR& forward)
{
    // エネルギー管理
    if (isVoice && energy_ > 0.0f)
    {
        energy_ -= 0.8f;  // 消費
        spawnTimer_++;

        if (spawnTimer_ >= spawnInterval_)
        {
            Spawn(mouthPos, forward, voicePower);
            spawnTimer_ = 0.0f;
        }
    }
    else
    {
        spawnTimer_ = 0.0f;

        // 回復
        energy_ += 0.5f;
        energy_ = min(energy_, maxEnergy_);
    }

    UpdateWaves();
}

void VoiceAttack::Spawn(const VECTOR& mouthPos,
    const VECTOR& forward,
    float voicePower)
{
    VoiceWave w;

    w.pos = mouthPos;
    w.dir = VNorm(forward);

    w.speed = 15.0f + voicePower * 0.3f;

    w.radius = 20.0f;
    w.maxRadius = 100.0f + voicePower * 1.5f;

    w.life = 0.0f;
    w.maxLife = 30.0f;

    w.isAlive = true;

    waves_.push_back(w);
}

void VoiceAttack::UpdateWaves()
{
    for (auto& w : waves_)
    {
        if (!w.isAlive) continue;

        w.life++;

        // 前進
        w.pos = VAdd(w.pos, VScale(w.dir, w.speed));

        float t = w.life / w.maxLife;

        // 拡大（イージング）
        w.radius = w.maxRadius * (1.0f - powf(1.0f - t, 2));

        if (w.life >= w.maxLife)
            w.isAlive = false;
    }
}

void VoiceAttack::Draw()
{
    for (auto& w : waves_)
    {
        if (!w.isAlive) continue;

        float t = w.life / w.maxLife;
        int alpha = (int)(255 * (1.0f - t));

        SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

        float scale = w.radius / 100.0f;

        DrawBillboard3D(
            w.pos,
            0.5f, 0.5f,
            scale,
            0.0f,
            texHandle_,
            TRUE
        );
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


//void VoiceAttack::Draw()
//{
//    SetUseZBuffer3D(TRUE);
//    SetWriteZBuffer3D(TRUE);
//
//    DrawBillboard3D(
//        VGet(0, 200, 0),     // ← ワールド中央固定
//        0.5f, 0.5f,
//        200.0f,
//        0.0f,
//        texHandle_,
//        TRUE
//    );
//}