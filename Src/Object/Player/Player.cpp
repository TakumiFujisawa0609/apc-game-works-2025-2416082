#include "Player.h"
#include <cmath>

#include "../../Application/Application.h"

#include "../../Manager/Animation/AnimationController.h"
#include "../../Manager/Input/InputManager.h"   
#include "../../Manager/Sound/SoundManager.h"

#include "../Camera/Camera.h"

#include "../Boss/Boss.h"
#include "../Boss/Hand/BossRightHand.h"
#include "Arm/LeftArm.h"
#include "Arm/RightArm.h"



Player::Player()
{
}

Player::~Player()
{
}

void Player::Load(void)
{
   // パスの省略
   std::string path = "Data/Model/Player/";

   // モデルのロード
   unit_.model_ = MV1LoadModel((path + "Player.mv1").c_str());

#pragma region クラスの定義

   // アニメーションクラス
   animation_ = new AnimationController(unit_.model_);

   // 左腕
   leftArm_ = new LeftArm(unit_.model_);
   leftArm_->Load();
   leftArm_->SetAddArmScaleFunc([this](VECTOR scale) { this->AddArmScale(scale); });

   // 右腕
   rightArm_ = new RightArm(unit_.model_);
   rightArm_->Load();
   rightArm_->SetAddArmScaleFunc([this](VECTOR scale) { this->AddArmScale(scale); });

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

void Player::Init(void)
{
    unit_.para_.colliShape = CollisionShape::CAPSULE;
    unit_.para_.colliType = CollisionType::ALLY;

    // プレイヤーのパラメータの初期化
    unit_.para_.capsuleHalfLen = CAPSULE_HALF_LENGTH; // カプセルの円から円までの長さの半分
    unit_.para_.radius = RADIUS_SIZE;                 // 半径の長さ
    unit_.hp_ = 100;

    unit_.isAlive_ = true;                  // プレイヤーの生存フラグ
	unit_.pos_ = DEFAULT_POS;               // プレイヤーの座標
    unit_.angle_ = Utility::VECTOR_ZERO;    // プレイヤーの向き・アングル
    unit_.para_.speed = MOUSE_MOVED;


    // カメラの注視点をずらす
    currentHeight = Camera::CAMERA_PLAYER_POS;

    //カメラ座標
    cameraPos_ = Utility::VECTOR_ZERO;

    // 一度回避を行ったとき、次の回避までのクールタイム用
    nextRollCounter_ = 0;
    
    // プレイヤーモデルのボーンの名前を
    // 羅列させるためのデバッグ用変数
    frameScrollIndex_ = 0;

    // 三段攻撃のカウンタ変数
    attacConboCnt_ = 0;

    move_ = Utility::VECTOR_ZERO;

    // 攻撃処理の初期化
    isAttacked_ = false;

    // 筋肉を増やす
    isUpMuscle_ = false;

    // 関数ポインタに登録
    stateFuncs_ =
    {
        { STATE::IDLE,      &Player::Idle   },
        { STATE::MOVE,      &Player::Move   },
        { STATE::ATTACK,    &Player::Attack },
        { STATE::ROLL,      &Player::Roll   },
        { STATE::DEATH,     &Player::Death  },
    };
    
    state_ = STATE::IDLE;
    conbo_ = CONBO::CONBO1;

    // 腕の初期化
    leftArm_->Init();
    rightArm_->Init();
}

void Player::Update(void)
{
    auto& input = InputManager::GetInstance();

    if (input.IsTrgDown(KEY_INPUT_P)) {
        leftArm_->SetAttackTime(60);
        rightArm_->SetAttackTime(60);
    }

    if (state_ != STATE::ATTACK) {
        conbo_ = CONBO::CONBO1;
    }

    // ローリング制御
    if (nextRollCounter_ <= 0)
    {
        nextRollCounter_ = 0;
    }
    else
    {
        nextRollCounter_--;
    }

    if (state_ != STATE::ATTACK) {
        isAttacked_ = false;
    }

    if (unit_.hp_ <= 0) {
        unit_.hp_ = 0;
        state_ = STATE::DEATH;
    }

    //マッスル関係の処理用関数
    Muscle();

    // 関数ポインタでそれぞれのステートの
    // アップデート関数を呼び出している
    auto Func = stateFuncs_.find(state_);
    (this->*(Func->second))();

    //状態遷移用関数
    StateManager();

    // プレイヤーの無敵処理
	Invi();

    //カメラ
    CameraPosUpdate();

    // アニメーション処理
    animation_->Update();

    // 腕の更新処理
    leftArm_->Update();
    rightArm_->Update();

}

void Player::Draw(void)
{
	if (!unit_.isAlive_)return;

    // プレイヤーの描画
    DrawPlayer();

    // 腕に関する描画処理
    leftArm_->Draw();
    rightArm_->Draw();

}

// 解放処理
void Player::Release(void)
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

    // モデルの解放
    MV1DeleteModel(unit_.model_);

    // サウンドの開放
    for (int i = 0; i < (int)SOUND::MAX; i++) {
        SoundManager::GetIns().Delete((SOUND)i);
    }
}

