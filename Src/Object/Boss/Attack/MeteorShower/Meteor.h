#pragma once

#include "../../Attack/AttackBase.h"

class Meteor : public AttackBase
{
public:
    Meteor(const VECTOR& startPos, const VECTOR& targetPos, float speed);
    ~Meteor();

    void DefaultUpdate() override;
    void DefaultDraw() override;
    void OnCollision(UnitBase* other) override;

private:
    VECTOR direction_;
    float speed_;
};
