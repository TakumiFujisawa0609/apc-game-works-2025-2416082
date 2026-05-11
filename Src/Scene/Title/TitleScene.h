#pragma once

#include "../../Application/Application.h"
#include "../SceneBase.h"

#include <DxLib.h>
#include <vector>

class AnimationController;
class MicInput;

class TitleScene : public SceneBase
{
public:
	enum  class ANIM_TYPE
	{
		IDLE,
		ATTACK,

		MAX
	};

	enum class TITLE_IMAGE
	{
		TITLE_LOGO,
		MEAT,
		MUSCLE,
		BACK_GROUND,
		PRESS_BUTTON,

		MAX
	};

	// ƒvƒŒƒCƒ„[```````````````````````````````````````````````
	static constexpr VECTOR DEFAULT_POS = { 1000.0f,100.0f,0.0f, };	// ƒ^ƒCƒgƒ‹—pƒvƒŒƒCƒ„[‚Ì‰ŠúÀ•W
	static constexpr VECTOR SCALE = { 2.0f,2.0f,2.0f };				// ƒvƒŒƒCƒ„[ƒ‚ƒfƒ‹‚ÌƒTƒCƒY

	static constexpr VECTOR MAX_SIZE = { 2.0f,2.0f,2.0f };			// ‹Ø“÷‚ÌƒXƒP[ƒ‹‚ÌÅ‘å’l
	static constexpr VECTOR MIN_ARM_MUSCLE = { 1.0f,1.0f,1.0f };	// ‹Ø“÷‚ÌƒXƒP[ƒ‹‚ÌÅ’á’l
	// ƒvƒŒƒCƒ„[```````````````````````````````````````````````

	// ‰æ‘œ`````````````````````````````````````````````````
	static constexpr VECTOR PRESS_BUTTON_POS = { Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 + 300, 0.0f };		// ƒ{ƒ^ƒ“—pUI‚Ì•`‰æÀ•W
	static constexpr VECTOR TITLE_LOGO_POS = { Application::SCREEN_CENTER_POS.x - 500.0f,	Application::SCREEN_CENTER_POS.y };		// ƒ^ƒCƒgƒ‹ƒƒS‚Ì•`‰æÀ•W

	static constexpr float MUSCLE_IMAGE_SCALE_MAX = 8.0f;	// ”wŒi‚ÌŠ¿š‰æ‘œ‚ªº‚Å‘å‚«‚­‚È‚éÅ‘å”{—¦
	static constexpr float MUSCLE_IMAGE_SCALE_MIN = 3.0f;	// ”wŒi‚ÌŠ¿š‰æ‘œ‚ÌƒTƒCƒY‚ÌÅ¬”{—¦

	static constexpr int MUSCLE_INDEX = 4;		// ƒvƒŒƒCƒ„[ƒ‚ƒfƒ‹‚Ì‹Ø“÷‚ÌƒtƒŒ[ƒ€”

	static constexpr int MUSCLE_KANJI_MAX = 10;	// u‹Øvu“÷v‚Ì”

	static constexpr double PRESS_BUTTON_EX_RATE = 2.0f;	// ƒ{ƒ^ƒ“‚ÌUI‚Ì•`‰æ‚µ‚½‚Æ‚«‚Ì‘å‚«‚³”{—¦
	// ‰æ‘œ`````````````````````````````````````````````````

	TitleScene();
	~TitleScene()override;

	// “Ç‚İ‚İ
	void Load(void)override;
	// ‰Šú‰»ˆ—
	void Init(void)override;
	// XVƒXƒeƒbƒv
	void Update(void)override;
	// •`‰æˆ—
	void Draw(void)override;
	// ‰ğ•úˆ—
	void Release(void)override;

private:
	AnimationController* animation_;
	MicInput* mic_;

	// ƒ‚ƒfƒ‹ƒAƒjƒ[ƒVƒ‡ƒ“—p----------------------------------
	struct AnimInfo
	{
		const char* name;
		float speed;
	};

	// ƒAƒjƒ[ƒVƒ‡ƒ“‚ÌƒpƒX‚ÆÄ¶‘¬“x‚Ìİ’è
	const std::vector<AnimInfo> ANIMATION_INFO = {
		{ "Idle1", 30.0f },
		{ "Punching", 100.0f }
	};
	// --------------------------------------------------------

	// ƒ^ƒCƒgƒ‹‰æ–Ê‚Å•\¦‚·‚é‰æ‘œ‚Ìƒnƒ“ƒhƒ‹ID
	std::vector<int> titleImages_;

	// ‰æ‘œ‚ÌÀ•W
	std::vector<VECTOR> imagePos_;
	int frameCounter_;

	struct SubBase
	{
		int model_;

		VECTOR pos;
		VECTOR scale;
		VECTOR angle;
	};

	SubBase unit_;

	bool isStart_;
	int	voiceLevel_;

	float imageScale_;

	void AddBoneScale(int index, VECTOR scale);

};