void Player::UIDraw(void)
{
    //HP描画
    HpDraw();

    // 筋肉関係のUI描画
    MuscleDraw();
#ifdef _DEBUG
    DebugDraw();
    DrawFormatString(0, Application::SCREEN_SIZE_Y - 16, 0xffffff, "%f", GetMuscleRatio());
#endif 
}

//当たり判定
void Player::OnCollision(UnitBase* other)
{
    if (unit_.inviciCounter_ > 0) { return; }

    if (dynamic_cast<Boss*>(other))
    {

        return;
    }

    if (dynamic_cast<BossRightHand*>(other))
    {
        if (state_ == STATE::ROLL) {
            AddArmScale(UP_MUSCLE[1]);
            return;
        }
        AddArmScale({ -0.3f, -0.3f, -0.3f });
        unit_.hp_ -= 10;
        unit_.inviciCounter_ = INVI_TIME;
    }
}

// 筋肉処理
void Player::Muscle(void)
{
    //static int cnt = 0;

    //AddArmScale(DOWN_MUSCLE);

#ifdef _DEBUG
    if (CheckHitKey(KEY_INPUT_0))
    {
        AddArmScale({ -1.0f,-1.0f,-1.0f });
    }
    if (CheckHitKey(KEY_INPUT_O)) {
        AddArmScale(UP_MUSCLE[2]);
    }
#endif // _DEBUG


    //if (state_ != STATE::ATTACK)
    //{
    //    cnt = 0;
    //    isUpMuscle_ = false;
    //    return;
    //}

    //if (isUpMuscle_)
    //{
    //    cnt++;
    //    if (cnt <= 10)
    //    {
    //        AddArmScale(UP_MUSCLE[(int)conbo_]);
    //    }
    //    else
    //    {
    //        cnt = 0;
    //        isUpMuscle_ = false;
    //    }
    //}
}

void Player::MuscleDraw(void)
{
    DrawCircle(100, 100, 80, 0xaaaaaa, true);
    DrawMuscleGauge(100,100, 80, 50,GetMuscleRatio());
}

// どこのボーンかを見て、そのボーンのスケールに引数のscaleを加算する
void Player::AddBoneScale(int index, VECTOR scale)
{
    MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, index);

    // 行列からスケール成分を抽出
    float currentScale[3];
    for (int i = 0; i < 3; i++) {
        currentScale[i] = VSize(VGet(mat.m[i][0], mat.m[i][1], mat.m[i][2]));
    }

    // スケール加算
    VECTOR newScale = VAdd(scale,{ currentScale[0], currentScale[1], currentScale[2] });

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

    muscleRatio_ = (avgScale - avgMin) / (avgMax - avgMin);
#endif // _DEBUG

    // スケール行列を作成
    MATRIX scaleMat = MGetScale(newScale);

    // 適用
    MV1SetFrameUserLocalMatrix(unit_.model_, index, scaleMat);
}

