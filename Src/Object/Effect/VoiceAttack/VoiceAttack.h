#pragma once
#pragma once
#include <vector>
#include <DxLib.h>

class VoiceAttack
{
public:

    VoiceAttack();
    ~VoiceAttack();

    void Load(int texHandle);
    void Update(bool isVoice,
        float voicePower,
        const VECTOR& mouthPos,
        const VECTOR& forward);

    void Draw();

private:

    struct VoiceWave
    {
        VECTOR pos;
        VECTOR dir;

        float speed;
        float radius;
        float maxRadius;

        float life;
        float maxLife;

        bool isAlive;
    };

    void Spawn(const VECTOR& mouthPos,
        const VECTOR& forward,
        float voicePower);

    void UpdateWaves();

private:

    std::vector<VoiceWave> waves_;

    int texHandle_;

    float spawnTimer_;
    float spawnInterval_;   // 生成間隔（フレーム）

    float energy_;          // 声エネルギー
    float maxEnergy_;
};