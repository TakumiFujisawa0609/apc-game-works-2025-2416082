#pragma once

#include<vector>

#include"../../Common/Vector2.h"

class KeyManager
{
private:
	static KeyManager* ins;

	KeyManager();
	~KeyManager() {};
public:
	static void CreateIns(void) { if (ins == nullptr) { ins = new KeyManager(); ins->Init(); } }
	static KeyManager& GetIns(void) { return *ins; }
	static void DeleteIns(void) { if (ins != nullptr) { ins->Release(); delete ins; ins = nullptr; } }

	void Update(void);

	enum class KEY_TYPE
	{
		NON = -1,

		// 汎用操作
		UP,
		DOWN,
		RIGHT,
		LEFT,
		// プレイヤー操作
		ATTACK,
		SPECIAL_ATTACK,

		// システム操作
		PAUSE,

		GAME_END,

		MAX,
	};

	struct KEY_INFO { bool prev = false, now = false, up = false, down = false; };

	const KEY_INFO GetInfo(KEY_TYPE k) { return keyInfo[(int)k]; }


	// コントローラーが接続されているか
	bool IsControllerConnected(void);

	Vector2 GetRightStickVec(void);
	Vector2 GetLeftStickVec(void);

private:
	void Init(void);
	void Release(void);

	KEY_INFO keyInfo[(int)KEY_TYPE::MAX];

	std::vector<int>keyboardFormat[(int)KEY_TYPE::MAX];
	std::vector<int>controllerButtonFormat[(int)KEY_TYPE::MAX];
	enum class CONTROLLER_OTHERS
	{
		LEFTSTICK_UP,
		LEFTSTICK_DOWN,
		LEFTSTICK_RIGHT,
		LEFTSTICK_LEFT,

		RIGHTSTICK_UP,
		RIGHTSTICK_DOWN,
		RIGHTSTICK_RIGHT,
		RIGHTSTICK_LEFT,

		LEFT_TRIGGER,
		RIGHT_TRIGGER,
	};
	bool ControllerOthersInput(const CONTROLLER_OTHERS& input);
	std::vector<CONTROLLER_OTHERS>controllerOthersFormat[(int)KEY_TYPE::MAX];
};

using KEY = KeyManager;
using KEY_TYPE = KEY::KEY_TYPE;