#include "Player.h"
#include <cmath>

#include "../../Application/Application.h"

#include "../../Manager/Animation/AnimationController.h"
#include "../../Manager/Input/InputManager.h"   
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/MicInput/MicInput.h"

#include "../Camera/Camera.h"

#include "../Boss/Boss.h"
#include "../Boss/Hand/BossRightHand.h"

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

void Player::SubLoad(void)
{
   // パスの省略
   std::string path = "Data/Model/Player/";

   // モデルのロード
   unit_.model_ = MV1LoadModel((path + "Player1.mv1").c_str());

#pragma region クラスの定義

   // アニメーションクラス
    animation_ = new AnimationController(unit_.model_);

    mic_ = new MicInput();
    mic_->Init();
    mic_->Start();

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

void Player::SubInit(void)
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

    // 三段攻撃のカウンタ変数
    attacConboCnt_ = 0;

    // 向き
    move_ = Utility::VECTOR_ZERO;

    // 攻撃処理の初期化
    isAttacked_ = false;

    // 筋肉を増やす
    isUpMuscle_ = false;

    // 関数ポインタに登録
    StateAdd((int)STATE::IDLE,      [this]() { Idle();      });
    StateAdd((int)STATE::ATTACK,    [this]() { Attack();    });
    StateAdd((int)STATE::MOVE,      [this]() { Move();      });
    StateAdd((int)STATE::ROLL,      [this]() { Roll();      });
    StateAdd((int)STATE::DEATH,     [this]() { Death();     });

    state_ = STATE::IDLE;
    conbo_ = CONBO::CONBO1;

    // 腕の初期化
    leftArm_->Init();
    rightArm_->Init();
}

void Player::SubUpdate(void)
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

    //HPがなくなったら死亡処理に移行
    if (unit_.hp_ <= 0) {
        unit_.hp_ = 0;
        state_ = STATE::DEATH;
    }

    // 関数ポインタでそれぞれのステートの
    // アップデート関数を呼び出している
    int key = static_cast<int>(state_);
    auto it = stateFuncs_.find(key);

    // 安全確認
    if (it != stateFuncs_.end() && it->second) {        
        it->second();
    }
     
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

void Player::SubDraw(void)
{
	if (!unit_.isAlive_)return;

    // プレイヤーの描画
    SetMatrix();

    // 腕に関する描画処理
    leftArm_->Draw();
    rightArm_->Draw();

    int volume = mic_->GetLevel();
    DrawFormatString(0, 0, 0xffffff, "入力された音量(%i)", volume);
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

    if (dynamic_cast<BossRightHand*>(other))
    {

        unit_.hp_ -= 10;
        unit_.inviciCounter_ = INVI_TIME;
    }
}

void Player::UIDraw(void)
{
    //HP描画
    HpDraw();
    rightArm_->UIDraw();

#ifdef _DEBUG
    DebugDraw();

    // 現在の筋肉の割合（ratio）
    DrawFormatString(0, Application::SCREEN_SIZE_Y - 16, 0xffffff, "%f", GetMuscleRatio(LeftArm::LEFT_ARM_INDEX));
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
        const float deadZone = 8000.0f;   // デッドゾーン
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

const float Player::GetMuscleRatio(int index)
{
    // 左右腕のローカル行列取得
    MATRIX leftMat = MV1GetFrameLocalMatrix(unit_.model_, index);

    // 太さ方向(Y軸)のスケールのみ抽出
    float scale = VSize({ leftMat.m[1][0], leftMat.m[1][1], leftMat.m[1][2] });

    // ratio計算
    float ret = Utility::Clamp(
        (scale - ArmBase::MIN_ARM_MUSCLE.y) /
        (ArmBase::MAX_ARM_MUSCLE.x - ArmBase::MIN_ARM_MUSCLE.x),
        0.0f, 1.0f);

    return ret;
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

    VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
    VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
    DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, 0x0f0f0f, 0x0f0f0f, false);
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

    // スケールを先に適用
    mat = MMult(MGetScale(unit_.scale_), mat);

    VECTOR worldPos = VTransform(ofset, mat);

    mat.m[3][0] = unit_.pos_.x + worldPos.x;
    mat.m[3][1] = unit_.pos_.y + worldPos.y;
    mat.m[3][2] = unit_.pos_.z + worldPos.z;

    // 行列の設定
    MV1SetMatrix(unit_.model_, mat);
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


