#include "Player.h"
#include <cmath>
#include <EffekseerForDXLib.h>

#include"../../../Application/Application.h"

#include "../../../Manager/Input/KeyManager.h"
#include "../../../Manager/Sound/SoundManager.h"
#include "../../../Manager/Input/MicInput.h"
#include "../../../Manager/Camera/Camera.h"

#include"../../../Scene/Game/GameScene.h"

Player::Player() :
    mic_(nullptr),
    effectSoutPlayid_(0)
{
}

// 最初に一度だけ呼び出す関数
void Player::Load(void)
{
   // パスの省略
   std::string playerModelPath = "Data/Model/Player/";

   // モデルのロード
   trans_.Load("Player/Player1");

#pragma region クラスの定義

   // マイク
   //Utility::ClassNew(mic_);
   mic_ = new MicInput();
#pragma endregion

   SetDynamicFlg(true);
   SetGravityFlg(false);

   // プレイヤー本体のコライダー
   //ColliderCreate(new CapsuleCollider(TAG::PLAYER, CAPSULE_COLLIDER_START_POS, CAPSULE_COLLIDER_END_POS, RADIUS_SIZE));

   // 拳コライダー（左）
   ColliderCreate(new SphereCollider(TAG::PLAYER_KNUCKLE_LEFT, RADIUS_SIZE, RADIUS_SIZE, leftKnuckleLocalPos_, false));

   // 拳コライダー（右）
   ColliderCreate(new SphereCollider(TAG::PLAYER_KNUCKLE_RIGHT, RADIUS_SIZE, RADIUS_SIZE, rightKnuckleLocalPos_, false));

   // モデルアニメーションのロード--------------------------------

   // アニメーションコントローラーの生成
   CreateAnimationController();

   // アニメーションの追加
   for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++) {
       AddAnimation(i, ANIMATION_INFO[i].speed, (playerModelPath + "Animation/" + ANIMATION_INFO[i].name + ".mv1").c_str());
   }
   // -------------------------------------------------------

   // 音声のロード-----------------------------------------
   SoundManager::GetIns().Load(SOUND::PLAYER_BIG_ATTACK);
   SoundManager::GetIns().Load(SOUND::PLAYER_SMALL_ATTACK);
   // -----------------------------------------------------

   // effekseerのロード---------------------------------------
   
   effectSoutId_ = LoadEffekseerEffect("Data/Effekseer/Shout.efkefc");

   effectSoutPlayid_ = PlayEffekseer3DEffect(effectSoutId_);

   // -------------------------------------------------------

   trans_.localAngle = Vector3::Yonly(Utility::Deg2RadF(180.0f));
   trans_.centerDiff = Vector3::Yonly(-166.573f);
}

//初期化処理
void Player::CharactorInit(void)
{
    // 攻撃処理の初期化
    for (bool& flg : isAttacked_) { flg = false; }

    // カウンタの初期化-----
    rollCounter_ = 0;           // 回避時間用カウンタ
    nextRollCounter_ = 0;       // 一度回避を行ったとき、次の回避までのクールタイム用
    attackEscapeCounter_ = 0;
    // ---------------------

    hp_ = HP_MAX;

    // 関数ポインタに登録
    CHARACTOR_SET_STATE((int)STATE::IDLE, &Player::Idle);
    CHARACTOR_SET_STATE((int)STATE::ATTACK, &Player::Attack);
    CHARACTOR_SET_STATE((int)STATE::MOVE, &Player::Move);
    CHARACTOR_SET_STATE((int)STATE::ROLL, &Player::Roll);
    CHARACTOR_SET_STATE((int)STATE::DEATH, &Player::Death);

    state_ = (int)STATE::IDLE;
    conbo_ = CONBO::CONBO1;

    // 入力デバイスの初期化
    mic_->Init();
    mic_->Start();

    SetIsDraw(true);
    SetJudge(true);
}

//更新処理
void Player::CharactorUpdate(void)
{
    // ステージとの当たり判定を無理やりやってる処理
    StageCollision();

    // 音量で筋肉を増やす処理
    VoiceUpMuscle();

    // 回避用のカウンタ処理
    RollCountUpdate();

    ////状態遷移用関数
    StateManager();

    //カメラ
    CameraPosUpdate();
}

