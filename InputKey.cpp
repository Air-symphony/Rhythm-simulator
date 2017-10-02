#include "DxLib.h"

const int keytype = 6;//1frameボタンの種類
const int gameKey_type = 5;//1frameボタンの種類
class InputKey{
private:
	int keytime[keytype];
	int GameKey[gameKey_type];
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
	bool PushOneframe(int keyCode) {
		int id;
		switch (keyCode) {
		case KEY_INPUT_SPACE://決定
			id = 0;
			break;
		case KEY_INPUT_BACK://一時停止
			id = 1;
			break;
		case KEY_INPUT_R://Reset
			id = 2;
			break;
		case KEY_INPUT_RETURN://デバッグ用
			id = 3;
			break;
		case KEY_INPUT_UP://選択用
			id = 4;
			break;
		case KEY_INPUT_DOWN://選択用
			id = 5;
			break;
		}
		if (CheckHitKey(keyCode) == 1) keytime[id]++;
		else keytime[id] = 0;

		return keytime[id] == 1;
	}
};