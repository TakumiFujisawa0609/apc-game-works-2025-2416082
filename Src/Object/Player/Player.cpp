#include "Player.h"
#include <cmath>

#include "../../Application/Application.h"

#include "../../Manager/Animation/AnimationController.h"
#include "../../Manager/Input/InputManager.h"   
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/MicInput/MicInput.h"

#include "../Camera/Camera.h"

#include "../Boss/Boss.h"
#include "../Boss/Attack/Hand/HandSlap.h"

#include "Arm/LeftArm.h"
#include "Arm/RightArm.h"



Player::Player() :
    animation_(nullptr),
    mic_(nullptr),
    leftArm_(nullptr),
    rightArm_(nullptr)
{
}

Player::~Player()
{
}

// 最初に一度だけ呼び出す関数
void Player::SubLoad(void)
{
   // パスの省略
   std::string path = "Data/Model/Player/";

   // モデルのロード
   unit_.model_ = MV1LoadModel((path + "Player1.mv1").c_str());

   hpFrameImg_ = LoadGraph("Data/Image/PlayerUI/HP_Frame.png");

#pragma region クラスの定義

   // アニメーションクラス
    animation_ = new AnimationController(unit_.model_);

    mic_ = new MicInput();


   // 左腕
   leftArm_ = new LeftArm(unit_.model_);
   leftArm_->Load();

   // 右腕
   rightArm_ = new RightArm(unit_.model_);
   rightArm_->Load();

#pragma endregion

    // アニメーションのロード
    animation_->Add((int)(ANIM_TYPE::IDLE), 30.0f, (path + "Animation/Idle1.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::RUN), 50.0f, (path + "Animation/Run.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ROLL), 100.0f, (path + "Animation/Evasion.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK1), 100.0f, (path + "Animation/Punching.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK2), 100.0f, (path + "Animation/Punching2.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK3), 130.0f, (path + "Animation/Swiping.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::DEATH), 30.0f, (path + "Animation/Death.mv1").c_str());

    // 音声のロード
    SoundManager::GetIns().Load(SOUND::PLAYER_BIG_ATTACK);
    SoundManager::GetIns().Load(SOUND::PLAYER_SMALL_ATTACK);
}

//初期化処理
void Player::SubInit(void)
{
    ParamInit();


    // カメラの注視点をずらす
    currentHeight = Camera::CAMERA_PLAYER_POS;

    //カメラ座標
    cameraPos_ = Utility::VECTOR_ZERO;



    // 向き
    move_ = Utility::VECTOR_ZERO;

    // 攻撃処理の初期化
    isAttacked_ = false;

    // カウンタの初期化-----
    attacConboCnt_ = 0;         // 攻撃段階が今どこか見るためのカウンタ
    attackEscapeCounter_ = 0;   // 一定時間攻撃中だったら強制的に抜け出すためのカウンタ
    rollCounter_ = 0;           // 回避時間用カウンタ
    nextRollCounter_ = 0;       // 一度回避を行ったとき、次の回避までのクールタイム用
    // ---------------------
    
    // 関数ポインタに登録
    StateAdd((int)STATE::IDLE,      [this]() { Idle();      });
    StateAdd((int)STATE::ATTACK,    [this]() { Attack();    });
    StateAdd((int)STATE::MOVE,      [this]() { Move();      });
    StateAdd((int)STATE::ROLL,      [this]() { Roll();      });
    StateAdd((int)STATE::DEATH,     [this]() { Death();     });

    state_ = STATE::IDLE;
    conbo_ = CONBO::CONBO1;
    muscleLevel_ = MUSCLE_LEVEL::LOW;

    // 腕の初期化
    leftArm_->Init();
    rightArm_->Init();

    // 入力デバイスの初期化
    mic_->Init();
    mic_->Start();
}

