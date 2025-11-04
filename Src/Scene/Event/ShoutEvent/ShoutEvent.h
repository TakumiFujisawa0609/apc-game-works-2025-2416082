#pragma once
#include "../../SceneBase.h"

class ShoutEvent : public SceneBase
{
public:

	static constexpr int SHOUT_VOICE_MAX = 4000;

    ShoutEvent(const int& micLevel);
    ~ShoutEvent() override;

    void Load() override;
    void Init() override;
    void Update() override;
    void Draw() override;
    void Release() override;

	const bool EventEnd(void) { return end_; }

private:
	bool end_;
    const int& micLv_;
};