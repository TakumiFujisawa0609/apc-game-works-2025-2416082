#pragma once
#include <DxLib.h>

#include<functional>
#include<map>

class Camera
{
public:

	static constexpr float CAMERA_PLAYER_POS = 300.0f;

	static constexpr VECTOR LOCAL_POS = { 0.0f, 400.0f, -800.0f };

	enum MODE
	{
		PLAYER_FOLLOW,
		BOSS_DEATH,
	};


	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new Camera(); instance_->Init(); } }
	static Camera& GetInstance(void) { return *instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

   void Init();
    void Update();
    void Apply();

	void SetTarget(const VECTOR* player, const VECTOR* boss) { targetPlayerPos_ = player; targetBossPos_ = boss; }
	VECTOR GetAngle(void) { return angle_; }

	void SetBossDeathCamera(void) { mode_ = MODE::BOSS_DEATH; }

private:

	// 静的インスタンス
	static Camera* instance_;

	MODE mode_;

	using ModeFunc = void(Camera::*)();
	std::map<MODE, ModeFunc> modeFuncs_;

	VECTOR camPos_;
	VECTOR angle_;

	int mouseX, mouseY;

	const VECTOR* targetPlayerPos_;
	const VECTOR* targetBossPos_;

	VECTOR bossPos_;     // ボスの位置
	float  deathTimer_;  // 演出用タイマー
	float  radius_;      // カメラ距離
	float  height_;      // 高さ

	void MouseMoveCamera(void);
	void PadMoveCamera();

	void PlayerFollowCamera(void);
	void BossDeathCamera(void);
};