void Player::AddArmScale(VECTOR scale)
{
    AddBoneScale(LeftArm::LEFT_ARM_INDEX, scale);

    //// 行列からスケール成分を抽出
    //float currentScale[3];
    //for (int i = 0; i < 3; i++) {
    //    currentScale[i] = VSize(VGet(mat.m[i][0], mat.m[i][1], mat.m[i][2]));
    //}

    //// スケール加算
    //VECTOR newScale = VAdd(scale, { currentScale[0], currentScale[1], currentScale[2] });

    AddBoneScale(RightArm::RIGHT_ARM_INDEX, scale);
}


// 何もしていない
void Player::Idle(void)
{
    animation_->Play((int)ANIM_TYPE::IDLE, true);
}

// 移動処理
void Player::Move(void)
{
    auto& camera = Camera::GetInstance();
    move_ = Utility::VECTOR_ZERO;

    // ---------- キーボード入力 ----------
    if (CheckHitKey(KEY_INPUT_W)) move_.z += 1.0f;
    if (CheckHitKey(KEY_INPUT_S)) move_.z -= 1.0f;
    if (CheckHitKey(KEY_INPUT_A)) move_.x -= 1.0f;
    if (CheckHitKey(KEY_INPUT_D)) move_.x += 1.0f;

    // ---------- Xbox コントローラー入力 ----------
    XINPUT_STATE padState{};
    if (GetJoypadXInputState(DX_INPUT_PAD1, &padState) == 0)
    {
        const float deadZone = 8000.0f;   // 中心の遊び
        const float maxValue = 32767.0f;  // 最大スティック値

        int joyX = padState.ThumbLX;
        int joyY = padState.ThumbLY;

        // デッドゾーン処理
        if (fabsf((float)joyX) > deadZone) {
            move_.x += (float)joyX / maxValue;
        }
        if (fabsf((float)joyY) > deadZone) {
            move_.z += (float)joyY / maxValue; // 上方向を前進に補正
        }
    }

    // ---------- 実際の移動 ----------
    if (move_.x != 0.0f || move_.z != 0.0f)
    {
        MATRIX mat = MGetRotY(camera.GetAngle().y * DX_PI_F / 180.0f);
        VECTOR worldMove = VTransform(VNorm(move_), mat);

        // 移動速度（筋肉量で変動）
        unit_.para_.speed = (GetMuscleRatio() < 0.7f) ? MOVE_SPEED : MOVE_SPEED / 2;

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
    auto& input = InputManager::GetInstance();
    auto& camera = Camera::GetInstance();

    move_ = Utility::VECTOR_ZERO;

    if (CheckHitKey(KEY_INPUT_W)) { move_ = VAdd(move_, {  0.0f, 0.0f,  1.0f }); }
    if (CheckHitKey(KEY_INPUT_S)) { move_ = VAdd(move_, {  0.0f, 0.0f, -1.0f }); }
    if (CheckHitKey(KEY_INPUT_A)) { move_ = VAdd(move_, { -1.0f, 0.0f,  0.0f }); }
    if (CheckHitKey(KEY_INPUT_D)) { move_ = VAdd(move_, {  1.0f, 0.0f,  0.0f }); }

    XINPUT_STATE padState{};
    if (GetJoypadXInputState(DX_INPUT_PAD1, &padState) == 0)
    {
        const float deadZone = 8000.0f;   // 中心の遊び
        const float maxValue = 32767.0f;  // 最大スティック値

        int joyX = padState.ThumbLX;
        int joyY = padState.ThumbLY;

        // デッドゾーン処理
        if (fabsf((float)joyX) > deadZone) {
            move_.x += (float)joyX / maxValue;
        }
        if (fabsf((float)joyY) > deadZone) {
            move_.z += (float)joyY / maxValue; // 上方向を前進に補正
        }
    }

    if (move_.x != 0.0f || move_.z != 0.0f)
    {
        MATRIX mat = MGetRotY(camera.GetAngle().y * DX_PI_F / 180.0f);
        VECTOR worldMove = VTransform(move_, mat);
        worldMove = VNorm(worldMove);

        float targetY = atan2f(worldMove.x, worldMove.z);
        unit_.angle_.y = Utility::LerpAngle(unit_.angle_.y, targetY, 0.5f);
    }

    int anim = (int)conbo_;
    switch (conbo_)
    {
    case CONBO::CONBO1: 
        anim = (int)ANIM_TYPE::ATTACK1;
        leftArm_->SetAttackTime(5);
        break;
    case CONBO::CONBO2: 
        anim = (int)ANIM_TYPE::ATTACK2; 
        rightArm_->SetAttackTime(5);
        break;
    case CONBO::CONBO3:
        anim = (int)ANIM_TYPE::ATTACK3; 
        leftArm_->SetAttackTime(5);
        break;
    }

    animation_->Play(anim, false);

    // 攻撃判定管理
    DoAttack();

    if (animation_->IsPassedRatio(anim, 0.0f) && !animation_->IsPassedRatio(anim, 0.7f))
    {
         VECTOR forward = VGet(
            sinf(unit_.angle_.y),
            0.0f,
            cosf(unit_.angle_.y)
        );

        float dashPower = 0.0f;

        dashPower = CONBO_MOVE_SPEED[(int)conbo_];

        unit_.pos_ = VAdd(unit_.pos_, VScale(forward, dashPower));
    }

    // アニメーションが終了したらリセット
    if (animation_->IsPassedRatio(anim, 0.7))
    {
        isAttacked_ = false;
        state_ = STATE::IDLE;
        conbo_ = CONBO::CONBO1;
    }

    static int cnt = 0;
    cnt++;
    if (cnt > 300) {
        cnt = 0;
        state_ = STATE::IDLE;
    }
}

void Player::Roll(void)
{
    auto& camera = Camera::GetInstance();

    static int cnt = 0;
    cnt++;

    // アイドルステートに移行
    if (cnt > ROLLING_TIME)
    {
        state_ = STATE::IDLE;
        cnt = 0;
        nextRollCounter_ = NEXT_ROLL_TIME;
        return;
    }

    // ローリング中
    if (move_.x != 0.0f || move_.z != 0.0f)
    {
        animation_->Play((int)ANIM_TYPE::ROLL, false);

        // カメラの向きから回転行列を作る
        MATRIX mat = MGetRotY(camera.GetAngle().y * DX_PI_F / 180.0f);

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

    // プレイヤーがどこに向いているかどうか
    //if (CheckHitKey(KEY_INPUT_W)) { move_ = { 0.0f, 0.0f, 1.0f  }; }
    //if (CheckHitKey(KEY_INPUT_S)) { move_ = { 0.0f, 0.0f, -1.0f }; }
    //if (CheckHitKey(KEY_INPUT_A)) { move_ = { -1.0f, 0.0f, 0.0f }; }
    //if (CheckHitKey(KEY_INPUT_D)) { move_ = { 1.0f, 0.0f, 0.0f  }; }
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
    switch (state_)
    {
    case Player::STATE::IDLE:
        DoWalk();
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

void Player::DoWalk(void)
{
    // --- キーボード入力チェック ---
    bool keyboardMove =
        CheckHitKey(KEY_INPUT_W) ||
        CheckHitKey(KEY_INPUT_S) ||
        CheckHitKey(KEY_INPUT_A) ||
        CheckHitKey(KEY_INPUT_D);

    // --- コントローラー入力チェック ---
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

    // --- 状態遷移処理 ---
    if (keyboardMove || controllerMove)
    {
        state_ = STATE::MOVE;
    }
}

void Player::DoIdle(void)
{
    // --- コントローラー入力チェック ---
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

    // 攻撃開始（1段目）
    if (!isAttacked_ &&
        (input.IsTrgMouseLeft() ||
            input.IsTrgDown(KEY_INPUT_J) ||
            input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT) ||
            input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER1) ||
            input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER2)))
    {
        conbo_ = CONBO::CONBO1;
        state_ = STATE::ATTACK;

        isAttacked_ = true;
        isUpMuscle_ = true;

        sound.Stop(SOUND::PLAYER_SMALL_ATTACK);
        sound.Play(SOUND::PLAYER_SMALL_ATTACK, false, 255, false, true);

        return;
    }

    int animIndex = (int)ANIM_TYPE::ATTACK1 + (int)conbo_;
    if (animation_->IsPassedRatio(animIndex, 0.4f) &&
        (input.IsTrgMouseLeft() || input.IsTrgDown(KEY_INPUT_J) ||
            input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT)  ||
            input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER1)||
            input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER2)))
    {
        if (conbo_ < CONBO::CONBO3)
        {
            conbo_ = (CONBO)((int)conbo_ + 1);
            state_ = STATE::ATTACK;
            isUpMuscle_ = true;

            sound.Stop(SOUND::PLAYER_BIG_ATTACK);
            sound.Stop(SOUND::PLAYER_SMALL_ATTACK);

            if ((int)conbo_ >= (int)CONBO::MAX - 1) {
                sound.Play(SOUND::PLAYER_BIG_ATTACK, false, 255, false, true);
            }
            else {
                sound.Play(SOUND::PLAYER_SMALL_ATTACK, false, 255, false, true);
            }
        }
    }
}

