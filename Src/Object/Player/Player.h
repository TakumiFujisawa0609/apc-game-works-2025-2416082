#pragma once
#include "../UnitBase.h"

#include <vector>
#include <DxLib.h>

class AnimationController;

class MicInput;

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

		MAX
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

	static constexpr int HP_MAX = 100;

	static constexpr float CAPSULE_HALF_LENGTH = 100;								// カプセルの真ん中から外側（円の中心）までの長さ

	static constexpr VECTOR DEFAULT_POS = { 0.0f, CAPSULE_HALF_LENGTH, -500.0f };	// 初期座標

	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };			// モデルの向き修正用

	static constexpr VECTOR LOCAL_POS = { 0.0f, CAPSULE_HALF_LENGTH, 0.0f };		// 描画と座標のずれを直すためのローカル座標

	static constexpr float RADIUS_SIZE = 60.0f;				//プレイヤーの半径（仮）

	static constexpr float MOVE_SPEED = 16.0f;				// 移動速度

	static constexpr float ROLL_SPEED = MOVE_SPEED * 2;		// 回避速度
	static constexpr int ROLLING_TIME = 30;					// 回避時間
	static constexpr int NEXT_ROLL_TIME = 60;				// 回避行動のクールタイム

	    // 移動範囲制限（外側）
    static constexpr float STAGE_COLLISION_RADIUS_OUTSIDE = 4300.0f;  // 最大半径
	static constexpr float STAGE_COLLISION_RADIUS_INSIDE = 450.0f;  // 最大半径



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

	static constexpr VECTOR DOWN_MUSCLE = { -0.005f,-0.005f,-0.005f };	//常時筋肉が減るため、減らし続ける用の値



	// モデルアニメーション用----------------------------------
	struct AnimInfo
	{
		const char* name;
		float speed;
	};

	// アニメーションのパスと再生速度の設定
	const std::vector<AnimInfo> ANIMATION_INFO = {
		{ "Idle1", 30.0f },
		{ "Run", 50.0f },
		{ "Punching", 100.0f },
		{ "Punching2", 100.0f },
		{ "Swiping", 130.0f },
		{ "Evasion", 100.0f },
		{ "Death", 30.0f },
	};
	// --------------------------------------------------------
#pragma endregion

	Player();
	~Player() override;

	void UIDraw(void);				// UI描画

	void OnCollision(UnitBase* other) override;		// 当たり判定処理

	void CameraPosUpdate(void);						// カメラ座標に関する処理

	// カメラのローカル座標のゲット関数
	const VECTOR &GetCameraLocalPos(void) { return cameraPos_; }

	/// <summary>
	/// 筋肉量割合のゲット関数
	/// </summary>
	/// <param name="index">モデルのフレームの配列</param>
	/// <returns>筋肉の割合(Ratio)が返ってくる</returns>
	const float GetMuscleRatio(int index);

	// プレイヤーのステートのゲット関数
	const STATE GetState(void) { return state_; }

	void SetDamage(int damage);

	int GetVoiceLevel(void) const;

	// 腕クラスのインスタンスのゲット関数
	LeftArm* GetLeftArm(void) { return leftArm_; }
	RightArm* GetRightArm(void) { return rightArm_; }

protected:
	void SubLoad(void) override;		// 最初に呼び出す関数
	void SubInit(void) override;		// 初期化処理
	void SubUpdate(void) override;		// 更新処理
	void SubDraw(void) override;		// 描画処理
	void SubRelease(void) override;	// 解放処理
private:

	// プレイヤー情報の初期化
	void ParamInit(void);

	// 回避用カウンタの更新処理
	void RollCountUpdate(void);

	// 入力を見て移動方向を決める
	void SetMoveVec(void);

	// デバッグ関係の描画用関数
	void DebugDraw(void);

	// モデルに行列の適用処理
	void SetMatrix(void);

	// HPの描画処理
	void HpDraw(void);

	// ステージとの疑似当たり判定をここでしている
	void StageCollision(void);

	// 筋肉ゲージの初期化
	void MuscleGaugeDraw(void);
private:

	// インスタンス----------------------------------------
	AnimationController* animation_;	// アニメーションクラス
	MicInput* mic_;						// マイクインプットクラス
	LeftArm* leftArm_;					// 左腕クラス
	RightArm* rightArm_;				// 右腕クラス
	// -----------------------------------------------------
	
#pragma region 列挙型定義
	// ステート管理用
	STATE state_;

	// 現在のコンボ
	CONBO conbo_;
#pragma endregion

#pragma region 変数

	//　移動用
	VECTOR move_;
	VECTOR cameraPos_;

	// 攻撃したかどうかの確認用
	bool isAttacked_;

	// 回避のクールタイムカウンタ
	int nextRollCounter_;

	int rollCounter_;

	// 攻撃から抜け出さないときがあるので、
	// 攻撃の状態が一定時間たったら抜けるようにする用カウンタ
	int attackEscapeCounter_;

	// 筋肉に伴い体が大きくなるためカメラの位置を変える用の変数
	float currentHeight;

#pragma endregion

#pragma region 筋肉関係
	// 音声を受け取って音量に応じて筋肉を増やす処理
	void VoiceUpMuscle(void);
#pragma endregion

#pragma region ステート管理関係
	void Idle(void);	// 何もしていない
	void Move(void);	// 動いている(歩く)
	void Attack(void);	// 攻撃中
	void Roll(void);	// 回避中
	void Death(void);	// 死にました
#pragma endregion


#pragma region 状態遷移関係
	// 状態遷移用の関数
	void StateManager(void);

	void DoMove(void);		// 移動に遷移するための処理
	void DoIdle(void);		// 何もしていなかったらIdleに遷移
	void DoAttack(void);	// 攻撃に遷移するための処理
	void DoRoll(void);		// 回避するための処理
#pragma endregion

};

