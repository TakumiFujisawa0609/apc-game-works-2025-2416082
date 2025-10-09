#include "Boss.h"
#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Scene/SceneManager/SceneManager.h"

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
	unit_.model_ = MV1LoadModel("Data/Model/Boss/hand.mv1");
}

void Boss::Init(void)
{
	unit_.para_.colliShape = CollisionShape::CAPSULE;

	unit_.para_.size = { 200.0f, 600.0f,200.0f };
	unit_.para_.radius = 100.0f;
	unit_.para_.capsuleHalfLen = unit_.para_.size.y - (unit_.para_.radius * 2);
	unit_.pos_ = { 0.0f, 50.0f, 200.0f};

	unit_.hp_ = HP_MAX;

	unit_.isAlive_ = true;

	color1 = 0xfff000;
}

void Boss::Update(void)
{
	auto& scene = SceneManager::GetInstance();
	if (unit_.hp_ < 0) {
		unit_.hp_ = 0;
		unit_.isAlive_ = false;

		scene.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
	Invi();
}

void Boss::Draw(void)
{
	if (!unit_.isAlive_)return;

	VECTOR pos1 = VSub(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen / 2,0.0f });
	VECTOR pos2 = VAdd(unit_.pos_, { 0.0f,unit_.para_.capsuleHalfLen / 2,0.0f });
	DrawCapsule3D(pos1, pos2, unit_.para_.radius, 16, color1, color1, false);

	MV1SetPosition(unit_.model_, unit_.pos_);
	MV1DrawModel(unit_.model_);


	for (int i = 0; i < unit_.hp_; i++) {
		DrawBox(50 + (i * 10), Application::SCREEN_SIZE_Y - 100, 70 + (i * 10), Application::SCREEN_SIZE_Y - 100 + 50, 0xff0000, true);
	}
}

void Boss::Release(void)
{
	MV1DeleteModel(unit_.model_);
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
