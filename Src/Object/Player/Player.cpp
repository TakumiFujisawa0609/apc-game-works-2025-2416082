#include "Player.h"

#include "../../Manager/Animation/AnimationController.h"
#include "../../Manager/Camera/Camera.h"
#include "../../Manager/Input/InputManager.h"   


Player::Player()
{
}

Player::~Player()
{
}

void Player::Load(void)
{
   std::string path = "Data/Model/Player/";

    // モデルのロード
    unit_.model_ = MV1LoadModel((path + "Player2.mv1").c_str());

    // アニメーションクラス
    animation_ = new AnimationController(unit_.model_);

    // アニメーションのロード
    animation_->Add((int)(ANIM_TYPE::IDLE), 30.0f, (path + "Animation/Idle.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::RUN), 50.0f, (path + "Animation/Run.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::Roll), 100.0f, (path + "Animation/Evasion.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK1), 100.0f, (path + "Animation/Punching.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK2), 100.0f, (path + "Animation/Punching2.mv1").c_str());
    animation_->Add((int)(ANIM_TYPE::ATTACK3), 100.0f, (path + "Animation/Swiping.mv1").c_str());

    move_ = VGet(0.0f, 0.0f, 0.0f);

}

void Player::Init(void)
{
    // 変数の初期化
    unit_.isAlive_ = true;
	unit_.pos_ = DEFAULT_POS;
    unit_.angle_ = Utility::VECTOR_ZERO;

    currentHeight = Camera::CAMERA_PLAYER_POS;
    cameraPos_ = Utility::VECTOR_ZERO;

    nextRollCounter_ = 0;

    frameScrollIndex_ = 0;

    attacConboCnt_ = 0;

    // 関数ポインタに登録
    stateFuncs_ =
    {
        { STATE::IDLE,      &Player::Idle   },
        { STATE::MOVE,      &Player::Move   },
        { STATE::ATTACK,    &Player::Attack },
        { STATE::ROLL,      &Player::Roll   },
    };
    
    state_ = STATE::IDLE;
}

void Player::Update(void)
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
}

void Player::Draw(void)
{
	if (!unit_.isAlive_)return;

    //for(int i = 0;)

    MV1SetMatrix(unit_.model_, MatrixSet());

    MV1DrawModel(unit_.model_);

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

    // モデルの解放
    MV1DeleteModel(unit_.model_);
}

//当たり判定
void Player::OnCollision(UnitBase* other)
{
}

// 筋肉処理
void Player::Muscle(void)
{
    static int cnt = 0;

  /*  if (state_ != STATE::ATTACK)cnt = 0;
    if (isAttacked_) {
        cnt++;
        if (cnt < 10) {
            BoneScaleChange(LEFT_ARM, UP_MUSCLE);
            BoneScaleChange(RIGHT_ARM, UP_MUSCLE);
        }
    }*/

    if (CheckHitKey(KEY_INPUT_0))
    {
        BoneScaleChange(LEFT_ARM, { -1.0f,-1.0f,-1.0f });
        BoneScaleChange(RIGHT_ARM, { -1.0f,-1.0f,-1.0f });
    }

    BoneScaleChange(RIGHT_ARM, DOWN_MUSCLE);
    BoneScaleChange(LEFT_ARM, DOWN_MUSCLE);
}

