#pragma once
#include <DxLib.h>

#include <list>

class FlashEffectManager
{
public:

	struct Flash
	{
		VECTOR start_;
		VECTOR end_;
		float aliveTime_;
		float maxAliveTime_;

	};

	FlashEffectManager();
	~FlashEffectManager();
	
	void Add(const VECTOR start, const VECTOR end);

	void Update(void);
	void Draw(void);

private:

	std::list<Flash> flashes_;
};