#include "BossRightHand.h"

#include "../../Player/Player.h"

BossRightHand::BossRightHand()
{
}

BossRightHand::~BossRightHand()
{
}

void BossRightHand::Load(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Boss/hand.mv1");
}

void BossRightHand::Init(void)
{
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.colliType = CollisionType::ENEMY;

    unit_.angle_ = { 0.0f, Utility::Deg2RadF(90.0f), 0.0f };

    unit_.para_.radius = 200.0f;
    unit_.scale_ = SCALE;

	unit_.isAlive_ = true;
}

void BossRightHand::Update(void)
{
	Invi();
}

void BossRightHand::Draw(void)
{
    if (!unit_.isAlive_) return;

    // --- ローカル変換（右手自体の姿勢など） ---
    MATRIX localMat = MGetIdent();
    localMat = MMult(localMat, MGetScale(unit_.scale_));
    localMat = MMult(localMat, MGetRotX(unit_.angle_.x));
    localMat = MMult(localMat, MGetRotY(unit_.angle_.y));
    localMat = MMult(localMat, MGetRotZ(unit_.angle_.z));

    // --- ボスを中心に右手が回る位置を計算 ---
    VECTOR localOffset = LOCAL_POS;   // 例: {100.0f, 0.0f, 0.0f} でボスの右側
    unit_.pos_ = VTransform(localOffset, baseMat_); // ボス行列で変換 → 公転

    // さらに自分で移動したいなら加算（攻撃モーションなど）
    //worldPos = VAdd(worldPos, unit_.pos_);

    // --- 描画行列を作る ---
    MATRIX drawMat = MMult(baseMat_, localMat);
    drawMat.m[3][0] = unit_.pos_.x;
    drawMat.m[3][1] = unit_.pos_.y;
    drawMat.m[3][2] = unit_.pos_.z;

    // --- モデル描画 ---
    MV1SetMatrix(unit_.model_, drawMat);
    MV1DrawModel(unit_.model_);

#ifdef _DEBUG
    DrawSphere3D(unit_.pos_, unit_.para_.radius, 16, 0xff00ff, 0xff00ff, true);
#endif
}

void BossRightHand::Release(void)
{
    MV1DeleteModel(unit_.model_);
}

void BossRightHand::OnCollision(UnitBase* other)
{
	if (dynamic_cast<Player*>(other)) {
		unit_.inviciCounter_ = 60;
	}
}