// 描画処理
void Player::CharactorDraw(void)
{
    // その後に各種設定
    float SCALE = 10.0f;
    SetScalePlayingEffekseer3DEffect(
        effectSoutPlayid_, SCALE, SCALE, SCALE);

    VECTOR angles = { 0.0f, DX_PI_F, 0.0f };
    SetRotationPlayingEffekseer3DEffect(effectSoutPlayid_, angles.x, angles.y, angles.z);

    SetPosPlayingEffekseer3DEffect(
        effectSoutPlayid_,
        trans_.pos.x,
        trans_.pos.y,
        trans_.pos.z
    );
}

void Player::CharactorAlphaDraw(void)
{
}

// 解放処理
void Player::CharactorRelease(void)
{
    // マイクインプット
    if (mic_) {
        mic_->Stop();
        delete mic_;
        mic_ = nullptr;
    }

    // エフェクトの解放
    DeleteEffekseerEffect(effectSoutId_);
}


//当たり判定
void Player::OnCollision(const ColliderBase& collider)
{
    if (GetInviCounter() > 0) { return; }

    switch (collider.GetTag())
    {
    case ColliderBase::TAG::BOSS:
        return;
    case ColliderBase::TAG::BOSS_HANDSLAP:
        SetDamage(10);
        GameScene::Shake();
        return;
    case ColliderBase::TAG::BOSS_ROTATEHAND:
    case ColliderBase::TAG::BOSS_SHOT:
        SetDamage(10);
        GameScene::Shake();
        return;
    case ColliderBase::TAG::ENEMY:
        return;
    default: { return; }
    }
}

void Player::OnGrounded(void)
{
    ActorBase::OnGrounded();
}

// UIの描画関数
void Player::UiDraw(void)
{
    //HP描画
    HpDraw();


#ifdef _DEBUG
    if (App::GetIns().IsDrawDebug()) {
        // プレイヤーのステートをデバッグ表示用
        switch ((STATE)state_)
        {
        case Player::STATE::IDLE:
            DrawString(0, 100, "IDLE", 0xffffff);
            break;
        case Player::STATE::MOVE:
            DrawString(0, 100, "MOVE", 0xffffff);
            break;
        case Player::STATE::ATTACK:
            DrawString(0, 100, "ATTACK", 0xffffff);
            break;
        case Player::STATE::ROLL:
            DrawString(0, 100, "Roll", 0xffffff);
            break;
        }

        // 現在の筋肉の割合（ratio）
        mic_->VoiceLevelDraw();
    }
#endif 
}



// 立ち止まっているときの処理
void Player::Idle(void)
{
    //AnimePlay((int)ANIM_TYPE::IDLE);
}

// 移動処理
void Player::Move(void)
{
    Vector3 move = {};

    SetMoveVec(move);

    // ---------- 実際の移動 ----------
    if (move != 0.0f) {
        // 座標に移動量を加算
        trans_.pos += move * 15.0f;

        // 移動ベクトルのY軸角度を算出してメンバ変数に保持
        moveAngleY_ = atan2f(move.x, move.z);
    }

    // プレイヤーの角度を最終移動Y軸角度に補間
    trans_.angle.y = LerpAngle(trans_.angle.y, moveAngleY_, 0.3f);

    AnimePlay((int)ANIM_TYPE::RUN);
}

// 攻撃処理
void Player::Attack(void)
{
    float nowAnimeRatio = GetAnimeRatio();

#pragma region 移動処理
    // 攻撃の判定が発生する前の間、前方に移動させる
    if (nowAnimeRatio <= 0.7f) {
        // 移動方向ベクトル
        Vector3 move = {};

        SetMoveVec(move);

        if (move != 0.0f) { moveAngleY_ = atan2f(move.x, move.z); }

        move = {};

        // trans_.angle(角度) から ベクトル(向き) を割り出す
        move.x = sinf(trans_.angle.y);
        move.z = cosf(trans_.angle.y);

        // 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
        trans_.pos += move.Normalized() * CONBO_MOVE_SPEED[(int)conbo_];
    }

    // プレイヤーの角度を最終移動Y軸角度に補間
    trans_.angle.y = LerpAngle(trans_.angle.y, moveAngleY_, 0.1f);
#pragma endregion

#pragma region 攻撃判定の処理

    // 毎フレーム一旦オフ(攻撃判定)
    for (auto& c : ColliderSerch<SphereCollider>()) { c->SetJudgeFlg(false); }

    // 大体攻撃判定を発生させる時間
    if (nowAnimeRatio <= 0.7f) {
        switch (conbo_)
        {
        case CONBO::CONBO1:
        case CONBO::CONBO3: {
            MATRIX matp = MV1GetFrameLocalMatrix(trans_.model, LEFT_HAND_INDEX);
            leftKnuckleLocalPos_ = Vector3(matp.m[3][0], matp.m[3][1], matp.m[3][2]);
            ColliderSerch<SphereCollider>(TAG::PLAYER_KNUCKLE_LEFT).back()->SetJudgeFlg(true);
            break;
        }
        case CONBO::CONBO2: {
            MATRIX matp = MV1GetFrameLocalMatrix(trans_.model, RIGHT_HAND_INDEX);
            rightKnuckleLocalPos_ = Vector3(matp.m[3][0], matp.m[3][1], matp.m[3][2]);
            ColliderSerch<SphereCollider>(TAG::PLAYER_KNUCKLE_RIGHT).back()->SetJudgeFlg(true);
            break;
        }
        }
    }

    // 攻撃判定終わったらボタンで次段攻撃に遷移可能にしておく(操作性向上)
    if (nowAnimeRatio > 0.7f) { DoAttack(); }

    // 何も入力なく攻撃アニメーションが終了したら通常状態に自動で遷移
    if (IsAnimeEnd()) { state_ = (int)STATE::IDLE; }
#pragma endregion
}