//更新処理
void Player::SubUpdate(void)
{
    auto& input = InputManager::GetInstance();

    if (state_ != STATE::ATTACK) {
        isAttacked_ = false;
    }

    if (GetMuscleRatio(4) <= 0.03f) {
        muscleLevel_ = MUSCLE_LEVEL::LOW;
    }
    else if (GetMuscleRatio(4) <= 0.7f) {
        muscleLevel_ = MUSCLE_LEVEL::NORMAL;
    }
    else  if (GetMuscleRatio(4) <= 1.0f) {
        muscleLevel_ = MUSCLE_LEVEL::BIG;
    }
   

    // ステージとの当たり判定を無理やりやってる処理
    StageCollision();

    // 音量で筋肉を増やす処理
    VoiceUpMuscle();

    // 回避用のカウンタ処理
    RollCountUpdate();

    // ステートごと更新処理呼び出し
    StateUpdate(static_cast<int>(state_));
     
    //状態遷移用関数
    StateManager();

    // プレイヤーの無敵時間処理
	Invi();

    //カメラ
    CameraPosUpdate();

#pragma region 他クラスの更新処理

    // 腕の更新処理
    leftArm_->Update();
    rightArm_->Update();

    // アニメーション処理
    animation_->Update();

#pragma endregion

#ifdef _DEBUG
    if (input.IsNew(KEY_INPUT_P)) {
        leftArm_->SetAttackTime(60);
        rightArm_->SetAttackTime(60);
    }
#endif // _DEBUG
}

// 描画処理
void Player::SubDraw(void)
{
	if (!unit_.isAlive_)return;

    // プレイヤーの描画
    SetMatrix();

    // 腕に関する描画処理
    leftArm_->Draw();
    rightArm_->Draw();

    MV1DrawModel(unit_.model_);


}

// 解放処理
void Player::SubRelease(void)
{
    //アニメーション
    if (animation_)
    {
        animation_->Release();
        delete animation_;
        animation_ = nullptr;
    }

    // 左腕
    if (leftArm_)
    {
        leftArm_->Release();
        delete leftArm_;
        leftArm_ = nullptr;
    }

    // 右腕
    if (rightArm_)
    {
        rightArm_->Release();
        delete rightArm_;
        rightArm_ = nullptr;
    }

    if (mic_)
    {
        mic_->Stop();
        delete mic_;
        mic_ = nullptr;
    }

    // モデルの解放
    MV1DeleteModel(unit_.model_);

    // サウンドの開放
    for (int i = 0; i < (int)SOUND::MAX; i++) {
        SoundManager::GetIns().Delete((SOUND)i);
    }
}


//当たり判定
void Player::OnCollision(UnitBase* other)
{
    if (unit_.inviciCounter_ > 0) { return; }

    if (dynamic_cast<Boss*>(other))
    {
        return;
    }

    if (HandSlap* hand = dynamic_cast<HandSlap*>(other))
    {
        if (hand->isHit()) { return; }
        SetDamage(10);
        return;
    }
}

void Player::UIDraw(void)
{
    //HP描画
    HpDraw();
    rightArm_->UIDraw();
    MuscleGaugeDraw();


#ifdef _DEBUG
    DebugDraw();

    // 現在の筋肉の割合（ratio）
    mic_->VoiceLevelDraw();
#endif 
}

// 立ち止まっているときの処理
void Player::Idle(void)
{
    animation_->Play((int)ANIM_TYPE::IDLE, true);
}

// 移動処理
void Player::Move(void)
{
    auto& camera = Camera::GetInstance();
    move_ = Utility::VECTOR_ZERO;

    SetMoveVec();

    // ---------- 実際の移動 ----------
    if (move_.x != 0.0f || move_.z != 0.0f)
    {
        MATRIX mat = MGetRotY(camera.GetAngle().y * DX_PI_F / 180.0f);
        VECTOR worldMove = VTransform(VNorm(move_), mat);

        // 移動速度（筋肉量で変動）
        unit_.para_.speed = (GetMuscleRatio(LeftArm::LEFT_ARM_INDEX) < 0.7f) ? MOVE_SPEED : MOVE_SPEED / 2;

        worldMove = VScale(worldMove, unit_.para_.speed);
        unit_.pos_ = VAdd(unit_.pos_, worldMove);

        // プレイヤーの向きを移動方向に補間
        float targetY = atan2f(worldMove.x, worldMove.z);
        unit_.angle_.y = Utility::LerpAngle(unit_.angle_.y, targetY, 0.3f);

    }

    animation_->Play((int)ANIM_TYPE::RUN, true);
}

