#include "Camera.h"
#include <math.h>

#include "../../Application/Application.h"

#include "../../Utility/Utility.h"

Camera* Camera::instance_ = nullptr;

void Camera::Init() 
{
    angle_ = Utility::VECTOR_ZERO;

    GetMousePoint(&mouseX, &mouseY);
}

void Camera::Update()
{
    MouseMoveCamera();

    if (CheckHitKey(KEY_INPUT_RIGHT)) angle_.y += 5;
    if (CheckHitKey(KEY_INPUT_LEFT))  angle_.y -= 5;
    if (CheckHitKey(KEY_INPUT_DOWN) && angle_.x <= 30)  angle_.x += 5;
    if (CheckHitKey(KEY_INPUT_UP) && angle_.x >= -30)  angle_.x -= 5;

    // YŽ²‰ñ“]s—ñ‚ðì¬
    MATRIX matY = MGetRotY(angle_.y * DX_PI_F / 180.0f);
    MATRIX matX = MGetRotX(angle_.x * DX_PI_F / 180.0f);
    MATRIX mat = MMult(matX, matY);

    // LOCAL_POS‚ÌŽü‚è‚Å‰ñ“]‚³‚¹‚é
    VECTOR rotatePos = VTransform(LOCAL_POS, mat);

    // ‰ñ“]Œã‚ÌˆÊ’u‚ðƒ^[ƒQƒbƒgÀ•W‚É‰ÁŽZ
    camPos_ = VAdd(*camTarget_, rotatePos);
}

void Camera::Apply()
{
    SetCameraPositionAndTarget_UpVecY(camPos_, *camTarget_);
}

void Camera::MouseMoveCamera(void)
{
    // ‰æ–Ê‚Ì’†‰›‚ðŠî€‚É‚·‚é

    int nowMouseX, nowMouseY;
    GetMousePoint(&nowMouseX, &nowMouseY);

    // ·•ªŒvŽZi‘Š‘ÎˆÚ“®—Êj
    int deltaX = nowMouseX - Application::SCREEN_SIZE_X / 2;
    int deltaY = nowMouseY - Application::SCREEN_SIZE_Y / 2;

    // ƒ}ƒEƒX‚ðÄ‚Ñ’†‰›‚É–ß‚·
    SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

    // ‰‰ñ‚¾‚¯·•ª‚ð–³Ž‹i‘åƒWƒƒƒ“ƒv–hŽ~j
    static bool first = true;
    if (first) {
        first = false;
        return;
    }

    const float sens = 0.3f; // Š´“x’²®
    angle_.y += deltaX * sens;
    angle_.x += deltaY * sens;

    // ã‰º‚Ì‰ñ“]§ŒÀ
    float limit = DX_PI_F / 3.0f; // –ñ60“x
    if (angle_.x > limit) angle_.x = limit;
    if (angle_.x < -limit) angle_.x = -limit;
}

