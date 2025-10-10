#include "Boss.h"
#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Scene/SceneManager/SceneManager.h"

#include "Hand/BossRightHand.h"

#include "../Player/Arm/LeftArm.h"
#include "../Player/Arm/RightArm.h"

Boss::Boss()
{
}

Boss::~Boss()
{
}

void Boss::Load(void)
{
	rHand_ = new BossRightHand();
	rHand_->Load();

	unit_.model_ = MV1LoadModel("Data/Model/Boss/BossHead.mv1");
}

void Boss::Init(void)
{
	unit_.para_.colliShape = CollisionShape::CAPSULE;

	unit_.para_.radius = RADIUS;
	unit_.para_.capsuleHalfLen = HALF_LEN;
	unit_.pos_ = DEFAULT_POS;

	unit_.hp_ = HP_MAX;
	unit_.scale_ = SCALE;

	unit_.isAlive_ = true;

	color1 = 0xfff000;

	rHand_->Init();
}

void Boss::Update(void)
{
	auto& scene = SceneManager::GetInstance();
	if (unit_.hp_ < 0) {
		unit_.hp_ = 0;
		unit_.isAlive_ = false;
	}

	rHand_->Update();

	Invi();
}

void Boss::Draw(void)
{
	if (!unit_.isAlive_)return;

	VECTOR offset = { 0.0f, -150.0f, 0.0f };

	MATRIX mat = MGetIdent();

	mat = MMult(mat, MGetRotX(unit_.angle_.x));
	mat = MMult(mat, MGetRotY(unit_.angle_.y));
	mat = MMult(mat, MGetRotZ(unit_.angle_.z));

	mat = MMult(MGetScale(unit_.scale_), mat);

	VECTOR worldPos = VTransform(offset, mat);

	mat.m[3][0] = worldPos.x + unit_.pos_.x;
	mat.m[3][1] = worldPos.y + unit_.pos_.y;
	mat.m[3][2] = worldPos.z + unit_.pos_.z;

	MV1SetMatrix(unit_.model_, mat);
	MV1DrawModel(unit_.model_);

	rHand_->SetBaseMat(mat);

	VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
	VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen,0.0f });
	DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, color1, color1, false);


#ifdef _DEBUG
	for (int i = 0; i < unit_.hp_; i++) {
		DrawBox(50 + (i * 5), Application::SCREEN_SIZE_Y - 100, 60 + (i * 5), Application::SCREEN_SIZE_Y - 100 + 50, 0xff0000, true);
	}
	DrawSphere3D(unit_.pos_, 20, 16, 0xff00ff, 0xff00ff, true);
#endif // _DEBUG

	rHand_->Draw();
}

void Boss::Release(void)
{
	MV1DeleteModel(unit_.model_);

	if (rHand_)
	{
		rHand_->Release();
		delete rHand_;
		rHand_ = nullptr;
	}
}

void Boss::OnCollision(UnitBase* other)
{
	if (unit_.inviciCounter_ > 0) { return; }
	
	if (dynamic_cast<LeftArm*>(other) ||
		dynamic_cast<RightArm*>(other))
	{
		unit_.hp_ -= 10;
		unit_.inviciCounter_ = INVI_TIME;
		return;
	}

}
