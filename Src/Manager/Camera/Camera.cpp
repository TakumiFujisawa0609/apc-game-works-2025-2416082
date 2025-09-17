#include "Camera.h"
#include "../../Utility/Utility.h"

Camera* Camera::instance_ = nullptr;

void Camera::Init()
{
}

void Camera::Update()
{
	//auto& ins = InputManager::GetInstance();

	// 矢印キーでカメラの角度を変える
	float rotPow = 1.0f * DX_PI_F / 180.0f;
	if (CheckHitKey(KEY_INPUT_DOWN)) { angles_.x += rotPow; }
	if (CheckHitKey(KEY_INPUT_UP)) { angles_.x -= rotPow; }
	if (CheckHitKey(KEY_INPUT_RIGHT)) { angles_.y += rotPow; }
	if (CheckHitKey(KEY_INPUT_LEFT)) { angles_.y -= rotPow; }

	// WASDでカメラを移動させる
	const float movePow = 3.0f;
	VECTOR dir = Utility::VECTOR_ZERO;
	if (CheckHitKey(KEY_INPUT_W)) { dir = { 0.0f, 0.0f, 1.0f }; }
	if (CheckHitKey(KEY_INPUT_A)) { dir = { -1.0f, 0.0f, 0.0f }; }
	if (CheckHitKey(KEY_INPUT_S)) { dir = { 0.0f, 0.0f, -1.0f }; }
	if (CheckHitKey(KEY_INPUT_D)) { dir = { 1.0f, 0.0f, 0.0f }; }

	VECTOR vZero_ = Utility::VECTOR_ZERO;
	bool is = (dir.x == vZero_.x && dir.y == vZero_.y && dir.z == vZero_.z);

	if (is)
	{
		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();
		//mat = MMult(mat, MGetRotX(angles_.x));
		mat = MMult(mat, MGetRotY(angles_.y));
		//mat = MMult(mat, MGetRotZ(angles_.z));
		// 回転行列を使用して、ベクトルを回転させる
		VECTOR moveDir = VTransform(dir, mat);
		// 方向×スピードで移動量を作って、座標に足して移動
		pos_ = VAdd(pos_, VScale(moveDir, movePow));
	}
}

void Camera::Release()
{

}


