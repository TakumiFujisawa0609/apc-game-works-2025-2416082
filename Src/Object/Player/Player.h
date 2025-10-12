#pragma once
#include "../UnitBase.h"

#include <map>
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
		ROLL
	};

	// アニメーション管理用
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		Roll
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

	static constexpr float CAPSULE_HALF_LENGTH = 100;

	static constexpr VECTOR DEFAULT_POS = { 0.0f, CAPSULE_HALF_LENGTH, -100.0f }; //初期座標

	static constexpr VECTOR CENTER_DIFF = { 0.0f, CAPSULE_HALF_LENGTH, 0.0f };

	static constexpr float RADIUS_SIZE = 60.0f;				//プレイヤーの半径（仮）

	static constexpr float MOVE_SPEED = 16.0f;	// 移動速度

	static constexpr float ROLL_SPEED = MOVE_SPEED * 2;		// 回避速度
	static constexpr int ROLLING_TIME = 30;		// 回避時間
	static constexpr int NEXT_ROLL_TIME = 60;	// 回避行動のクールタイム

	static constexpr float CONBO_MOVE_SPEED[(int)CONBO::MAX] =
	{
		20.0f,
		25.0f,
		30.0f
	};

	static constexpr VECTOR MAX_MUSCLE = { 4.0f,4.0f,4.0f };
	static constexpr VECTOR MIN_MUSCLE = { 1.0f,1.0f,1.0f };

	static constexpr VECTOR UP_MUSCLE[(int)CONBO::MAX] =
	{
		{ 0.01f, 0.01f, 0.01f },
		{ 0.02f, 0.02f, 0.02f },
		{ 0.03f, 0.03f, 0.03f }
	}; 

	static constexpr VECTOR DOWN_MUSCLE = { -0.0005f,-0.0005f,-0.0005f };
#pragma endregion



	Player();
	~Player() override;

	void Load(void) override;
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void OnCollision(UnitBase* other) override;

	void CameraPosUpdate(void);

	const VECTOR &GetCameraLocalPos(void) { return cameraPos_; }
	const VECTOR &GetAngle(void) { return unit_.angle_; }

	LeftArm* GetLeftArm(void) { return leftArm_; }
	RightArm* GetRightArm(void) { return rightArm_; }

private:

	AnimationController* animation_;

	LeftArm* leftArm_;
	RightArm* rightArm_;;
	
	void DebugDraw(void);
	void DrawPlayer(void);

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

#pragma endregion

#pragma region 筋肉関係
	void Muscle(void);
	void MuscleDraw(void);
	void AddBoneScale(int index, VECTOR scale);
	void AddArmScale(VECTOR scale);
#pragma endregion

#pragma region ステート管理関係
	// 関数ポインタ
	using StateFunc = void(Player::*)();
	std::map<STATE, StateFunc> stateFuncs_;

	void Idle(void);
	void Move(void);
	void Attack(void);
	void Roll(void);
#pragma endregion


#pragma region 状態遷移関係

	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };

	// 状態遷移用の関数
	void StateManager(void);
	void DoWalk(void);
	void DoIdle(void);
	void DoAttack(void);
	void DoRoll(void);

#pragma endregion

	// デバッグ用変数
	int frameScrollIndex_;
	float muscleRatio_;
};