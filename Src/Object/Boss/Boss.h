#pragma once
#include "../UnitBase.h"

#include <Vector>

class AttackBase;

class Boss : public UnitBase
{
public:
#pragma region 定数定義
	const VECTOR LOCAL_ANGLE = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };

	static constexpr float RADIUS = 250.0f;				// 半径の大きさ
	static constexpr VECTOR SCALE = { 5.0f,5.0f,5.0f };	// 拡大率
	static constexpr float HALF_LEN = 300.0f;			// カプセルの上と下の円の中心同士の長さの半分の大きさ

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 600.0f, 0.0f };	// ボスの初期位置
		
	static constexpr int HP_MAX = 300;		// 最大HP

	static constexpr int INVI_TIME = 30;	// 攻撃を食らったときの無敵時間

	static constexpr int NEXT_ATTACK_TIME = 120;	// 攻撃終了後、次の攻撃までのクールタイム	

	// ステート管理用の列挙
	enum class STATE
	{
		IDLE,
		ATTACK,
		DAMAGE,
		DEATH,
	};

	// 攻撃管理用の列挙
	enum class ATTACK
	{
		SLAP,
		SHOT,
		STAR,

		MAX
	};
#pragma endregion 定数定義

	// コンストラクタ
	Boss(const VECTOR& target);	
	// デストラクタ
	~Boss() override;			

	// UI描画
	void UIDraw(void);

	// 当たり判定処理
	void OnCollision(UnitBase* other) override;

	// ゲッター・セッター関数==================
	const std::vector<AttackBase*> GetAttackIns(void);
	const STATE GetState(void) { return state_; }

	void SetMuscleRatio(const float ratio) { playerMuscleRatio_ = ratio; }
	void SetVoiceLevel(const float voiceLevel) { voiceLevel_ = (int)voiceLevel; }
	// ========================================

protected:
	void SubLoad(void) override;
	void SubInit(void) override;
	void SubUpdate(void) override;
	void SubDraw(void) override;
	void SubRelease(void) override;
private:

	void SetMatrix(void);
	void LookTarget(void);

#pragma region ステート管理用関数
	void ToDeath(void);		// 死亡ステートへの遷移関数

	void Idle(void);		// 何もしてない
	void Attack(void);		// 攻撃ステート
	void Damage(void);		// ダメージステート
	void Death(void);		// 死亡ステート
#pragma endregion

#pragma region 攻撃関係の処理を別で管理
	Boss::ATTACK AttackLottery(void);

	std::vector<ATTACK> attackTable_;  // 攻撃順のテーブル
	size_t attackTableIndex_ = 0;      // 現在どの攻撃か

	void AttackLoad(void);
	void AttackInit(void);
	void AttackDraw(void);
	void AttackRelease(void);
#pragma endregion

#pragma region 変数定義
	// 攻撃のインスタンスの変数
	std::vector<AttackBase*> attacks_;

	// ボスの手用のモデルハンドル
	int handModel_;

	// 筋肉の割合
	float playerMuscleRatio_;

	// ステート管理用変数
	STATE state_;
	ATTACK attackState_;

	int attackCounter_;		// 攻撃用カウンタ
	bool isAttackInit_;		// (true : 攻撃開始後 / false : 攻撃開始前)
	bool isAttackEnd_;		// (true : 攻撃終了後 / false : 攻撃終了前)

	const VECTOR& playerPos_;	// プレイヤーの座標
	int voiceLevel_;		// マイクに入力された音量
#pragma endregion 変数定義

};