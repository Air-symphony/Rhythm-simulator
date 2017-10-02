#include "DxLib.h"

const int keytype = 6;//1frame�{�^���̎��
const int gameKey_type = 5;//1frame�{�^���̎��
class InputKey{
private:
	int keytime[keytype];
	int GameKey[gameKey_type];
public:
	/*D�AF�AG�AH�AJ
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
		case KEY_INPUT_SPACE://����
			id = 0;
			break;
		case KEY_INPUT_BACK://�ꎞ��~
			id = 1;
			break;
		case KEY_INPUT_R://Reset
			id = 2;
			break;
		case KEY_INPUT_RETURN://�f�o�b�O�p
			id = 3;
			break;
		case KEY_INPUT_UP://�I��p
			id = 4;
			break;
		case KEY_INPUT_DOWN://�I��p
			id = 5;
			break;
		}
		if (CheckHitKey(keyCode) == 1) keytime[id]++;
		else keytime[id] = 0;

		return keytime[id] == 1;
	}
};