void Player::Attack(void)
{
    Camera& camera = Camera::GetInstance();

    SetMoveVec();

    // 実際の移動
    if (move_.x != 0.0f || move_.z != 0.0f)
    {
        MATRIX mat = MGetRotY(camera.GetAngle().y * DX_PI_F / 180.0f);
        VECTOR worldMove = VTransform(VNorm(move_), mat);

        // プレイヤーの向きを移動方向に補間
        float targetY = atan2f(worldMove.x, worldMove.z);
        unit_.angle_.y = Utility::LerpAngle(unit_.angle_.y, targetY, 0.3f);

    }

    if (animation_->IsEnd((int)ANIM_TYPE::ATTACK1)) {
        state_ = STATE::IDLE;
    }

}

void Player::Roll(void)
{
    auto& camera = Camera::GetInstance();

    rollCounter_++;

    // アイドルステートに移行
    if (rollCounter_ > ROLLING_TIME)
    {
        state_ = STATE::IDLE;
        rollCounter_ = 0;
        nextRollCounter_ = NEXT_ROLL_TIME;
        return;
    }

    // ローリング中
    if (move_.x != 0.0f || move_.z != 0.0f)
    {
        animation_->Play((int)ANIM_TYPE::ROLL, false);

        // カメラの向きから回転行列を作る
        MATRIX mat = MGetRotY(Utility::Deg2RadF(camera.GetAngle().y));

        // カメラ基準の方向をワールド基準に変換
        VECTOR worldMove = VTransform(move_, mat);
        
        // 正規化＋スケーリング
        worldMove = VNorm(worldMove);
        worldMove = VScale(worldMove, ROLL_SPEED);

        unit_.pos_ = VAdd(unit_.pos_, worldMove);

        // プレイヤーの向きも移動方向に合わせる
        unit_.angle_.y = atan2f(worldMove.x, worldMove.z);
        return;
    }
}

void Player::Death(void)
{
    animation_->Play((int)ANIM_TYPE::DEATH, false);

    if (animation_->IsEnd((int)ANIM_TYPE::DEATH)) {
        unit_.isAlive_ = false;
    }
}

void Player::StateManager(void)
{
    //HPがなくなったら死亡処理に移行
    if (unit_.hp_ <= 0) {
        unit_.hp_ = 0;
        state_ = STATE::DEATH;
        return;
    }

    switch (state_)
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
    }
}

void Player::DoMove(void)
{
    // キーボード入力チェック
    bool keyboardMove =
        CheckHitKey(KEY_INPUT_W) ||
        CheckHitKey(KEY_INPUT_S) ||
        CheckHitKey(KEY_INPUT_A) ||
        CheckHitKey(KEY_INPUT_D);

    // コントローラー入力チェック
    XINPUT_STATE padState{};
    bool controllerMove = false;

    if (GetJoypadXInputState(DX_INPUT_PAD1, &padState) == 0)
    {
        const float deadZone = 8000.0f;
        int joyX = padState.ThumbLX;
        int joyY = padState.ThumbLY;

        // 左スティックがデッドゾーンを超えて動いていたら
        if (fabsf((float)joyX) > deadZone || fabsf((float)joyY) > deadZone)
        {
            controllerMove = true;
        }
    }

    // 状態遷移処理 
    if (keyboardMove || controllerMove)
    {
        state_ = STATE::MOVE;
    }
}

