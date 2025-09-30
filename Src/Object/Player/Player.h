#pragma once
#include "../UnitBase.h"
#include <map>
#include <DxLib.h>

class AnimationController;

class Player : public UnitBase
{
public:
#pragma region 定数定義
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, -100.0f }; //初期座標
	static constexpr float RADIUS_SIZE = 100.0f;				//プレイヤーの半径（仮）


	static constexpr float MOVE_SPEED = 16.0f;	// 移動速度

	static constexpr float ROLL_SPEED = MOVE_SPEED * 2;		// 回避速度
	static constexpr int ROLLING_TIME = 30;		// 回避時間
	static constexpr int NEXT_ROLL_TIME = 60;	// 回避行動のクールタイム

	//両腕のインデックス
	static constexpr int LEFT_ARM = 12;   // 左腕
	static constexpr int RIGHT_ARM = 36;   // 右腕
#pragma endregion


	enum class STATE
	{
		IDLE,
		MOVE,
		ATTACK,
		ROLL
	};

	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		ATTACK,
		Roll
	};

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
	const VECTOR GetAngle(void) { return unit_.angle_; }

private:

	AnimationController* animation_;

	MATRIX MatrixSet(void);

#pragma region 列挙型定義
	// ステート管理用
	STATE state_;
	// アニメション管理用
	ANIM_TYPE animType_;
#pragma endregion

#pragma region 変数
	//　移動用
	VECTOR move_;

	// 攻撃したかどうかの確認用
	bool attackScaleApplied_ = false;

	// 回避用カウンタ
	int nextRollCounter_;

	// 筋肉に伴い体が大きくなるためカメラの位置を変える用の変数
	float currentHeight;


	VECTOR cameraPos_;
#pragma endregion

#pragma region 筋肉関係
	void Muscle(void);
	void BoneScale(int index, VECTOR scale);
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

	int frameScrollIndex_ = 0;
};