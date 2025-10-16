#pragma once
#include "../UnitBase.h"

#include <vector>
#include <DxLib.h>

class AnimationController;

class LeftArm;
class RightArm;

class Player : public UnitBase
{
public:

	// プレイヤーのステート管理
	enum class STATE
	{
		IDLE,
		MOVE,
		ATTACK,
		ROLL,
		DEATH,

		MAX
	};

	// アニメーション管理用
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ROLL,
		DEATH,
	};

	// コンボ管理用
	enum class CONBO
	{
		CONBO1,
		CONBO2,
		CONBO3,

		MAX,
	};

#pragma region 定数定義

	static constexpr float CAPSULE_HALF_LENGTH = 100;								// カプセルの真ん中から外側（円の中心）までの長さ

	static constexpr VECTOR DEFAULT_POS = { 0.0f, CAPSULE_HALF_LENGTH, -100.0f };	//初期座標

	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };			//モデルの向き修正用

	static constexpr VECTOR CENTER_DIFF = { 0.0f, CAPSULE_HALF_LENGTH, 0.0f };

	static constexpr float RADIUS_SIZE = 60.0f;				//プレイヤーの半径（仮）

	static constexpr float MOVE_SPEED = 16.0f;				// 移動速度

	static constexpr float ROLL_SPEED = MOVE_SPEED * 2;		// 回避速度
	static constexpr int ROLLING_TIME = 30;					// 回避時間
	static constexpr int NEXT_ROLL_TIME = 60;				// 回避行動のクールタイム



	// コンボの段階に応じて攻撃したときの移動量
	static constexpr float CONBO_MOVE_SPEED[(int)CONBO::MAX] =
	{
		5.0f,
		10.0f,
		5.0f
	};



	// 攻撃時に筋肉を増やすときのコンボ段階に応じたスケールの増量
	static constexpr VECTOR UP_MUSCLE[(int)CONBO::MAX] =
	{
		{ 0.01f, 0.01f, 0.01f },
		{ 0.02f, 0.02f, 0.02f },
		{ 0.03f, 0.03f, 0.03f }
	}; 

	static constexpr VECTOR DOWN_MUSCLE = { -0.001f,-0.001f,-0.001f };	//常時筋肉が減るため、減らし続ける用の値
#pragma endregion



	Player();
	~Player() override;

	void SubLoad(void) override;		// 最初に呼び出す関数
	void SubInit(void) override;		// 初期化処理
	void SubUpdate(void) override;		// 更新処理
	void SubDraw(void) override;		// 描画処理
	void SubRelease(void) override;	// 解放処理

	void UIDraw(void);				// UI描画

	void OnCollision(UnitBase* other) override;		// 当たり判定処理

	void CameraPosUpdate(void);						// カメラ座標に関する処理

	const VECTOR &GetCameraLocalPos(void) { return cameraPos_; }
	const VECTOR &GetAngle(void) { return unit_.angle_; }
	const float GetMuscleRatio();
	const STATE GetState(void) { return state_; }


	LeftArm* GetLeftArm(void) { return leftArm_; }
	RightArm* GetRightArm(void) { return rightArm_; }

private:

	AnimationController* animation_;

	LeftArm* leftArm_;
	RightArm* rightArm_;;
	
	void DebugDraw(void);
	void SetMatrix(void);
	void HpDraw(void);

#pragma region 列挙型定義
	// ステート管理用
	STATE state_;

	CONBO conbo_;

#pragma endregion

#pragma region 変数

	//　移動用
	VECTOR move_;
	VECTOR cameraPos_;

	// 攻撃したかどうかの確認用
	bool isAttacked_;

	// 筋肉を増やす
	bool isUpMuscle_;

	// 回避用カウンタ
	int nextRollCounter_;

	// 攻撃コンボ用
	int attacConboCnt_;

	// 筋肉に伴い体が大きくなるためカメラの位置を変える用の変数
	float currentHeight;

	//float muscleRatio_;

#pragma endregion

#pragma region 筋肉関係
	void Muscle(void);
	//void AddBoneScale(int index, VECTOR scale);
#pragma endregion

#pragma region ステート管理関係


	void Idle(void);
	void Move(void);
	void Attack(void);
	void Roll(void);
	void Death(void);
#pragma endregion


#pragma region 状態遷移関係


	// 状態遷移用の関数
	void StateManager(void);
	void DoWalk(void);
	void DoIdle(void);
	void DoAttack(void);
	void DoRoll(void);

#pragma endregion



#pragma region UI関係
	//void DrawRingGauge(int cx, int cy, int outerR, int innerR, float ratio, int color);

	//int CalcGaugeColor(float ratio) const;
	//float CalcEffectRatio(float ratio, int time) const;
	//void DrawGaugeBack(int centerX, int centerY, float radius)const;
	//void DrawGaugeRing(int centerX, int centerY, int innerR, int outerR, float ratio, int color) const;
	//void DrawGaugeFrame(int centerX, int centerY, int innerR, int outerR) const;
	//void DrawGlowEffect(int cx, int cy, float radius, float& ringThickness) const;
	//void DrawGaugeText(int cx, int cy, float ratio, float pulse) const;
	//void DrawMuscleGauge(int cx, int cy, int outerR, int innerR, float ratio);
#pragma endregion
};