void Player::DoIdle(void)
{
    // コントローラー入力チェック
    XINPUT_STATE padState{};
    bool controllerMove = false;

    if (GetJoypadXInputState(DX_INPUT_PAD1, &padState) == 0)
    {
        const float deadZone = 8000.0f;
        int joyX = padState.ThumbLX;
        int joyY = padState.ThumbLY;

        // 左スティックがデッドゾーンを超えて動いていたら
        if (fabsf((float)joyX) > deadZone || fabsf((float)joyY) > deadZone)
        {
            controllerMove = true;
        }
    }
    // 移動入力がなくなったらIDLEに戻る
    if (!(CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_S) ||
        CheckHitKey(KEY_INPUT_A) || CheckHitKey(KEY_INPUT_D) || controllerMove))
    {
        state_ = STATE::IDLE;
    }
}

void Player::DoAttack(void)
{
    auto& input = InputManager::GetInstance();
    auto& sound = SoundManager::GetIns();

    bool isTrg = (input.IsTrgMouseLeft() ||
        input.IsTrgDown(KEY_INPUT_J) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER1) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER2));

    if (isTrg) {
        state_ = STATE::ATTACK;
        animation_->Play((int)ANIM_TYPE::ATTACK1, false);
    }
}

void Player::DoRoll(void)
{
    auto& input = InputManager::GetInstance();

    // 回避のクールタイムが終わっていない and 攻撃が終了していなければ
    if (nextRollCounter_ > 0 || state_ == STATE::ATTACK) {
        return;
    }

    // どちらかのキーが押された瞬間にROLLへ
    if (input.IsTrgDown(KEY_INPUT_LSHIFT) || 
        input.IsTrgDown(KEY_INPUT_K) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER1) || 
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER2)) {
        state_ = STATE::ROLL;
    }
}

void Player::ParamInit(void)
{
    unit_.para_.colliShape = CollisionShape::CAPSULE;
    unit_.para_.colliType = CollisionType::ALLY;

    // プレイヤーのパラメータの初期化
    unit_.para_.capsuleHalfLen = CAPSULE_HALF_LENGTH; // カプセルの円から円までの長さの半分
    unit_.para_.radius = RADIUS_SIZE;                 // 半径の長さ
    unit_.hp_ = HP_MAX;

    unit_.isAlive_ = true;                  // プレイヤーの生存フラグ
    unit_.pos_ = DEFAULT_POS;               // プレイヤーの座標
    unit_.angle_ = Utility::VECTOR_ZERO;    // プレイヤーの向き・アングル
    unit_.para_.speed = MOVE_SPEED;
}

void Player::VoiceUpMuscle(void)
{
    // マイクレベル（音量）が4000より高いとき、筋肉を増やす
    if (mic_->GetPlayGameLevel() > 4000) {
        AddBoneScale(4, { 0.01f,0.01f,0.01f });
    }

    // 平常時筋肉は減り続ける
    AddBoneScale(4, DOWN_MUSCLE);
}

const float Player::GetMuscleRatio(int index)
{
    // 指定ボーンのローカル行列を取得
    MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, index);

    // Y軸のスケールを抽出
    float scaleY = VSize({ mat.m[1][0], mat.m[1][1], mat.m[1][2] });

    // 筋肉の比率を正規化
    float ret = Utility::Clamp((scaleY - ArmBase::MIN_MUSCLE.y) / (ArmBase::MAX_MUSCLE.y - ArmBase::MIN_MUSCLE.y), 0.0f, 1.0f);

    return ret;
}

void Player::SetDamage(int damage)
{
    unit_.hp_ -= damage;
    unit_.inviciCounter_ = INVI_TIME;
}




 //カメラが向く方向の処理
