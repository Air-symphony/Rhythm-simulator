#include "DxLib.h"

const int keytype = 7;//1frameボタンの種類
const int gameKey_type = 5;//gane中のキー
class InputKey{
private:
	int keytime[keytype];
	int GameKey[gameKey_type];
	bool PushOneframe(int id, int keyCode) {
		if (CheckHitKey(keyCode) == 1) keytime[id]++;
		else keytime[id] = 0;
		return keytime[id] == 1;
	}
public:
	/*D、F、G、H、J
	number = 1,2,3,4,5*/
	bool PushOneframe_PlayGame(int number) {
		int keyCode;
		switch (number) {
		case 1:
			keyCode = KEY_INPUT_D;
			break;
		case 2:
			keyCode = KEY_INPUT_F;
			break;
		case 3:
			keyCode = KEY_INPUT_G;
			break;
		case 4:
			keyCode = KEY_INPUT_H;
			break;
		case 5:
			keyCode = KEY_INPUT_J;
			break;
		}
		int id = number - 1;
		if (CheckHitKey(keyCode) == 1) GameKey[id]++;
		else GameKey[id] = 0;

		return GameKey[id] == 1;
	}

	/*0, KEY_INPUT_SPACE*/
	bool PushOneframe_Decide() {
		return PushOneframe(0, KEY_INPUT_SPACE);
	}
	/*1, KEY_INPUT_BACK*/
	bool PushOneframe_Stop() {
		return PushOneframe(1, KEY_INPUT_BACK);
	}
	/*2, KEY_INPUT_R*/
	bool PushOneframe_Reset() {
		return PushOneframe(2, KEY_INPUT_R);
	}
	/*3, KEY_INPUT_RETURN*/
	bool PushOneframe_Debug() {
		return PushOneframe(3, KEY_INPUT_RETURN);
	}
	/*4, KEY_INPUT_UP*/
	bool PushOneframe_UP() {
		return PushOneframe(4, KEY_INPUT_UP);
	}
	/*5, KEY_INPUT_DOWN*/
	bool PushOneframe_DOWN() {
		return PushOneframe(5, KEY_INPUT_DOWN);
	}
	/*6, KEY_INPUT_P*/
	bool PushOneframe_ChangeAutoMode() {
		return PushOneframe(6, KEY_INPUT_P);
	}

	/*強制終了
	CheckHitKey(KEY_INPUT_ESCAPE) == 0*/
	bool ForcedTermination() {
		return CheckHitKey(KEY_INPUT_ESCAPE) == 0;
	}
};