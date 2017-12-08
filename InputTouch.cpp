#include "DxLib.h"

class TouchID {
public:
	int x, y, ID, time;
	TouchID() {
		time = 0;
	}
	void CopyPos(TouchID t) {
		x = t.x;
		y = t.y;
	}
};

const int NUM = 5;
class InputTouch {
public:
	TouchID touch[NUM];
	int touchCount = -1;
	TouchID Log[NUM];
	int LogCount = 0;

	InputTouch() {

	}

	void SetTouch() {
		/*���t���[���ł̃^�b�`��Ԃ̎擾*/
		TouchID newTouch[NUM];
		int newTouchCount = (GetTouchInputNum() < NUM) ? GetTouchInputNum() : NUM;;
		for (int i = 0; i < newTouchCount; i++) {
			GetTouchInput(i, &newTouch[i].x, &newTouch[i].y, &newTouch[i].ID, NULL);
		}

		/*�O�t���[�����Ɣ�r�A�s�v��ID���폜*/
		LogCount = 0;
		for (int j = 0; j < touchCount; j++) {
			bool check = true;
			for (int i = 0; i < newTouchCount; i++) {
				if (touch[j].ID == newTouch[i].ID) {
					check = false;
				}
			}
			/*�O�t���[���ɂ̂ݑ��݂��Ă����ꍇ*/
			if (check) {
				Log[LogCount] = touch[j];
				LogCount++;
				touch[j].ID = -1;
				Compression(j);
				touchCount--;
			}
		}
		/*�O�t���[�����Ɣ�r�A�s�����̒ǉ�*/
		for (int i = 0; i < newTouchCount; i++) {
			bool check = true;
			for (int j = 0; j < touchCount; j++) {
				if (newTouch[i].ID == touch[j].ID) {
					touch[j].CopyPos(newTouch[i]);
					check = false;
				}
			}
			/*�O�t���[���ɑ��݂��Ȃ������ꍇ*/
			if (check) {
				touch[touchCount] = newTouch[i];
				touchCount++;
			}
		}

		/*���t���[�����ɍX�V*/
		touchCount = newTouchCount;
		/*time�̉��Z*/
		for (int i = 0; i < touchCount; i++) {
			touch[i].time++;
		}
	}

	int GetX(int ID) {
		if (ID == NULL) return NULL;
		for (int i = 0; i < touchCount; i++) {
			if (ID == touch[i].ID) return touch[i].x;
		}
		return NULL;
	}
	int GetY(int ID) {
		if (ID == NULL) return NULL;
		for (int i = 0; i < touchCount; i++) {
			if (ID == touch[i].ID) return touch[i].x;
		}
		return NULL;
	}
	int GetTime(int ID) {
		if (ID == NULL) return NULL;
		for (int i = 0; i < touchCount; i++) {
			if (ID == touch[i].ID) return touch[i].time;
		}
		return NULL;
	}
	bool GetRelease(int ID) {
		if (ID == NULL) return NULL;

		for (int i = 0; i < LogCount; i++) {
			if (ID == Log[i].ID) return true;
		}
		for (int i = 0; i < touchCount; i++) {
			if (ID == touch[i].ID) return false;
		}
		return NULL;
	}
	/*�`�掞�̒��S�_�ƁA�����`�̕ӂ̒���(�S��)*/
	int GetID_RangeBox(int x, int y, int width, int height, int number = 5) {
		int minX = x - (((number - 1) % 3)) * (width / 2);
		int maxX = x + (2 - ((number - 1) % 3)) * (width / 2);
		int minY = y - ((int)((number - 1) / 3)) * (height / 2);
		int maxY = y + (2 - (int)((number - 1) / 3)) * (height / 2);

		for (int i = 0; i < touchCount; i++) {
			if (minX <= touch[i].x && touch[i].x <= maxX &&
				minY <= touch[i].y && touch[i].y <= maxY)
				return touch[i].ID;
		}
		return NULL;
	}
	/*
	DrawFormatString(x, y, GetColor(255, 255, 255), "Touch = %d", touchCount);
	for (int i = 0; i < touchCount; i++) {
	DrawFormatString(x, y + (i + 1) * 20, GetColor(255, 255, 255),
	"(%d)ID: %d (%d, %d) %d", i, touch[i].ID, touch[i].x, touch[i].y, touch[i].time);
	}
	*/
	void PrintTouch(int x, int y) {
		DrawFormatString(x, y, GetColor(255, 255, 255),
			"Touch = %d", touchCount);
		for (int i = 0; i < touchCount; i++) {
			DrawFormatString(x, y + (i + 1) * 20, GetColor(255, 255, 255),
				"(%d)ID: %d (%d, %d) %d", i, touch[i].ID, touch[i].x, touch[i].y, touch[i].time);
		}
	}
	void PrintLog(int x, int y) {
		DrawFormatString(x, y, GetColor(255, 255, 255),
			"Log = %d", LogCount);
		for (int i = 0; i < LogCount; i++) {
			DrawFormatString(x, y + (i + 1) * 20, GetColor(255, 255, 255),
				"(%d)ID: %d (%d, %d) %d", i, Log[i].ID, Log[i].x, Log[i].y, Log[i].time);
		}
	}
private:
	void Compression(int i)
	{
		if (i + 1 < NUM)
		{
			if (touch[i + 1].ID > 0) {
				touch[i] = touch[i + 1];
				touch[i + 1].ID = 0;
				Compression(i + 1);
			}
		}
	}
};