#include "Player.h"

#include "../../Manager/Animation/AnimationController.h"
#include "../../Manager/Camera/Camera.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Load(void)
{

   std::string path = "Data/Model/Player/";
    unit_.model_ = MV1LoadModel((path + "Player1.mv1").c_str());

    animation_ = new AnimationController(unit_.model_);

    animation_->Add((int)(ANIM_TYPE::IDLE), 30.0f, "Data/Model/Player/Animation/Idle.mv1");
    animation_->Add((int)(ANIM_TYPE::RUN), 50.0f, "Data/Model/Player/Animation/Run.mv1");
    animation_->Add((int)(ANIM_TYPE::ATTACK), 30.0f, "Data/Model/Player/Animation/Throw.mv1");
    animation_->Add((int)(ANIM_TYPE::Roll), 100.0f, "Data/Model/Player/Animation/Evasion.mv1");

    move_ = VGet(0.0f, 0.0f, 0.0f);

}

void Player::Init(void)
{
	unit_.isAlive_ = true;
	unit_.pos_ = DEFAULT_POS;
    unit_.angle_ = Utility::VECTOR_ZERO;

    currentHeight = Camera::CAMERA_PLAYER_POS;

    cameraPos_ = Utility::VECTOR_ZERO;

    nextRollCounter_ = 0;

    stateFuncs_ =
    {
        { STATE::IDLE,      &Player::Idle   },
        { STATE::MOVE,      &Player::Move   },
        { STATE::ATTACK,    &Player::Attack },
        { STATE::ROLL,      &Player::Roll },
    };
    
    state_ = STATE::IDLE;
    animType_ = ANIM_TYPE::IDLE;
}

void Player::Update(void)
{
    CameraPosUpdate();

    if (nextRollCounter_ <= 0)
    {
        nextRollCounter_ = 0;
    }
    else
    {
        nextRollCounter_--;
    }

    StateManager();

    auto Func = stateFuncs_.find(state_);
    (this->*(Func->second))();

    animation_->Update();

    //マッスル関係の処理用関数
    Muscle();

    // プレイヤーの無敵処理
	Invi();
}

void Player::Draw(void)
{
	if (!unit_.isAlive_)return;

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

    MV1SetMatrix(unit_.model_, mat);

    MV1DrawModel(unit_.model_);

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

}

void Player::Release(void)
{
    if (animation_)
    {
        animation_->Release();
        delete animation_;
        animation_ = nullptr;
    }

    MV1DeleteModel(unit_.model_);
}

void Player::OnCollision(UnitBase* other)
{
}

void Player::Muscle(void)
{
    static int cnt = 0;

    if (state_ != STATE::ATTACK)cnt = 0;
    if (attackScaleApplied_) {
        cnt++;
        if (cnt < 10) {
            unit_.scale_ = VAdd(unit_.scale_, { 0.02f, 0.02f, 0.02f });
        }

        const float MAX_SCALE = 5.0f;
        if (unit_.scale_.x > MAX_SCALE) unit_.scale_ = VGet(MAX_SCALE, MAX_SCALE, MAX_SCALE);
    }

    if (CheckHitKey(KEY_INPUT_0))
    {
        unit_.scale_ = VAdd(unit_.scale_, { -0.02f, -0.02f, -0.02f });
        if (unit_.scale_.x < 1.0f)unit_.scale_ = VGet(1.0f, 1.0f, 1.0f);
    };
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
        unit_.angle_.y = atan2f(worldMove.x, worldMove.z);
    }

    animation_->Play((int)ANIM_TYPE::RUN, true);
}

void Player::Attack(void)
{
    if (animation_->IsEnd((int)(STATE::ATTACK)))
    {
        state_ = STATE::IDLE;
        return;
    }

    animation_->Play((int)(ANIM_TYPE::ATTACK), false);
    attackScaleApplied_ = true; 
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
        if (animation_->IsPassedRatio((int)(ANIM_TYPE::ATTACK), 0.7f))
        {
            DoWalk();
            DoRoll();
        }
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
    static int  prevJ = 0;
    static int nowJ = 0;
    prevJ = nowJ;
    nowJ = CheckHitKey(KEY_INPUT_J);
    // Jキーが押されたら攻撃に移る
    if (nowJ == 1 && prevJ == 0)state_ = STATE::ATTACK;

    // 攻撃処理の初期化
    attackScaleApplied_ = false;
}

void Player::DoRoll(void)
{
    if (nextRollCounter_ > 0 || animation_->IsEnd((int)(ANIM_TYPE::ATTACK))) return;

    // staticにして毎フレーム値を保持する
    static int prevK = 0, prevShift = 0;

    int nowK = CheckHitKey(KEY_INPUT_K);
    int nowShift = CheckHitKey(KEY_INPUT_LSHIFT);

    // どちらかのキーが押された瞬間にROLLへ
    if ((nowK == 1 && prevK == 0) || (nowShift == 1 && prevShift == 0)) {
        state_ = STATE::ROLL;
    }

    // 前フレームの状態を更新
    prevK = nowK;
    prevShift = nowShift;
}

// カメラが向く方向の処理
void Player::CameraPosUpdate(void)
{
    cameraPos_ = unit_.pos_;

    // 補間用static変数

    // プレイヤーのスケールに応じた目標高さ
    float scaleAvg = (unit_.scale_.x + unit_.scale_.y + unit_.scale_.z) / 3.0f;
    float targetHeight = Camera::CAMERA_PLAYER_POS * scaleAvg;

    // 線形補間（0.2は追従速度）
    currentHeight += (targetHeight - currentHeight) * 0.2f;

    cameraPos_.y = unit_.pos_.y + currentHeight;
}

