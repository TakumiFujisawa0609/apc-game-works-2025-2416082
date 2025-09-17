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
    unit_.model_ = MV1LoadModel("Data/Model/Player/Player.mv1");

    animation_ = new AnimationController(unit_.model_);
    for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++)
    {
        animation_->AddInFbx(i, 30.0f, i);
    }
}

void Player::Init(void)
{
	unit_.isAlive_ = true;
	unit_.pos_ = DEFAULT_POS;
    unit_.scale_ = SMALL_MUSCLE;


    stateFuncs_ =
    {
        { STATE::IDLE,  &Player::Idle },
        { STATE::MOVE,  &Player::Move }
    };
    
    state_ = STATE::MOVE;
    animType_ = ANIM_TYPE::MOVE;
    muscleStat_ = MUSCLE::SMALL;

}

void Player::Update(void)
{
    if (CheckHitKey(KEY_INPUT_1))muscleStat_ = MUSCLE::SMALL;
    if (CheckHitKey(KEY_INPUT_2))muscleStat_ = MUSCLE::MEDIUM;
    if (CheckHitKey(KEY_INPUT_3))muscleStat_ = MUSCLE::BIG;

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

    MV1SetPosition(unit_.model_, unit_.pos_);
    MV1SetScale(unit_.model_, unit_.scale_);
    MV1DrawModel(unit_.model_);

	//DrawSphere3D(unit_.pos_, RADIUS_SIZE, 30, 0xffffff, 0xffffff, true);
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
    switch (muscleStat_)
    {
    case Player::MUSCLE::SMALL:
        unit_.scale_ = SMALL_MUSCLE;
        break;
    case Player::MUSCLE::MEDIUM:
        unit_.scale_ = MEDIUM_MUSCLE;
        break;
    case Player::MUSCLE::BIG:
        unit_.scale_ = BIG_MUSCLE;
        break;
    }
}

// 何もしていない
void Player::Idle(void)
{
}

// 移動処理
void Player::Move(void)
{
    VECTOR move = VGet(0.0f, 0.0f, 0.0f);

    // 入力で方向ベクトルを作る
    if (CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A))  move.x -= 1.0f;
    if (CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_D)) move.x += 1.0f;
    if (CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W))    move.z += 1.0f;
    if (CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S))  move.z -= 1.0f;

    // 移動量があるときだけ正規化して移動
    if (move.x != 0.0f || move.z != 0.0f)
    {
        // 斜め移動の補正（正規化）
        float len = sqrtf(move.x * move.x + move.z * move.z);
        move.x = (move.x / len) * MOVE_SPEED;
        move.z = (move.z / len) * MOVE_SPEED;

        // 位置を更新
        unit_.pos_.x += move.x;
        unit_.pos_.z += move.z;
    }

}


