#pragma once
#include"../SceneBase.h"

#include"../../Common/Vector2.h"

#include"../../Manager/Collision/Collision.h"

#include<vector>

class Player;

class GameScene : public SceneBase
{
public:
	GameScene();
	~GameScene()override;

	// 読み込み
	void Load(void)override;
	// 初期化処理
	void Init(void)override;
	// 更新ステップ
	void Update(void)override;
	// 描画処理
	void Draw(void)override;
	// 解放処理
	void Release(void)override;


#pragma region 画面演出
	// ヒットストップ演出
	static void HitStop(int time = 20) { hitStop_ = time; }

	// スロー演出
	static void Slow(int time = 10, int inter = 5) { slow_ = time; slowInter_ = inter; }

	// 画面揺れの種類
	enum ShakeKinds { WID/*横揺れ*/, HIG/*縦揺れ*/, DIAG/*斜め揺れ*/, ROUND/*くるくる*/ };
	// 画面揺れの大きさ
	enum ShakeSize { SMALL = 3/*小さく*/, MEDIUM = 5/*中くらい*/, BIG = 8, /*大きく*/ };

	/// <summary>
	/// 画面揺らし
	/// </summary>
	/// <param name="kinds">揺れ方(enum ShakeKinds を使用)</param>
	/// <param name="size">揺れる大きさ(enum ShakeSize を使用)</param>
	/// <param name="time">揺れる時間(フレーム数)</param>
	static void Shake(ShakeKinds kinds = ShakeKinds::DIAG, ShakeSize size = ShakeSize::MEDIUM, int time = 20);
#pragma endregion

private:
	Collision* collision_;

	Player* player_;

#pragma region 画面演出
	bool ScreenProduction(void);

	// ヒットストップカウンター
	static int hitStop_;

	// スローカウンター
	static int slow_;
	static int slowInter_;

	// 画面揺れ------------------------
	int mainScreen_;
	static int shake_;
	static ShakeKinds shakeKinds_;
	static ShakeSize shakeSize_;
	Vector2I ShakePoint(void);
	//---------------------------------
#pragma endregion

	// デバック用の座標線描画
	void DrawAxis(VECTOR origin, float length)
	{
		// X軸（赤）
		DrawLine3D(origin,
			VAdd(origin, VGet(length, 0.0f, 0.0f)),
			GetColor(255, 0, 0));

		// Y軸（緑）
		DrawLine3D(origin,
			VAdd(origin, VGet(0.0f, length, 0.0f)),
			GetColor(0, 255, 0));

		// Z軸（青）
		DrawLine3D(origin,
			VAdd(origin, VGet(0.0f, 0.0f, length)),
			GetColor(0, 0, 255));
	}

};

using ShakeKinds = GameScene::ShakeKinds;
using ShakeSize = GameScene::ShakeSize;