void Player::CameraPosUpdate(void)
{
    cameraPos_ = unit_.pos_;

    //もともとボーンごとじゃなくてモデル自体を
    //大きくしていたのでそれに応じてカメラの位置も高くなるようにしていた
    float scaleAvg = (unit_.scale_.x + unit_.scale_.y + unit_.scale_.z) / 3.0f;
    float targetHeight = Camera::CAMERA_PLAYER_POS * scaleAvg;

    currentHeight += (targetHeight - currentHeight) * 0.2f;

    cameraPos_.y = unit_.pos_.y + currentHeight;
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

void Player::SetMoveVec(void)
{
    // ---------- キーボード入力 ----------
    if (CheckHitKey(KEY_INPUT_W)) move_.z += 1.0f;
    if (CheckHitKey(KEY_INPUT_S)) move_.z -= 1.0f;
    if (CheckHitKey(KEY_INPUT_A)) move_.x -= 1.0f;
    if (CheckHitKey(KEY_INPUT_D)) move_.x += 1.0f;

    // ---------- Xbox コントローラー入力 ----------
    XINPUT_STATE padState{};
    if (GetJoypadXInputState(DX_INPUT_PAD1, &padState) == 0)
    {
        const float deadZone = 8000.0f;   // デッドゾーン
        const float maxValue = 32767.0f;  // 最大スティック値

        // デッドゾーン処理
        if (fabsf((float)padState.ThumbLX) > deadZone) {
            move_.x += (float)padState.ThumbLX / maxValue;
        }
        if (fabsf((float)padState.ThumbLY) > deadZone) {
            move_.z += (float)padState.ThumbLY / maxValue;
        }
    }
}

//デバック用描画
void Player::DebugDraw(void)
{
    auto& input = InputManager::GetInstance();

    // プレイヤーのステートをデバッグ表示用
    switch (state_)
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

    // 当たり判定を視覚的に見えるように
    VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
    VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
    DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, 0x0f0f0f, 0x0f0f0f, false);

    // 音量確認用
    int volume = mic_->GetLevel();
    DrawFormatString(0, 0, 0xffffff, "入力された音量(%i)", volume);
}

void Player::SetMatrix(void)
{
    VECTOR ofset = { 0.0f, -unit_.para_.capsuleHalfLen ,0.0f };

    // 回転行列の作成　
    MATRIX mat = MGetIdent();

    mat = MMult(mat, MGetRotX(unit_.angle_.x));
    mat = MMult(mat, MGetRotY(unit_.angle_.y));
    mat = MMult(mat, MGetRotZ(unit_.angle_.z));

    // モデルの反転を修正
    MATRIX localMat = MGetIdent();
    localMat = MMult(localMat, MGetRotX(LOCAL_ANGLE.x));
    localMat = MMult(localMat, MGetRotY(LOCAL_ANGLE.y));
    localMat = MMult(localMat, MGetRotZ(LOCAL_ANGLE.z));

    mat = MMult(localMat, mat);

    mat = MMult(MGetScale(unit_.scale_), mat);

    VECTOR worldPos = VTransform(ofset, mat);

    mat.m[3][0] = unit_.pos_.x + worldPos.x;
    mat.m[3][1] = unit_.pos_.y + worldPos.y;
    mat.m[3][2] = unit_.pos_.z + worldPos.z;

    // 行列の設定
    MV1SetMatrix(unit_.model_, mat);
}

// HP描画
void Player::HpDraw(void)
{
    VECTOR pos1 = { Application::SCREEN_SIZE_X / 20,Application::SCREEN_SIZE_Y / 20 };
    VECTOR pos2 = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 10 };

    //float nowHp = unit_.hp_;
    //float fullLength = pos2.x - pos1.x;
    //float hpRatio = nowHp / HP_MAX;
    //float currentLength = fullLength * hpRatio;

    //// HPバーを描画
    //DrawBox(pos1.x + 50, pos1.y + 5, (pos1.x - 50) + currentLength, pos2.y-5, 0x44ff44, true);

    //DrawExtendGraph(pos1.x, pos1.y, pos2.x, pos2.y, hpFrameImg_, true);



    HpBarDraw(unit_.hp_, HP_MAX, pos1, pos2, 0x77ff77);
}

