#include "UI.cpp"

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
private:
	TouchID touch[NUM];
	TouchID Log[NUM];
public:
	int touchCount = -1;
	int LogCount = 0;

	InputTouch() {

	}

	/*毎フレーム呼び出す*/
	void SetTouch() {
		/*現フレームでのタッチ状態の取得*/
		TouchID newTouch[NUM];
		int newTouchCount = (GetTouchInputNum() < NUM) ? GetTouchInputNum() : NUM;;
		for (int i = 0; i < newTouchCount; i++) {
			GetTouchInput(i, &newTouch[i].x, &newTouch[i].y, &newTouch[i].ID, NULL);
		}

		/*前フレーム分と比較、不要なIDを削除*/
		LogCount = 0;
		for (int j = 0; j < touchCount; j++) {
			bool check = true;
			for (int i = 0; i < newTouchCount; i++) {
				if (touch[j].ID == newTouch[i].ID) {
					check = false;
				}
			}
			/*前フレームにのみ存在していた場合*/
			if (check) {
				Log[LogCount] = touch[j];
				LogCount++;
				touch[j].ID = -1;
				Compression(j);
				touchCount--;
			}
		}
		/*前フレーム分と比較、不足分の追加*/
		for (int i = 0; i < newTouchCount; i++) {
			bool check = true;
			for (int j = 0; j < touchCount; j++) {
				if (newTouch[i].ID == touch[j].ID) {
					touch[j].CopyPos(newTouch[i]);
					check = false;
				}
			}
			/*前フレームに存在しなかった場合*/
			if (check) {
				touch[touchCount] = newTouch[i];
				touchCount++;
			}
		}

		/*現フレーム情報に更新*/
		touchCount = newTouchCount;
		/*timeの加算*/
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
	/*
	for (int i = 0; i < touchCount; i++) {
		if (ID == touch[i].ID) {
			if (touch[i].time > 0) return true;
			else return false;
		}
	}
	*/
	bool GetPressed(int ID) {
		if (ID == NULL) return NULL;

		for (int i = 0; i < touchCount; i++) {
			if (ID == touch[i].ID) {
				if (touch[i].time > 0) return true;
				else return false;
			}
		}
		return NULL;
	}
	/*
	for (int i = 0; i < LogCount; i++) {
		if (ID == Log[i].ID) return true;
	}
	for (int i = 0; i < touchCount; i++) {
		if (ID == touch[i].ID) return false;
	}
	*/
	bool GetReleased(int ID) {
		if (ID == NULL) return NULL;

		for (int i = 0; i < LogCount; i++) {
			if (ID == Log[i].ID) return true;
		}
		for (int i = 0; i < touchCount; i++) {
			if (ID == touch[i].ID) return false;
		}
		return NULL;
	}
	/*描画時の中心点と、長方形の辺の長さ(全体)*/
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
	correction　タッチ判定範囲を加算
	return GetID_RangeBox(ui.x, ui.y, 
			ui.sizeX + correction * 2, ui.sizeY + correction * 2, 5);
	*/
	int GetID_RangeBox(UI ui, int correction = 0) {
		return GetID_RangeBox(ui.x, ui.y, 
			ui.sizeX + correction * 2, ui.sizeY + correction * 2, ui.number);
	}
	/*描画時の中心点と、円の半径*/
	int GetID_RangeCircle(int x, int y, int r) {
		for (int i = 0; i < touchCount; i++) {
			int dx = (x - touch[i].x) * (x - touch[i].x);
			int dy = (y - touch[i].y) * (y - touch[i].y);
			if (dx + dy <= r * r) return touch[i].ID;
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