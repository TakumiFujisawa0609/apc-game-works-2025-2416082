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

	void Init();	 // 初期化
	void Update();	 // 更新処理
	void Release();	 // 解放

	void SetMode(MODE mode) { mode_ = mode; }


private:

	// 静的インスタンス
	static Camera* instance_;

	MODE mode_;

	VECTOR pos_;

	VECTOR targetPos_;

	VECTOR angles_;

};