#include "UI.cpp"

class TouchID {
public:
	int x, y, ID, time;
	TouchID() {
		time = 0; ID = x = y = -1;
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
	int touchCount = 0;
	int LogCount = 0;

	InputTouch() {

	}

	/*毎フレーム呼び出す*/
	void Update() {
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
			if (ID == touch[i].ID) return touch[i].y;
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
	bool Press(int ID) {
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
	離されていたら
	for (int i = 0; i < touchCount; i++) {
	if (ID == touch[i].ID) return false;
	}
	return true;
	*/
	bool Release(int ID) {
		if (ID == NULL) return NULL;

		for (int i = 0; i < touchCount; i++) {
			if (ID == touch[i].ID) return false;
		}
		return true;
	}
	/*
	1フレーム前に離されたかを所得
	for (int i = 0; i < LogCount; i++) {
	if (ID == Log[i].ID) return true;
	}
	for (int i = 0; i < touchCount; i++) {
	if (ID == touch[i].ID) return false;
	}
	*/
	bool Released(int ID) {
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
	return GetID_RangeBox(ui.x, ui.y, 
			ui.sizeX + correction * 2, ui.sizeY + correction * 2, 5);
	*/
	int GetID_RangeBox(UI ui) {
		return GetID_RangeBox(ui.x, ui.y,
			ui.sizeX + ui.correction * 2, ui.sizeY + ui.correction * 2, ui.number);
	}
	/*
	int id = GetID_RangeBox(ui.x, ui.y,
			ui.sizeX + correction * 2, ui.sizeY + correction * 2, ui.number);
	if (GetTime(id) == 1) return id;
	return NULL;
	*/
	int GetID_RangeBoxOneFrame(UI ui) {
		if (ui.ID == NULL || Release(ui.ID)) {
			int id = GetID_RangeBox(ui);

			if (GetTime(id) == 1) return id;
			return NULL;
		}
		return ui.ID;
	}
	/*描画時の中心点と、長方形の辺の長さ(全体)*/
	int GetLogID_RangeBox(int x, int y, int width, int height, int number = 5) {
		int minX = x - (((number - 1) % 3)) * (width / 2);
		int maxX = x + (2 - ((number - 1) % 3)) * (width / 2);
		int minY = y - ((int)((number - 1) / 3)) * (height / 2);
		int maxY = y + (2 - (int)((number - 1) / 3)) * (height / 2);

		for (int i = 0; i < LogCount; i++) {
			if (minX <= Log[i].x && Log[i].x <= maxX &&
				minY <= Log[i].y && Log[i].y <= maxY)
				return Log[i].ID;
		}
		return NULL;
	}
	/*
	return GetLogID_RangeBox(ui.x, ui.y,
	ui.sizeX + correction * 2, ui.sizeY + correction * 2, 5);
	*/
	int GetLogID_RangeBox(UI ui) {
		return GetLogID_RangeBox(ui.x, ui.y,
			ui.sizeX + ui.correction * 2, ui.sizeY + ui.correction * 2, ui.number);
	}
	/*
	if (ui.ID == NULL) return false;
	return (ui.ID == GetLogID_RangeBox(ui));
	*/
	bool ReleasedRangeBox(UI ui) {
		if (ui.ID == NULL) return false;
		if (ui.ID == GetLogID_RangeBox(ui)) {
			PlaySoundMem(ui.sound, DX_PLAYTYPE_BACK, TRUE);
			return true;
		}
		return false;
		return (ui.ID == GetLogID_RangeBox(ui));
	}
	/*
	return (GetTime(GetID_RangeBox(x, y, width, height, number)) == 1);
	*/
	bool PressRangeBoxOneFrame(int x, int y, int width, int height, int number = 5) {
		return (GetTime(GetID_RangeBox(x, y, width, height, number)) == 1);
	}
	/*
	return (GetTime(GetID_RangeBox(ui)) == 1);
	*/
	bool PressRangeBoxOneFrame(UI ui) {
		return (GetTime(GetID_RangeBox(ui)) == 1);
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