#include "AttackBase.h"

#include "../Boss.h"

#include "../../../Scene/Game/GameScene.h"

#include "../../../Application/Application.h"

AttackBase::AttackBase(const VECTOR& boss, const VECTOR& player, const int& voiceLevel) :
	boss_(boss),
	player_(player),
	voiceLevel_(voiceLevel),
    isChanceNow_(false)
{
}

AttackBase::~AttackBase()
{
}

void AttackBase::SubLoad()
{
    DefaultLoad();
}

void AttackBase::SubInit()
{
    isChanceNow_ = false;

    unit_.isAlive_ = true;
    attack_.isEnd_ = false;       // 終了判定(true : 終了 / false : 攻撃中)

    attack_.isParried_ = false;

    attack_.parryCounter_ = PARRY_COUNT_TIME;

    // 継承先の初期化処理
    ParamInit();
}

void AttackBase::SubUpdate()
{
    // パリィされたときと、されてない時の更新処理を分けた
    // されていないときの更新処理は継承先で定義する
    attack_.isParried_ ? ParryUpdate() : DefaultUpdate();
}

void AttackBase::SubDraw()
{
    if (attack_.isEnd_ || !unit_.isAlive_) return;

    // モデルの回転行列の設定
    SetModelMatrix();

    DefaultDraw();
}

void AttackBase::UIDraw(void)
{
    if (!unit_.isAlive_ && attack_.isEnd_) { return; }

    // モデルの描画
    MV1DrawModel(unit_.model_);

    if (isChanceNow_) {
        SetFontSize(128);
        DrawString(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, "攻撃が来た!!!\n叫ぶんだ！！！", 0xffffff);
        SetFontSize(16);
    }
}

void AttackBase::SubRelease()
{
    if (unit_.model_ != -1) {
        MV1DeleteModel(unit_.model_);
        unit_.model_ = -1;
    }
}

void AttackBase::ParryUpdate(void)
{
    // ボス方向ベクトル
    VECTOR dir = VSub(boss_, unit_.pos_);

    // 正規化
    dir = VNorm(dir);

    // パリィ後のスピード
    const float PARRY_SPEED = 15.0f;

    // 移動
    unit_.pos_ = VAdd(unit_.pos_, VScale(dir, PARRY_SPEED));

    unit_.angle_.x += Utility::Deg2RadF(PARRY_SPEED);
    unit_.angle_.z = atan2f(dir.x, dir.z);
}


void AttackBase::SetModelMatrix(void)
{
    MATRIX mat = MGetIdent();

    // スケール・アングル・座標の合成
    mat = MMult(MGetScale(unit_.scale_), mat);
    Utility::MatrixRotMult(mat, unit_.angle_);
    Utility::MatrixPosMult(mat, unit_.pos_);

    MV1SetMatrix(unit_.model_, mat);
}

bool AttackBase::IsChanceNow(void)
{
    // 攻撃がプレイヤーのパリィの範囲内に入ったかどうか
    if (Utility::IsHitCircle(unit_.pos_, attack_.parryCollRadius_, player_, attack_.parryCollRadius_)) {
        isChanceNow_ = true;
        attack_.parryCounter_--;
        GameScene::Slow(10);

        if (attack_.parryCounter_ <= 0) { isChanceNow_ = false; }
    }

    return isChanceNow_;
}