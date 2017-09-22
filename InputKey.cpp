#include "DxLib.h"

const int keytype = 6;//1frame�{�^���̎��
class InputKey{
private:
	int keytime[keytype];

public:
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