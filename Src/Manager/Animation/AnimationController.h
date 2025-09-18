#pragma once

#include<map>
#include <string>

class AnimationController
{
public:

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
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
	void Play(int type, bool isLoop);
	void Update(void);
	void Release(void);
	bool IsEnd(int type) const;

	// 外部FBXからアニメーション追加
	void Add(int type, float speed, const std::string path);
private:
	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	// 再生中のアニメーション
	int playType_;

	bool isLoop_;

	Animation playAnim_;

	//アニメーション追加の共通処理
	void Add(int type, float speed, Animation& animation);
};