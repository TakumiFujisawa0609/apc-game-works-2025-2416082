#pragma once
#include "../../../UnitBase.h"

class RotateHand : public UnitBase
{
public:
    static constexpr VECTOR SCALE = { 2.0f,2.0f,2.0f };

    RotateHand(int model, const VECTOR& bossPos);
    ~RotateHand() override;

    void SubLoad() override;
    void SubInit() override;
    void SubUpdate() override;
    void SubDraw() override;
    void SubRelease() override;

    void OnCollision(UnitBase* other) override;

    bool IsEnd(void) { return end_; }

private:
    VECTOR center_;     // ボス中心
    float radius_;      // 回転半径
    float angle_;       // 現在の角度（ラジアン）
    float speed_;       // 回転速度

    bool end_;

};