void Player::DoRoll(void)
{
    auto& input = InputManager::GetInstance();
    if (nextRollCounter_ > 0 || animation_->IsEnd((int)(ANIM_TYPE::ATTACK1))) return;


    // どちらかのキーが押された瞬間にROLLへ
    if (input.IsTrgDown(KEY_INPUT_LSHIFT) || 
        input.IsTrgDown(KEY_INPUT_K) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER1) || 
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER2)) {
        state_ = STATE::ROLL;
    }

}

//void Player::DrawRingGauge(int cx, int cy, int outerR, int innerR, float ratio, int color)
//{
//    float ratioC = Utility::Clamp(ratio, 0.0f, 1.0f);
//    int time = GetNowCount();
//
//    // --- 筋肉の鼓動パルス（少しだけ伸縮）---
//    float pulse = 0.03f * sinf(time * 0.03f);
//    float effectiveRatio = Utility::Clamp(ratioC + pulse, 0.0f, 1.0f);
//
//    // --- 太さ計算 ---
//    float baseThickness = (float)(outerR - innerR);
//
//    // --- 色変化（緑→黄→赤） ---
//    int r, g, b;
//    if (effectiveRatio < 0.5f) {
//        float t = effectiveRatio / 0.5f;
//        r = (int)(0 + 255 * t);
//        g = 255;
//        b = 0;
//    }
//    else {
//        float t = (effectiveRatio - 0.5f) / 0.5f;
//        r = 255;
//        g = (int)(255 - 255 * t);
//        b = 0;
//    }
//    int ringColor = GetColor(r, g, b);
//
//    // --- リング角度（時計回り） ---
//    float endAngle = 360.0f * ratioC;
//
//    // --- MAX時の特別演出 ---
//    if (ratioC >= 0.999f) {
//        float glow = 3.0f * sinf(time * 0.2f) + 5.0f;
//        ringColor = GetColor(255, 200, 150);
//        int auraColor = GetColor(255, 255, 200);
//        DrawCircleAA(cx, cy, outerR + glow * 0.5f, 64, auraColor, FALSE, 4.0f);
//    }
//
//    // --- リング描画 ---
//    const int div = 128;
//    for (int i = 0; i < div; i++) {
//        float angle1 = DX_PI_F * 2.0f * i / div;
//        float angle2 = DX_PI_F * 2.0f * (i + 1) / div;
//        if ((360.0f * i / div) > endAngle) break;
//
//        float inner = (float)innerR;
//        float outer = (float)outerR;
//
//        VECTOR p1 = { cx + cosf(angle1) * inner, cy + sinf(angle1) * inner };
//        VECTOR p2 = { cx + cosf(angle1) * outer, cy + sinf(angle1) * outer };
//        VECTOR p3 = { cx + cosf(angle2) * inner, cy + sinf(angle2) * inner };
//        VECTOR p4 = { cx + cosf(angle2) * outer, cy + sinf(angle2) * outer };
//
//        DrawTriangleAA(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, ringColor, TRUE);
//        DrawTriangleAA(p3.x, p3.y, p2.x, p2.y, p4.x, p4.y, ringColor, TRUE);
//    }
//
//    // --- 中央にパーセント表示 ---
//    char str[16];
//    sprintf_s(str, "%3d%%", (int)(ratioC * 100));
//    int fontSize = 32 + (int)(8 * pulse);
//    int font = CreateFontToHandle("Impact", fontSize, 4);
//    int textColor = (ratioC >= 0.999f) ? GetColor(255, 240, 200) : GetColor(230, 255, 230);
//    int strW = GetDrawStringWidthToHandle(str, strlen(str), font);
//    DrawStringToHandle(cx - strW / 2, cy - fontSize / 2, str, textColor, font);
//    DeleteFontToHandle(font);
//}

