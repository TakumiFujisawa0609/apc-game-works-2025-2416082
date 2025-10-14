#pragma once
#include <DxLib.h>

class Camera
{
public:

	static constexpr float CAMERA_PLAYER_POS = 200.0f;

	enum MODE
	{
		PLAYER_FOLLOW,
	};

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new Camera(); instance_->Init(); } }
	static Camera& GetInstance(void) { return *instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

   void Init();
    void Update();
    void Apply();

	void SetTarget(const VECTOR* target) { camTarget_ = target; }
	VECTOR GetAngle(void) { return angle_; }


private:

	// 静的インスタンス
	static Camera* instance_;

	VECTOR camPos_;
	VECTOR angle_;

	int mouseX, mouseY;

	const VECTOR* camTarget_;

	const VECTOR LOCAL_POS = { 0.0f, 300.0f, -500.0f };

	void MouseMoveCamera(void);
	void PadMoveCamera();
};