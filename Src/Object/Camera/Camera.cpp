#include "Camera.h"
#include <math.h>

#include "../../Application/Application.h"

#include "../../Manager/Input/InputManager.h"

#include "../../Utility/Utility.h"

Camera* Camera::instance_ = nullptr;

void Camera::Init() 
{
    angle_ = Utility::VECTOR_ZERO;

    GetMousePoint(&mouseX, &mouseY);
}

void Camera::Update()
{
    auto& input = InputManager::GetInstance();
    MouseMoveCamera();
    PadMoveCamera();

    if (input.IsNew(KEY_INPUT_RIGHT)) angle_.y += 5;
    if (input.IsNew(KEY_INPUT_LEFT))  angle_.y -= 5;
    if (input.IsNew(KEY_INPUT_DOWN) && angle_.x <= 30)  angle_.x += 5;
    if (input.IsNew(KEY_INPUT_UP) && angle_.x >= -30)  angle_.x -= 5;

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

    SetUseLighting(false);

    // ã‹ó‚Í–¾‚é‚¢ÂA’n•½ü‚Í’W‚¢Â
    //int topColor = GetColor(100, 180, 255);
    //int bottomColor = GetColor(180, 220, 255);

    //DrawSphere3D(camPos_, 10000.0f, 32, topColor, bottomColor, TRUE);

    SetUseLighting(TRUE);
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

    const float sens = 0.1f; // Š´“x’²®
    angle_.y += deltaX * sens;
    angle_.x += deltaY * sens;

    // ã‰º‚Ì‰ñ“]§ŒÀ
    float limit = DX_PI_F * 18;
    if (angle_.x > limit) angle_.x = limit;
    if (angle_.x < -limit) angle_.x = -limit;
}

void Camera::PadMoveCamera()
{
    int lx = 0, ly = 0;
    GetJoypadAnalogInputRight(&lx, &ly, DX_INPUT_PAD1);

    const float sens = 0.05f;
    const int deadZone = 200;

    if (abs(lx) > deadZone)
        angle_.y += lx * sens * 0.1f;

    if (abs(ly) > deadZone)
        angle_.x += ly * sens * 0.1f;

    // ã‰º§ŒÀ
    if (angle_.x > 89.0f) angle_.x = 89.0f;
    if (angle_.x < -89.0f) angle_.x = -89.0f;
}