int Player::CalcGaugeColor(float ratio) const
{
    int rCol, gCol, bCol;
    if (ratio < 0.5f) {
        float t = ratio / 0.5f;
        rCol = static_cast<int>(255 * t);
        gCol = 255;
        bCol = 0;
    }
    else {
        float t = (ratio - 0.5f) / 0.5f;
        rCol = 255;
        gCol = static_cast<int>(255 * (1.0f - t));
        bCol = 0;
    }
    return GetColor(rCol, gCol, bCol);
}

// =====================================================
// 前処理（パルス補正）
float Player::CalcEffectRatio(float ratio, int time) const
{
    float clamped = Utility::Clamp(ratio, 0.0f, 1.0f);
    float pulse = PULSE_AMPLITUDE * sinf(time * PULSE_SPEED);
    return Utility::Clamp(clamped + pulse, 0.0f, 1.0f);
}

// 背景描画
void Player::DrawGaugeBack(int cx, int cy, float radius) const
{
    DrawCircleAA(cx, cy, radius, GAUGE_SEGMENTS, GetColor(40, 0, 0), TRUE);
}

// ゲージ描画（円弧）
void Player::DrawGaugeRing(int cx, int cy, int innerR, int outerR, float ratio, int color) const
{
    float endAngle = DX_PI_F * 2.0f * ratio;

    for (int i = 0; i < GAUGE_SEGMENTS; i++)
    {
        float a1 = DX_PI_F * 2.0f * i / GAUGE_SEGMENTS;
        float a2 = DX_PI_F * 2.0f * (i + 1) / GAUGE_SEGMENTS;
        if (a1 > endAngle) break;

        VECTOR p1 = { cx + cosf(a1) * innerR, cy + sinf(a1) * innerR };
        VECTOR p2 = { cx + cosf(a1) * outerR, cy + sinf(a1) * outerR };
        VECTOR p3 = { cx + cosf(a2) * innerR, cy + sinf(a2) * innerR };
        VECTOR p4 = { cx + cosf(a2) * outerR, cy + sinf(a2) * outerR };

        DrawTriangleAA(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, TRUE);
        DrawTriangleAA(p3.x, p3.y, p2.x, p2.y, p4.x, p4.y, color, TRUE);
    }
}

