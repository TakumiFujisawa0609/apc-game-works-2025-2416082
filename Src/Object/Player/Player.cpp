#include "Player.h"

#include "../../Manager/Animation/AnimationController.h"
#include "../../Manager/Input/InputManager.h"   
#include "../../Manager/Sound/SoundManager.h"

#include "../Camera/Camera.h"

#include "../Boss/Boss.h"

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
   unit_.model_ = MV1LoadModel((path + "Player2.mv1").c_str());

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
    animation_->Add((int)(ANIM_TYPE::Roll), 100.0f, (path + "Animation/Evasion.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK1), 100.0f, (path + "Animation/Punching.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK2), 100.0f, (path + "Animation/Punching2.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK3), 130.0f, (path + "Animation/Swiping.mv1").c_str());

    // 音声のロード
    SoundManager::GetIns().Load(SOUND::PLAYER_BIG_ATTACK);
    SoundManager::GetIns().Load(SOUND::PLAYER_SMALL_ATTACK);
}

void Player::Init(void)
{
    unit_.para_.colliShape = CollisionShape::CAPSULE;
    unit_.para_.colliType = CollisionType::ALLY;

    // 当たり判定するための変数
    unit_.para_.capsuleHalfLen = CAPSULE_HALF_LENGTH; // カプセルの円から円までの長さの半分
    unit_.para_.radius = RADIUS_SIZE;                 // 半径の長さ

    // 変数の初期化
    unit_.isAlive_ = true;                  // プレイヤーの生存フラグ
	unit_.pos_ = DEFAULT_POS;               // プレイヤーの座標
    unit_.angle_ = Utility::VECTOR_ZERO;    // プレイヤーの向き・アングル

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

    DrawPlayer();

    // 腕に関する描画処理
    leftArm_->Draw();
    rightArm_->Draw();

#ifdef _DEBUG
    DebugDraw();
#endif 
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

//当たり判定
void Player::OnCollision(UnitBase* other)
{
    if (unit_.inviciCounter_ > 0) { return; }

    if (dynamic_cast<Boss*>(other))
    {
        if (state_ == STATE::ROLL) {
            AddArmScale(UP_MUSCLE[2]);
            return;
        }
        //AddArmScale({ -0.2f,-0.2f,-0.2f });

        unit_.inviciCounter_ = INVI_TIME;
        return;
    }
}

// 筋肉処理
void Player::Muscle(void)
{
    static int cnt = 0;

    AddArmScale(DOWN_MUSCLE);

#ifdef _DEBUG
    if (CheckHitKey(KEY_INPUT_0))
    {
        AddArmScale({ -1.0f,-1.0f,-1.0f });
    }
#endif // _DEBUG

    if (state_ != STATE::ATTACK)
    {
        cnt = 0;
        isUpMuscle_ = false;
        return;
    }

    if (isUpMuscle_)
    {
        cnt++;
        if (cnt <= 10)
        {
            AddArmScale(UP_MUSCLE[(int)conbo_]);
        }
        else
        {
            cnt = 0;
            isUpMuscle_ = false;
        }
    }
}

void Player::MuscleDraw(void)
{
    // ===== 筋肉ゲージ（外装強化）=====
    int mx = 50, my = 50;               // 表示位置
    int width = 600, height = 50;       // サイズ
    int filled = (int)(width * muscleRatio_);

    // ▼ 背面の影（奥行き）
    DrawBox(mx - 3, my - 3, mx + width + 3, my + height + 3, GetColor(30, 0, 0), true);

    // ▼ 外枠（太め・メタル調）
    for (int i = 0; i < 3; i++) {
        DrawBox(mx - i, my - i, mx + width + i, my + height + i, GetColor(150 + i * 30, 30 + i * 10, 30 + i * 10), false);
    }

    // ▼ ゲージ背景（深赤）
    DrawBox(mx, my, mx + width, my + height, GetColor(60, 0, 0), true);

    // ▼ 筋繊維ライン
    float t = (float)GetNowCount() / 100.0f;
    for (int i = 0; i < height; i += 4)
    {
        int strength = (int)(128 + 127 * sin(i * 0.5f + t));
        DrawLine(mx, my + i, mx + filled, my + i, GetColor(180 + strength / 4, 30, 30));
    }

    // ▼ 赤→黄グラデーション
    for (int x = 0; x < filled; x++)
    {
        float f = (float)x / filled;
        int r = 255;
        int g = (int)(f * 180);
        int b = 0;
        DrawLine(mx + x, my, mx + x, my + height, GetColor(r, g, b));
    }

    // ▼ 上部ハイライト
    DrawLine(mx, my + 2, mx + filled, my + 2, GetColor(255, 180, 180));

    // ▼ 下部に影（立体感）
    DrawLine(mx, my + height - 1, mx + width, my + height - 1, GetColor(40, 0, 0));

    // ▼ 外光オーラ（力の気配）
    int auraColor = GetColor(255, 80, 80);
    DrawBox(mx - 5, my - 5, mx + filled + 5, my + height + 5, auraColor, false);
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

    // 入力方向ベクトル（カメラ基準のローカル座標）
    if (CheckHitKey(KEY_INPUT_W)) { move_ = VAdd(move_, { 0.0f, 0.0f,  1.0f }); }
    if (CheckHitKey(KEY_INPUT_S)) { move_ = VAdd(move_, { 0.0f, 0.0f, -1.0f }); }
    if (CheckHitKey(KEY_INPUT_A)) { move_ = VAdd(move_, { -1.0f, 0.0f, 0.0f }); }
    if (CheckHitKey(KEY_INPUT_D)) { move_ = VAdd(move_, { 1.0f, 0.0f, 0.0f  }); }

    if (move_.x != 0.0f || move_.z != 0.0f)
    {
        // カメラの向きから回転行列を作る
        MATRIX mat = MGetRotY(camera.GetAngle().y * DX_PI_F / 180.0f);

        // カメラ基準の方向をワールド基準に変換
        VECTOR worldMove = VTransform(move_, mat);

        // 正規化＋スケーリング
        worldMove = VNorm(worldMove);
        worldMove = VScale(worldMove, MOVE_SPEED);

        unit_.pos_ = VAdd(unit_.pos_, worldMove);

        // プレイヤーの向きも移動方向に合わせる
        float targetY = atan2f(worldMove.x, worldMove.z);

        unit_.angle_.y = Utility::LerpAngle(unit_.angle_.y, targetY, 0.5f);
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

    if (animation_->IsPassedRatio(anim, 0.1f) && !animation_->IsPassedRatio(anim, 0.7f))
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
        animation_->Play((int)ANIM_TYPE::Roll, false);

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
    if (CheckHitKey(KEY_INPUT_W)) { move_ = { 0.0f, 0.0f, 1.0f  }; }
    if (CheckHitKey(KEY_INPUT_S)) { move_ = { 0.0f, 0.0f, -1.0f }; }
    if (CheckHitKey(KEY_INPUT_A)) { move_ = { -1.0f, 0.0f, 0.0f }; }
    if (CheckHitKey(KEY_INPUT_D)) { move_ = { 1.0f, 0.0f, 0.0f  }; }
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
    // 移動入力があったらMOVEに移行する
    if (CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_S) ||
        CheckHitKey(KEY_INPUT_A) || CheckHitKey(KEY_INPUT_D))
    {
        state_ = STATE::MOVE;
    }
}

void Player::DoIdle(void)
{
    // 移動入力がなくなったらIDLEに戻る
    if (!(CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_S) ||
        CheckHitKey(KEY_INPUT_A) || CheckHitKey(KEY_INPUT_D)))
    {
        state_ = STATE::IDLE;
    }
}

void Player::DoAttack(void)
{
    auto& input = InputManager::GetInstance();
    auto& sound = SoundManager::GetIns();

    // 攻撃開始（1段目）
    if (!isAttacked_ && (input.IsTrgMouseLeft() || input.IsTrgDown(KEY_INPUT_J)))
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
    if (animation_->IsPassedRatio(animIndex, 0.5f) &&
        (input.IsTrgMouseLeft() || input.IsTrgDown(KEY_INPUT_J)))
    {
        // 次の段階がある場合のみ進める
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
    if (input.IsTrgDown(KEY_INPUT_LSHIFT) || input.IsTrgDown(KEY_INPUT_K)) {
        state_ = STATE::ROLL;
    }

}

// カメラが向く方向の処理
void Player::CameraPosUpdate(void)
{
    //もともとボーンごとじゃなくてモデル自体を
    //大きくしていたのでそれに応じてカメラの位置も高くなるようにしていた
    cameraPos_ = unit_.pos_;

    float scaleAvg = (unit_.scale_.x + unit_.scale_.y + unit_.scale_.z) / 3.0f;
    float targetHeight = Camera::CAMERA_PLAYER_POS * scaleAvg;

    currentHeight += (targetHeight - currentHeight) * 0.2f;

    cameraPos_.y = unit_.pos_.y + currentHeight;
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

    MuscleDraw();



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