// 回避処理
void Player::Roll(void)
{
#pragma region もともとの
    //rollCounter_++;

    // アイドルステートに移行
    //if (rollCounter_ > ROLLING_TIME) {
    //    state_ = (int)STATE::IDLE;
    //    rollCounter_ = 0;
    //    nextRollCounter_ = NEXT_ROLL_TIME;
    //    return;
    //}

    // ローリング中
    //if (move_.x != 0.0f || move_.z != 0.0f)
    //{
    //    AnimePlay((int)ANIM_TYPE::ROLL, false);

    //    // カメラの向きから回転行列を作る
    //    MATRIX mat = MGetRotY(Utility::Deg2RadF(camera.GetAngle().y));

    //    // カメラ基準の方向をワールド基準に変換
    //    VECTOR worldMove = VTransform(move_, mat);
    //    
    //    // 正規化＋スケーリング
    //    worldMove = VNorm(worldMove);
    //    worldMove = VScale(worldMove, ROLL_SPEED);

    //    unit_.pos_ = VAdd(unit_.pos_, worldMove);

    //    // プレイヤーの向きも移動方向に合わせる
    //    unit_.angle_.y = atan2f(worldMove.x, worldMove.z);
    //    return;
    //}
#pragma endregion

    float nowAnimeRatio = GetAnimeRatio();

    // 移動させる時間
    if (nowAnimeRatio <= 0.65f) {
        // 移動方向ベクトル
        Vector3 move = {};

        // trans_.angle(角度) から ベクトル(向き) を割り出す
        move.x = sinf(trans_.angle.y);
        move.z = cosf(trans_.angle.y);

        // 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
        trans_.pos += move.Normalized() * ROLL_SPEED;
    }

    if (IsAnimeEnd()) {
        AnimePlay((int)ANIM_TYPE::IDLE, true);
        state_ = (int)STATE::IDLE;
    }
}

// 死亡処理
void Player::Death(void)
{
    // アニメーション再生はDoDeath()のほうにかいた
    //animation_->Play((int)ANIM_TYPE::DEATH, false);

    if (IsAnimeEnd()) {
        // 終了判定

    }
}

// ステート管理関数
void Player::StateManager(void)
{
    //HPがなくなったら死亡処理に移行
    if (hp_ <= 0) {
        hp_ = 0;
        state_ = (int)STATE::DEATH;
        return;
    }

    // 現在の状態によって遷移できるステートを管理
    // 例）「現在がMOVEの状態ならIDLEとATTACKとROLLに遷移できる。
    //       現在がIDLEならMOVE、ATTACK、ROLLに遷移できる。」
    switch ((STATE)state_)
    {
    case Player::STATE::IDLE:
        DoMove();
        DoAttack();
        DoRoll();
        break;
    case Player::STATE::MOVE:
        DoIdle();
        DoAttack();
        DoRoll();
        break;
    case Player::STATE::ATTACK:
        
        break;
    case Player::STATE::DEATH:
        break;
    }
}

// 移動に遷移するための関数
void Player::DoMove(void)
{
    Vector3 move = {};

    SetMoveVec(move);

    if (move != 0.0f) {
        AnimePlay((int)ANIM_TYPE::RUN);
        state_ = (int)STATE::MOVE;
    }
}