// 外枠
void Player::DrawGaugeFrame(int cx, int cy, int innerR, int outerR) const
{
    for (int i = 0; i < 3; i++)
    {
        int frameColor = GetColor(120 + i * 40, 100 + i * 30, 100 + i * 30);
        DrawCircleAA(cx, cy, outerR + i, GAUGE_SEGMENTS, frameColor, FALSE, 2.0f);
        DrawCircleAA(cx, cy, innerR - i, GAUGE_SEGMENTS, frameColor, FALSE, 2.0f);
    }
}

// 最大値演出
void Player::DrawGlowEffect(int cx, int cy, float radius, float& ringThickness) const
{
    int time = GetNowCount();
    float glow = MAX_GLOW_AMPLITUDE * sinf(time * MAX_GLOW_SPEED) + MAX_GLOW_AMPLITUDE;
    ringThickness += glow * 0.3f;

    int auraColor = GetColor(255, 255, 200);
    DrawCircleAA(cx, cy, radius + ringThickness * 0.8f, GAUGE_SEGMENTS, auraColor, FALSE, 3.0f);
}

// パーセント表示
void Player::DrawGaugeText(int cx, int cy, float ratio, float pulse) const
{
    char str[16];
    sprintf_s(str, sizeof(str), "%3d%%", static_cast<int>(ratio * 100));

    int fontSize = FONT_BASE_SIZE + static_cast<int>(PULSE_TEXT_SCALE * pulse);
    int font = CreateFontToHandle("Impact", fontSize, 3);

    int textColor = (ratio >= 1.0f)
        ? GetColor(255, 255, 180)
        : GetColor(230, 255, 230);

    int strW = GetDrawStringWidthToHandle(str, strlen(str), font);
    DrawStringToHandle(cx - strW / 2, cy - fontSize / 2, str, textColor, font);
    DeleteFontToHandle(font);
}

