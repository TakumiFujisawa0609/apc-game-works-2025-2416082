#pragma once

#include<map>

class AnimationController
{
public:

	// アニメーションデータ
	struct Animation
	{
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};
	// コンストラクタ
	AnimationController(int modelId);
	// デストラクタ
	~AnimationController(void);
	// 同じFBX内のアニメーションを準備
	void AddInFbx(int type, float speed, int animIndex);
	// アニメーション再生
	void Play(int type, bool isLoop8);
	void Update(void);
	void Release(void);
	bool IsEnd(int type) const;
private:
	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	// 再生中のアニメーション
	int playType_;

	bool isLoop_;

	Animation playAnim_;
};