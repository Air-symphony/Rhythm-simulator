#include "DxLib.h"

const int keytype = 6;//1frameボタンの種類
class InputKey{
private:
	int keytime[keytype];

public:
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