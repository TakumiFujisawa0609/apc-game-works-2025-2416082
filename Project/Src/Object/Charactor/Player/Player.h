#pragma once

#include"../CharactorBase.h"
class AnimationController;

class MicInput;

class Player : public CharactorBase
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


	static constexpr VECTOR CAPSULE_COLLIDER_START_POS = { 0.0f,166.573f,0.0f };
	static constexpr VECTOR CAPSULE_COLLIDER_END_POS = { 0.0f,-166.573f,0.0f };

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 200.0f, -500.0f };	// 初期座標

	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };			// モデルの向き修正用

	static constexpr VECTOR LOCAL_POS = { 0.0f, 166.573, 0.0f };		// 描画と座標のずれを直すためのローカル座標

	static constexpr float RADIUS_SIZE = 60.0f;				//プレイヤーの半径（仮）

	static constexpr float MOVE_SPEED = 16.0f;				// 移動速度

	static constexpr float ROLL_SPEED = MOVE_SPEED * 2;		// 回避速度
	static constexpr int ROLLING_TIME = 30;					// 回避時間
	static constexpr int NEXT_ROLL_TIME = 60;				// 回避行動のクールタイム

	// 移動範囲制限（外側）
    static constexpr float STAGE_COLLISION_RADIUS_OUTSIDE = 2300.0f;	// 最大半径
	static constexpr float STAGE_COLLISION_RADIUS_INSIDE = 450.0f;		// 最大半径

	// 無敵時間
	const int INVI_TIME = 40;

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

	static constexpr float DOWN_MUSCLE = -0.005f;	//常時筋肉が減るため、減らし続ける用の値



	// モデルアニメーション用----------------------------------
	struct AnimInfo
	{
		const char* name;
		float speed;
	};

	// アニメーションのパスと再生速度の設定
	const std::vector<AnimInfo> ANIMATION_INFO = {
		{ "Idle1", 0.5f },
		{ "Run", 0.8f },
		{ "Punching", 1.5f },
		{ "Punching2", 1.5f },
		{ "Swiping", 2.0f },
		{ "Evasion", 1.5f },
		{ "Death", 0.5f },
	};
	// --------------------------------------------------------
#pragma endregion

	Player();
	~Player() override = default;

	void Load(void)override;
	void UiDraw(void)override;				// UI描画

	// 当たり判定の通知
	void OnCollision(const ColliderBase& collider);

	// 接地判定の通知
	void OnGrounded(void)override;

	void CameraPosUpdate(void);						// カメラ座標に関する処理

	// カメラのローカル座標のゲット関数
	const Vector3 &GetCameraLocalPos(void) { return cameraPos_; }

	/// <summary>
	/// 筋肉量割合のゲット関数
	/// </summary>
	/// <param name="index">モデルのフレームの配列</param>
	/// <returns>筋肉の割合(Ratio)が返ってくる</returns>
	const float GetMuscleRatio(int index);

	void SetDamage(int damage);

	int GetVoiceLevel(void) const;

private:
	void CharactorInit(void)override;
	void CharactorUpdate(void)override;
	void CharactorDraw(void)override;
	void CharactorAlphaDraw(void)override;
	void CharactorRelease(void)override;

	// プレイヤー情報の初期化
	void ParamInit(void);

	// 回避用カウンタの更新処理
	void RollCountUpdate(void);

	// 入力を見て移動方向を決める
	void SetMoveVec(Vector3& move);

	// HPの描画処理
	void HpDraw(void);

	// ステージとの疑似当たり判定をここでしている
	void StageCollision(void);

private:

	// インスタンス----------------------------------------
	MicInput* mic_;						// マイクインプットクラス
	// -----------------------------------------------------
	
#pragma region 列挙型定義
	// 現在のコンボ
	CONBO conbo_;
#pragma endregion

	// カメラローカル座標
	static constexpr float CAMERA_PLAYER_POS = 300.0f;

	static constexpr int RIGHT_HAND_INDEX = 38;
	static constexpr int LEFT_HAND_INDEX = 13;

#pragma region 変数

	// ヒットポイント
	unsigned char hp_;

	// 最終移動Y軸角度
	float moveAngleY_;

	Vector3 leftKnuckleLocalPos_;
	Vector3 rightKnuckleLocalPos_;

	Vector3 cameraPos_;

	// 攻撃したかどうかの確認用
	bool isAttacked_[(int)CONBO::MAX];

	// 次の段につながるまでの時間(フレーム数)
	const unsigned char INPUT_ATTACK_FRAME = 60;
	unsigned char attackCounter_;
	void AttackCounterUpdate(void);

	// 回避のクールタイムカウンタ
	int nextRollCounter_;
	
	int rollCounter_;

	// 攻撃から抜け出さないときがあるので、
	// 攻撃の状態が一定時間たったら抜けるようにする用カウンタ
	int attackEscapeCounter_;

	// 筋肉に伴い体が大きくなるためカメラの位置を変える用の変数
	float currentHeight;

	// 叫ぶときのエフェクトのハンドル用変数
	int effectSoutId_;

	int effectSoutPlayid_;

#pragma endregion

#pragma region 筋肉関係
	const Vector3 MAX_MUSCLE = { 2.0f,2.0f,2.0f };	// 筋肉のスケールの最大値
	const Vector3 MIN_MUSCLE = { 1.0f,1.0f,1.0f };	// 筋肉のスケールの最低値

	// 音声を受け取って音量に応じて筋肉を増やす処理
	void VoiceUpMuscle(void);

	/// <summary>
	/// 指定されたボーンのスケールの加算
	/// </summary>
	/// <param name="index">ボーンの配列番号</param>
	/// <param name="scale">加算するスケール値</param>
	void AddBoneScale(int index, float scale);
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

