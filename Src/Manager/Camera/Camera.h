#pragma once
#include <DxLib.h>

class Camera
{
public:

	enum MODE
	{
		PLAYER_FOLLOW,
	};

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new Camera(); instance_->Init(); } }
	static Camera& GetInstance(void) { return *instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

   void Init();
    void Update(const VECTOR& playerPos, const VECTOR& playerAngle);
    void Apply();


private:

	// 静的インスタンス
	static Camera* instance_;

	VECTOR camPos_;
	VECTOR camTarget_;

	float distance_ = 10.0f;   // プレイヤーとの距離
	float height_ = 5.0f;    // 見下ろす高さ

};