// アイドルに遷移するための関数
void Player::DoIdle(void)
{
    Vector3 move = {};

    SetMoveVec(move);

    if (move == 0.0f) {
        AnimePlay((int)ANIM_TYPE::IDLE, true);
        state_ = (int)STATE::IDLE;
    }
}

// 攻撃に遷移する関数
void Player::DoAttack(void)
{
    // 最終段までいっている、または前の攻撃から一定時間すぎていたらフラグリセット
    if (isAttacked_[(int)CONBO::MAX - 1] || attackCounter_ > INPUT_ATTACK_FRAME) {
        for (bool& flg : isAttacked_) { flg = false; }
    }

    if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_ATTACK).down) {
        for (int i = 0; i < (int)CONBO::MAX; i++) {
            if (isAttacked_[i]) { continue; }
            isAttacked_[i] = true;
            attackCounter_ = 0;
            conbo_ = (CONBO)i;
            break;
        }

        state_ = (int)STATE::ATTACK;

        // アニメーションを再生
        switch (conbo_)
        {
        case Player::CONBO::CONBO1:
            AnimePlay((int)ANIM_TYPE::ATTACK1, false);
            break;
        case Player::CONBO::CONBO2:
            AnimePlay((int)ANIM_TYPE::ATTACK2, false);
            break;
        case Player::CONBO::CONBO3:
            AnimePlay((int)ANIM_TYPE::ATTACK3, false);
            break;
        }
    }
}

void Player::DoRoll(void)
{
    // 回避のクールタイムが終わっていない and 攻撃が終了していなければ
    //if (nextRollCounter_ > 0 || state_ == (int)STATE::ATTACK) { return; }

    // ↑後半はいらない。これをはしょるためのStateManage()

    // 回避のクールタイムが終わっていない場合は遷移不可能
    if (nextRollCounter_ > 0) { return; }

    // どちらかのキーが押された瞬間にROLLへ
    if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_ROWLING).down) {
        AnimePlay((int)ANIM_TYPE::ROLL, false);
        state_ = (int)STATE::ROLL;
    }
}

void Player::ParamInit(void)
{

}

void Player::AttackCounterUpdate(void)
{
    if (attackCounter_ <= INPUT_ATTACK_FRAME) { attackCounter_++; }
}

void Player::VoiceUpMuscle(void)
{
    // マイクレベル（音量）が4000より高いとき、筋肉を増やす
    if (mic_->GetPlayGameLevel() > 3000) {
        AddBoneScale(4, 0.07f);
        GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG, 60);
    }

    // 平常時筋肉は減り続ける
    AddBoneScale(4, DOWN_MUSCLE);
}

const float Player::GetMuscleRatio(int index)
{
    // 指定ボーンのローカル行列を取得
    MATRIX mat = MV1GetFrameLocalMatrix(trans_.model, index);

    // Y軸のスケールを抽出
    float scaleY = VSize({ mat.m[1][0], mat.m[1][1], mat.m[1][2] });

    // 筋肉の比率を正規化
    float ret = Utility::Clamp((scaleY - MIN_MUSCLE.y) / (MAX_MUSCLE.y - MIN_MUSCLE.y), 0.0f, 1.0f);

    return ret;
}

// プレイヤーのダメージ処理
void Player::SetDamage(int damage)
{
    hp_ -= damage;
    SetInviCounter(INVI_TIME);
}

// マイクの入力レベルのゲット関数
int Player::GetVoiceLevel(void) const
{
    return mic_->GetPlayGameLevel();
}

//カメラが向く方向の処理
void Player::CameraPosUpdate(void)
{
    cameraPos_ = trans_.pos;

    //もともとボーンごとじゃなくてモデル自体を
    //大きくしていたのでそれに応じてカメラの位置も高くなるようにしていた
    float scaleAvg = (trans_.scale.x + trans_.scale.y + trans_.scale.z) / 3.0f;
    float targetHeight = CAMERA_PLAYER_POS * scaleAvg;

    currentHeight += (targetHeight - currentHeight) * 0.2f;

    cameraPos_.y = trans_.pos.y + currentHeight;
}



void Player::RollCountUpdate(void)
{
    // ローリング制御
    if (nextRollCounter_ <= 0)
    {
        nextRollCounter_ = 0;
    }
    else
    {
        nextRollCounter_--;
    }
}