// ステージに対して無理やり当たり判定をしている
void Player::StageCollision(void)
{
    float distance = sqrtf(unit_.pos_.x * unit_.pos_.x + unit_.pos_.z * unit_.pos_.z);

    //　移動範囲制限（外側）
    if (distance > STAGE_COLLISION_RADIUS_OUTSIDE)
    {
        // 原点からの方向ベクトルを正規化して外側の円に制限
        float nx = unit_.pos_.x / distance;
        float nz = unit_.pos_.z / distance;
        unit_.pos_.x = nx * STAGE_COLLISION_RADIUS_OUTSIDE;
        unit_.pos_.z = nz * STAGE_COLLISION_RADIUS_OUTSIDE;
    }

    // 移動範囲制限（内側）
    if (distance < STAGE_COLLISION_RADIUS_INSIDE)
    {
        // 原点からの方向ベクトルを正規化して内側の円に制限
        float nx = unit_.pos_.x / distance;
        float nz = unit_.pos_.z / distance;
        unit_.pos_.x = nx * STAGE_COLLISION_RADIUS_INSIDE;
        unit_.pos_.z = nz * STAGE_COLLISION_RADIUS_INSIDE;
    }
}

// ゲージ描画の定数
const int GAUGE_WIDTH = 150;  // ゲージ全体の幅
const int GAUGE_HEIGHT = 20;  // ゲージの高さ
const int GAUGE_Y = 100;       // ゲージ描画開始Y座標
const int GAUGE_X = 100;       // ゲージ描画開始X座標
const int SEGMENT_WIDTH = GAUGE_WIDTH / 3; // 1セグメントの幅

void Player::MuscleGaugeDraw(void)
{
    // 各セグメントの色を定義 (DxLibのGetColorを使用)
    // RGB (R, G, B) の順
    unsigned int color_green = GetColor(0, 255, 0); // 緑
    unsigned int color_yellow = GetColor(255, 255, 0); // 黄色
    unsigned int color_red = GetColor(255, 0, 0); // 赤

    // ゲージの背景（全枠）を描画
    // DrawBox(x1, y1, x2, y2, Color, FillFlag)
    // 枠だけ描画する場合は FillFlag を FALSE (0) に
    DrawBox(GAUGE_X, GAUGE_Y, GAUGE_X + GAUGE_WIDTH, GAUGE_Y + GAUGE_HEIGHT, GetColor(100, 100, 100), FALSE);

    // 筋肉レベルに応じてセグメントを描画
    for (int i = 0; i < 3; i++) {
        // i は 0, 1, 2 に対応 (0:緑, 1:黄, 2:赤)
        int x1 = GAUGE_X + i * SEGMENT_WIDTH; // 描画開始X座標
        int y1 = GAUGE_Y;                     // 描画開始Y座標
        int x2 = x1 + SEGMENT_WIDTH;          // 描画終了X座標
        int y2 = GAUGE_Y + GAUGE_HEIGHT;      // 描画終了Y座標

        unsigned int segment_color;

        // セグメントの色を決定
        if (i == 0) {
            segment_color = color_green;
        }
        else if (i == 1) {
            segment_color = color_yellow;
        }
        else { // i == 2
            segment_color = color_red;
        }

        // 筋肉レベルが現在のセグメント以上の場合、色を付けて描画
        // i+1 は筋肉レベル (1, 2, 3) に対応
        if (muscleLevel_ >= i + 1) {
            // 塗りつぶしの四角を描画
            DrawBox(x1, y1, x2, y2, segment_color, TRUE);
        }

        // セグメントごとの区切り線（オプション）
        DrawLine(x2, y1, x2, y2, GetColor(0, 0, 0)); // 黒い線で区切る
    }
}


int Player::GetVoiceLevel(void)
{
    return mic_->GetPlayGameLevel();
}