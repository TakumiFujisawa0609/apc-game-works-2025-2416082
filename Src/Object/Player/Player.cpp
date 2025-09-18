#include "Player.h"

#include "../../Manager/Animation/AnimationController.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Load(void)
{

   std::string path = "Data/Model/Player/";
    unit_.model_ = MV1LoadModel((path + "Player.mv1").c_str());

    animation_ = new AnimationController(unit_.model_);

    animation_->Add((int)(STATE::IDLE), 30.0f, "Data/Model/Player/Animation/Idle.mv1");
    animation_->Add((int)(STATE::MOVE), 50.0f, "Data/Model/Player/Animation/Run.mv1");
    animation_->Add((int)(STATE::ATTACK), 30.0f, "Data/Model/Player/Animation/Throw.mv1");

    move_ = VGet(0.0f, 0.0f, 0.0f);

}

void Player::Init(void)
{
	unit_.isAlive_ = true;
	unit_.pos_ = DEFAULT_POS;
    unit_.angle_ = Utility::VECTOR_ZERO;


    stateFuncs_ =
    {
        { STATE::IDLE,      &Player::Idle   },
        { STATE::MOVE,      &Player::Move   },
        { STATE::ATTACK,    &Player::Attack },
    };
    
    state_ = STATE::IDLE;

}

void Player::Update(void)
{

    StateManager();

    auto Func = stateFuncs_.find(state_);
    (this->*(Func->second))();

    animation_->Update();

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
}

// 何もしていない
void Player::Idle(void)
{
    animation_->Play((int)STATE::IDLE, true);

}

// 移動処理
void Player::Move(void)
{
    move_ = Utility::VECTOR_ZERO;

    if (CheckHitKey(KEY_INPUT_W)) move_.z += 1.0f;
    if (CheckHitKey(KEY_INPUT_S)) move_.z -= 1.0f;
    if (CheckHitKey(KEY_INPUT_A)) move_.x -= 1.0f;
    if (CheckHitKey(KEY_INPUT_D)) move_.x += 1.0f;

    bool isRun = false;
    if (CheckHitKey(KEY_INPUT_LSHIFT))isRun = true;

    if (move_.x != 0.0f || move_.z != 0.0f)
    {
        move_ = VNorm(move_);
        move_ = VScale(move_, MOVE_SPEED);

        unit_.pos_ = VAdd(unit_.pos_, move_);
        unit_.angle_.y = atan2f(move_.x, move_.z);
    }

    animation_->Play((int)STATE::MOVE, true);
}

void Player::Attack(void)
{
    animation_->Play((int)(STATE::ATTACK), false);

    // 攻撃中でまだスケールを増やしていないなら一度だけ増やす
    if (!attackScaleApplied_) {
        unit_.scale_.x *= 1.10f;
        unit_.scale_.y *= 1.10f;
        unit_.scale_.z *= 1.10f;

        const float MAX_SCALE = 5.0f;
        if (unit_.scale_.x > MAX_SCALE) unit_.scale_ = VGet(MAX_SCALE, MAX_SCALE, MAX_SCALE);

        attackScaleApplied_ = true; // 二度目は増えないようにする
    }
}

void Player::StateManager(void)
{
    switch (state_)
    {
    case Player::STATE::IDLE:
        DoWalk();
        DoAttack();
        break;
    case Player::STATE::MOVE:
        DoIdle();
        DoAttack();
        break;
    case Player::STATE::ATTACK:
        if (animation_->IsEnd((int)(STATE::ATTACK)))
        {
            state_ = STATE::IDLE;
        }
        break;
    }
}

void Player::DoWalk(void)
{
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
    prevSpace = nowSpace;
    nowSpace = CheckHitKey(KEY_INPUT_K);

    attackScaleApplied_ = false;
    if (nowSpace == 1 && prevSpace == 0)state_ = STATE::ATTACK;
}