// メイン描画関数
void Player::DrawMuscleGauge(int cx, int cy, int outerR, int innerR, float ratio)
{
    int time = GetNowCount();
    float clampedRatio = Utility::Clamp(ratio, 0.0f, 1.0f);
    float effectiveRatio = CalcEffectRatio(clampedRatio, time);

    float ringThickness = static_cast<float>(outerR - innerR);
    float radius = (outerR + innerR) * 0.5f;

    int ringColor = CalcGaugeColor(effectiveRatio);

    DrawGaugeBack(cx, cy, radius);

    if (clampedRatio >= MAX_RATIO_THRESHOLD)
    {
        DrawGlowEffect(cx, cy, radius, ringThickness);
        ringColor = GetColor(255, 255, 180);
    }

    DrawGaugeRing(cx, cy, innerR, outerR, clampedRatio, ringColor);
    DrawGaugeFrame(cx, cy, innerR, outerR);
    DrawGaugeText(cx, cy, clampedRatio, PULSE_AMPLITUDE * sinf(time * PULSE_SPEED));
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

const float Player::GetMuscleRatio()
{
    MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, LeftArm::LEFT_ARM_INDEX);

    // スケール抽出
    float scale[3];
    for (int i = 0; i < 3; i++) {
        scale[i] = VSize(VGet(mat.m[i][0], mat.m[i][1], mat.m[i][2]));
    }

    // 軸ごとにratio化
    float scaleRatio[3];
    scaleRatio[0] = Utility::Clamp((scale[0] - MIN_MUSCLE.x) / (MAX_MUSCLE.x - MIN_MUSCLE.x), 0.0f, 1.0f);
    scaleRatio[1] = Utility::Clamp((scale[1] - MIN_MUSCLE.y) / (MAX_MUSCLE.y - MIN_MUSCLE.y), 0.0f, 1.0f);
    scaleRatio[2] = Utility::Clamp((scale[2] - MIN_MUSCLE.z) / (MAX_MUSCLE.z - MIN_MUSCLE.z), 0.0f, 1.0f);

    float muscleRatio = (scaleRatio[0] + scaleRatio[1] + scaleRatio[2]) / 3.0f;

    return muscleRatio;
}

void Player::DebugDraw(void)
{
    auto& input = InputManager::GetInstance();
    //デバック用Draw

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

    int frameNum = MV1GetFrameNum(unit_.model_);

    if (input.IsTrgDown(KEY_INPUT_UP))
    {
        frameScrollIndex_--;
        if (frameScrollIndex_ < 0) frameScrollIndex_ = 0;
    }

    if (input.IsTrgDown(KEY_INPUT_DOWN))
    {
        frameScrollIndex_++;
        if (frameScrollIndex_ > frameNum - 1) frameScrollIndex_ = frameNum - 1;
    }

    VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
    VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
    DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, 0x0f0f0f, 0x0f0f0f, false);

    // ===== 画面に描画 =====
    int y = 200;
    const int maxLines = 20; // 一度に表示する行数

    for (int i = 0; i < maxLines; i++)
    {
        int idx = frameScrollIndex_ + i;
        if (idx >= frameNum) break;

        const char* name = MV1GetFrameName(unit_.model_, idx);
        DrawFormatString(0, y, GetColor(255, 255, 255),
            "Frame %d : %s", idx, name ? name : "(null)");
        y += 16;
    }


}