// 入力をみて移動方向を決める
void Player::SetMoveVec(Vector3& move)
{
    move = {};

    // ---------- Xbox コントローラー入力 ----------
    move = KEY::GetIns().GetLeftStickVec().ToVector3XZ();

    // ---------- キーボード入力 ----------
    if (move == 0.0f) {
        if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_MOVE_FRONT).now) { move.z += 1.0f; }
        if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_MOVE_BACK).now) { move.z -= 1.0f; }
        if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_MOVE_LEFT).now) { move.x -= 1.0f; }
        if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_MOVE_RIGHT).now) { move.x += 1.0f; }
    }

    if (move != 0.0f) { 
        move.Normalize();
        move.TransMatOwn(MGetRotY(Camera::GetIns().GetAngle().y));
    }
}

// HP描画
void Player::HpDraw(void)
{
    // HP描画する左上の座標と右下の座標
    VECTOR pos1 = { Application::SCREEN_SIZE_X / 20,Application::SCREEN_SIZE_Y / 20 };
    VECTOR pos2 = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 10 };

    // HP描画関数
    DrawBar(
        pos1.x, pos1.y, pos2.x, pos2.y,
        hp_, HP_MAX,
        0xaaffaa, 0x000000
    );
}

// ステージに対して無理やり当たり判定をしている
void Player::StageCollision(void)
{
    float distance = sqrtf(trans_.pos.x * trans_.pos.x + trans_.pos.z * trans_.pos.z);

    if (distance == 0.0f) { distance = 0.01f; }

    // 移動範囲制限（外側）
    if (distance > STAGE_COLLISION_RADIUS_OUTSIDE)
    {
        // 原点からの方向ベクトルを正規化して外側の円に制限
        float nx = trans_.pos.x / distance;
        float nz = trans_.pos.z / distance;
        trans_.pos.x = nx * STAGE_COLLISION_RADIUS_OUTSIDE;
        trans_.pos.z = nz * STAGE_COLLISION_RADIUS_OUTSIDE;
    }

    // 移動範囲制限（内側）
    if (distance < STAGE_COLLISION_RADIUS_INSIDE)
    {
        // 原点からの方向ベクトルを正規化して内側の円に制限
        float nx = trans_.pos.x / distance;
        float nz = trans_.pos.z / distance;
        trans_.pos.x = nx * STAGE_COLLISION_RADIUS_INSIDE;
        trans_.pos.z = nz * STAGE_COLLISION_RADIUS_INSIDE;
    }
}


void Player::AddBoneScale(int index, float scale)
{
    MATRIX mat = MV1GetFrameLocalMatrix(trans_.model, index);

    // 行列からスケール成分を抽出
    float currentScale[3];
    for (int i = 0; i < 3; i++) {
        currentScale[i] = VSize(VGet(mat.m[i][0], mat.m[i][1], mat.m[i][2]));
    }

    // スケール加算
    Vector3 newScale = Vector3(scale) + Vector3(currentScale[0], currentScale[1], currentScale[2]);

    // 最大値の制限
    if (newScale.x > MAX_MUSCLE.x) newScale.x = MAX_MUSCLE.x;
    if (newScale.y > MAX_MUSCLE.y) newScale.y = MAX_MUSCLE.y;
    if (newScale.z > MAX_MUSCLE.z) newScale.z = MAX_MUSCLE.z;

    // 最低値の制限
    if (newScale.x < MIN_MUSCLE.x) newScale.x = MIN_MUSCLE.x;
    if (newScale.y < MIN_MUSCLE.y) newScale.y = MIN_MUSCLE.y;
    if (newScale.z < MIN_MUSCLE.z) newScale.z = MIN_MUSCLE.z;

#ifdef _DEBUG
    // 筋肉量を確認する用の処理(デバッグ用)
    float avgScale = (newScale.x + newScale.y + newScale.z) / 3.0f;
    float avgMin = (MIN_MUSCLE.x + MIN_MUSCLE.y + MIN_MUSCLE.z) / 3.0f;
    float avgMax = (MAX_MUSCLE.x + MAX_MUSCLE.y + MAX_MUSCLE.z) / 3.0f;

    float muscleRatio_ = (avgScale - avgMin) / (avgMax - avgMin);
#endif // _DEBUG

    // スケール行列を作成
    MATRIX scaleMat = MGetScale(newScale.ToVECTOR());

    // 適用
    MV1SetFrameUserLocalMatrix(trans_.model, index, scaleMat);
}
