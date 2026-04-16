#pragma once
#include "../UnitBase.h"

#include <algorithm>
#include <vector>
#include <DxLib.h>

class AnimationController;

class MicInput;

class LeftArm;
class RightArm;

class Player : public UnitBase
{
private:

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

	enum class UI_IMAGE
	{
		HP_FRAME,
		HP_BAR,
		MAX
	};
#pragma region 定数定義

	static constexpr int HP_MAX = 100;	// プレイヤーの最大HP

	static constexpr int HP_DAMAGE = 10;	// プレイヤーが攻撃を受けたときのダメージ量

	static constexpr float CAPSULE_HALF_LENGTH = 100;								// カプセルの真ん中から外側（円の中心）までの長さ
	static constexpr VECTOR DEFAULT_POS = { 0.0f, CAPSULE_HALF_LENGTH, -1000.0f };	// 初期座標
	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };			// モデルの向き修正用
	static constexpr VECTOR LOCAL_POS = { 0.0f, CAPSULE_HALF_LENGTH, 0.0f };		// 描画と座標のずれを直すためのローカル座標

	static constexpr float RADIUS_SIZE = 60.0f;				//プレイヤーの半径（仮）
	static constexpr float MOVE_SPEED = 16.0f;				// 移動速度

	static constexpr float ROLL_SPEED = MOVE_SPEED * 2;		// 回避速度
	static constexpr int ROLLING_TIME = 30;					// 回避時間
	static constexpr int NEXT_ROLL_TIME = 60;				// 回避行動のクールタイム

	static constexpr float MOVE_ANGLE_INTERPOLATION = 0.3f;		// 移動するときの向き補間率

	// 移動範囲制限（外側）
	static constexpr float STAGE_COLL_RAD_OUTSIDE = 2050.0f;  // 最大半径
	static constexpr float STAGE_COLL_RAD_INSIDE = 450.0f;  // 最大半径

	static constexpr int PLAYER_ICON_IMAGE_MAX = 2;

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

public:


	Player();
	~Player() override;

	void UIDraw(void);				// UI描画

	void OnCollision(UnitBase* other) override;		// 当たり判定処理

	void CameraPosUpdate(void);						// カメラ座標に関する処理

	// カメラのローカル座標のゲット関数
	const VECTOR& GetCameraLocalPos(void) { return cameraPos_; }

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


private:

	void SubLoad(void) override;		// 最初に呼び出す関数
	void SubInit(void) override;		// 初期化処理
	void SubUpdate(void) override;		// 更新処理
	void SubDraw(void) override;		// 描画処理
	void SubRelease(void) override;		// 解放処理

private:

	// プレイヤーパラメーターの初期化
	void ParamInit(void);

	// 回避用カウンタの更新処理
	void RollCountUpdate(void);

	// 入力を見て移動方向を決める
	void SetMoveVec(void);

	// モデルに行列の適用処理
	void SetMatrix(void);

	// 描画関係関数================
	// デバッグ関係の描画用関数
	void DebugDraw(void);

	// HPの描画処理
	void HpDraw(void);
	// ============================

	// ステージとの疑似当たり判定をここでしている
	void StageCollision(void);

	void KnockBack(const VECTOR& attackPos, float power = 20.0f);
	void KnocBackUpdate(void);
	VECTOR knockBackVel_;
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

	// HPの隣に置くプレイヤーのアイコン
	int playerIconImageId_[PLAYER_ICON_IMAGE_MAX];
	int playerHpImageId_[(int)UI_IMAGE::MAX];

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

	// 状態遷移用の関数
	void StateManager(void);

	void DoMove(void);		// 移動に遷移するための処理
	void DoIdle(void);		// 何もしていなかったらIdleに遷移
	void DoAttack(void);	// 攻撃に遷移するための処理
	void DoRoll(void);		// 回避するための処理
#pragma endregion

	void HpBarDraw(float currentHp, float maxHp, const VECTOR& pos1, const VECTOR& pos2)
	{
		// static 変数で前回の表示HPを保持（関数を呼ぶたびに滑らかに変化）
		static float displayHp = maxHp;

		// HP割合
		currentHp = std::clamp(currentHp, 0.0f, maxHp);
		float rate = displayHp / maxHp;

		// 徐々に追従
		const float speed = 0.1f;
		displayHp += (currentHp - displayHp) * speed;

		// 背景バー
		DrawBox((int)pos1.x, (int)pos1.y, (int)pos2.x, (int)pos2.y, GetColor(80, 80, 80), TRUE);

		// 現在のバー
		int barWidth = static_cast<int>((pos2.x - pos1.x) * rate);
		DrawBox((int)pos1.x, (int)pos1.y, (int)pos1.x + barWidth, (int)pos2.y, GetColor(255, 50, 50), TRUE);

		// 枠線
		DrawBox((int)pos1.x, (int)pos1.y, (int)pos2.x, (int)pos2.y, GetColor(255, 255, 255), FALSE);
	}


};