void Player::DrawPlayer(void)
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

    // スケールを先に適用
    mat = MMult(MGetScale(unit_.scale_), mat);

    VECTOR worldPos = VTransform(ofset, mat);

    mat.m[3][0] = unit_.pos_.x + worldPos.x;
    mat.m[3][1] = unit_.pos_.y + worldPos.y;
    mat.m[3][2] = unit_.pos_.z + worldPos.z;

    // 行列の設定
    MV1SetMatrix(unit_.model_, mat);

    int matNum = MV1GetMaterialNum(modelHandle);
    for (int i = 0; i < matNum; i++)
    {
        MATERIALPARAM mat;
        MV1GetMaterialParam(modelHandle, i, &mat);

        // --- Diffuse（基本色）を1.5倍して上限を1.0にクランプ ---
        mat.Diffuse.r = min(mat.Diffuse.r * 1.5f, 1.0f);
        mat.Diffuse.g = min(mat.Diffuse.g * 1.5f, 1.0f);
        mat.Diffuse.b = min(mat.Diffuse.b * 1.5f, 1.0f);

        // --- Ambient（環境光）も上げると暗部が持ち上がる ---
        mat.Ambient.r = min(mat.Ambient.r * 1.5f, 1.0f);
        mat.Ambient.g = min(mat.Ambient.g * 1.5f, 1.0f);
        mat.Ambient.b = min(mat.Ambient.b * 1.5f, 1.0f);

        MV1SetMaterialParam(&mat);
    }

    if (unit_.inviciCounter_ > 0)
    {
        float t = sinf(GetNowCount() * 0.03f);
        float intensity = (t > 0.0f) ? 1.0f : 0.1f;

        MV1SetDifColorScale(unit_.model_, { 1.0f, intensity * 0.5f, intensity * 0.5f, 1.0f });
    }
    else
    {
        MV1SetDifColorScale(unit_.model_, { 1.0f, 1.0f, 1.0f, 1.0f });
    }

    // モデル描画
    MV1DrawModel(unit_.model_);
}

void Player::HpDraw(void)
{
    float ratio = unit_.hp_ / 100.0f; // 100 が最大HPの場合
    // ===== 筋肉ゲージ（外装強化）=====
    int startPosX = 200, startPosY = 100;               // 表示位置
    int width = 600, height = 50;       // サイズ
    int filled = (int)(width * ratio);

    // ▼ 背面の影（奥行き）
    DrawBox(startPosX - 3, startPosY - 3, startPosX + width + 3, startPosY + height + 3, GetColor(30, 0, 0), true);

    // ▼ 外枠（太め・メタル調）
    for (int i = 0; i < 3; i++) {
        DrawBox(startPosX - i, startPosY - i, startPosX + width + i, startPosY + height + i, GetColor(150 + i * 30, 30 + i * 10, 30 + i * 10), false);
    }

    // ▼ ゲージ背景（深赤）
    DrawBox(startPosX, startPosY, startPosX + width, startPosY + height, GetColor(60, 0, 0), true);

    // ▼ 筋繊維ライン
    float t = (float)GetNowCount() / 100.0f;
    for (int i = 0; i < height; i += 4)
    {
        int strength = (int)(128 + 127 * sin(i * 0.5f + t));
        DrawLine(startPosX, startPosY + i, startPosX + filled, startPosY + i, GetColor(180 + strength / 4, 30, 30));
    }

    // ▼ 赤→黄グラデーション
    for (int x = 0; x < filled; x++)
    {
        float f = (float)x / filled;
        int r = 0;
        int g = 255;
        int b = (int)(f * 180);
        DrawLine(startPosX + x, startPosY, startPosX + x, startPosY + height, GetColor(r, g, b));
    }

    // ▼ 上部ハイライト
    DrawLine(startPosX, startPosY + 2, startPosX + filled, startPosY + 2, GetColor(255, 180, 180));

    // ▼ 下部に影（立体感）
    DrawLine(startPosX, startPosY + height - 1, startPosX + width, startPosY + height - 1, GetColor(40, 0, 0));

    // ▼ 外光オーラ（力の気配）
    int auraColor = GetColor(255, 80, 80);
    DrawBox(startPosX - 5, startPosY - 5, startPosX + filled + 5, startPosY + height + 5, auraColor, false);
}


