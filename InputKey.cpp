#include "DxLib.h"

const int keytype = 9;//1frame�{�^���̎��
const int gameKey_type = 5;//gane���̃L�[
class InputKey{
private:
	int keytime[keytype];
	int GameKey[gameKey_type];
	int holdGameKey[gameKey_type];
	bool PushOneframe(int id, int keyCode) {
		if (CheckHitKey(keyCode) == 1) keytime[id]++;
		else keytime[id] = 0;
		return keytime[id] == 1;
	}
	/*D�AF�AG�AH�AJ*/
	int getGameKeyCode(int number) {
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
		return keyCode;
	}
public:
	/*number = 1,2,3,4,5*/
	bool PushOneframe_PlayGame(int number) {
		int keyCode = getGameKeyCode(number);
		int id = number - 1;
		if (CheckHitKey(keyCode) == 1) GameKey[id]++;
		else GameKey[id] = 0;

		return GameKey[id] == 1;
	}
	/*�����O�m�[�c���Ɏg�p
	number = 1,2,3,4,5
	return CheckHitKey(keyCode)*/
	bool LongHoldKey(int number) {
		int keyCode = getGameKeyCode(number);
		return CheckHitKey(keyCode) > 0;
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
	/*6, KEY_INPUT_LEFT*/
	bool PushOneframe_LEFT() {
		return PushOneframe(6, KEY_INPUT_LEFT);
	}
	/*7, KEY_INPUT_RIGHT*/
	bool PushOneframe_RIGHT() {
		return PushOneframe(7, KEY_INPUT_RIGHT);
	}
	/*8, KEY_INPUT_P*/
	bool PushOneframe_ChangeAutoMode() {
		return PushOneframe(8, KEY_INPUT_P);
	}

	/*�����I��
	CheckHitKey(KEY_INPUT_ESCAPE) == 0*/
	bool ForcedTermination() {
		return CheckHitKey(KEY_INPUT_ESCAPE) == 0;
	}
};