// どこのボーンかを見て、そのボーンのスケールに引数のscaleを加算する
void Player::BoneScaleChange(int index, VECTOR scale)
{
    MATRIX mat = MV1GetFrameLocalMatrix(unit_.model_, index);

    // 行列からスケール成分を抽出
    VECTOR currentScale;
    currentScale.x = VSize(VGet(mat.m[0][0], mat.m[0][1], mat.m[0][2]));
    currentScale.y = VSize(VGet(mat.m[1][0], mat.m[1][1], mat.m[1][2]));
    currentScale.z = VSize(VGet(mat.m[2][0], mat.m[2][1], mat.m[2][2]));

    // スケール加算
    VECTOR newScale = {
        currentScale.x + scale.x,
        currentScale.y + scale.y,
        currentScale.z + scale.z
    };

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

    // 共通処理（攻撃中は腕を強調）
    BoneScaleChange(LEFT_ARM, UP_MUSCLE);
    BoneScaleChange(RIGHT_ARM, UP_MUSCLE);

    switch (attacConboCnt_)
    {
    case 1: //1段目
        animation_->Play((int)ANIM_TYPE::ATTACK1, false);

        // 7割を超えていて、入力があれば次へ
        if (animation_->IsPassedRatio((int)ANIM_TYPE::ATTACK1, 0.7f) &&
            (input.IsTrgDown(KEY_INPUT_J) || input.IsTrgMouseLeft()))
        {
            attacConboCnt_ = 2;
            break;
        }

        // アニメーションが終わったら終了
        if (animation_->IsPassedRatio((int)ANIM_TYPE::ATTACK1, 1.0f))
        {
            attacConboCnt_ = 1;
            state_ = STATE::IDLE;
        }
        break;

    case 2: //2段目
        // 1段目がある程度進んでいたら2段目を再生
        if (animation_->IsPassedRatio((int)ANIM_TYPE::ATTACK1, 0.7f)) {
            animation_->Play((int)ANIM_TYPE::ATTACK2, false);
        }

        // 7割を超えていて、入力があれば次へ
        if (animation_->IsPassedRatio((int)ANIM_TYPE::ATTACK2, 0.7f) &&
            (input.IsTrgDown(KEY_INPUT_J) || input.IsTrgMouseLeft()))
        {
            attacConboCnt_ = 3;
            break;
        }

        // アニメーションが終わったら終了
        if (animation_->IsPassedRatio((int)ANIM_TYPE::ATTACK2, 1.0f))
        {
            attacConboCnt_ = 1;
            state_ = STATE::IDLE;
        }
        break;

    case 3: //3段目
        if (animation_->IsPassedRatio((int)ANIM_TYPE::ATTACK2, 0.7f)) {
            animation_->Play((int)ANIM_TYPE::ATTACK3, false);
        }

        // 3段目は追加コンボなし → 終了判定だけ
        if (animation_->IsEnd((int)ANIM_TYPE::ATTACK3))
        {
            attacConboCnt_ = 1;
            state_ = STATE::IDLE;
        }
        break;

    default:
        attacConboCnt_ = 1;
        break;
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
        DoWalk();
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

    // Jキー or マウス左クリックを押した瞬間だけ攻撃に移る
    if (input.IsTrgMouseLeft() || input.IsTrgDown(KEY_INPUT_J)) {
        state_ = STATE::ATTACK;
    }

    // 攻撃処理の初期化
    isAttacked_ = false;
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

    // ===== 筋肉ゲージ描画 =====
    int mx = 50, my = 50; // 表示位置
    int width = 200, height = 20; // ゲージのサイズ
    int filled = (int)(width * muscleRatio_);

    // 外枠
    DrawBox(mx, my, mx + width, my + height, GetColor(255, 255, 255), false);
    // 中身（割合に応じて伸ばす）
    DrawBox(mx, my, mx + filled, my + height, GetColor(255, 0, 0), true);

    int frameNum = MV1GetFrameNum(unit_.model_);

    static int prevUp = 0, prevDown = 0;
    int nowUp = CheckHitKey(KEY_INPUT_UP);
    int nowDown = CheckHitKey(KEY_INPUT_DOWN);

    if (nowUp == 1 && prevUp == 0)
    {
        frameScrollIndex_--;
        if (frameScrollIndex_ < 0) frameScrollIndex_ = 0;
    }

    if (nowDown == 1 && prevDown == 0)
    {
        frameScrollIndex_++;
        if (frameScrollIndex_ > frameNum - 1) frameScrollIndex_ = frameNum - 1;
    }

    prevUp = nowUp;
    prevDown = nowDown;

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

MATRIX Player::MatrixSet(void)
{
    // 回転行列の作成　
    MATRIX mat = MGetIdent();
    mat = MMult(mat, MGetRotX(unit_.angle_.x));
    mat = MMult(mat, MGetRotY(unit_.angle_.y));
    mat = MMult(mat, MGetRotZ(unit_.angle_.z));

    MATRIX localMat = MGetIdent();
    localMat = MMult(localMat, MGetRotX(LOCAL_ANGLE.x));
    localMat = MMult(localMat, MGetRotY(LOCAL_ANGLE.y));
    localMat = MMult(localMat, MGetRotZ(LOCAL_ANGLE.z));

    mat = MMult(localMat, mat);

    // スケール行列
    MATRIX matScale = MGetScale(unit_.scale_);

    // スケールを先に適用
    mat = MMult(matScale, mat);

    mat.m[3][0] = unit_.pos_.x;
    mat.m[3][1] = unit_.pos_.y;
    mat.m[3][2] = unit_.pos_.z;

    return mat;
}

