#include "PlayGame.cpp"
#include "FileReader.cpp"

/*文字の高さの取得*/
int fontsize = 40;
/*
fontsize = size;
SetFontSize(fontsize);
*/
void FontSize(int size) {
	fontsize = size;
	SetFontSize(fontsize);
}
/*
選択画面
int scenecount = 3;
=> 0 終了、1 ゲーム画面、2 設定画面
int scene = 0;
InputKey input;
*/
class GameSimulator {
private:
	int scenecount = 3;
	//int musiccount = 3 + 1;
	InputKey input;
	InputTouch inTouch;
	FileReader file;

	int id;
public:
	GameSimulator(Display display) {
		file.ReadConfig();

		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			int scene = SelectNumber(1, scenecount);
			if (scene == 1)
				Playgame(display);
			else if (scene == 2)
				Config();
			else if (scene == 0)
				break;
		}
	}

	void Playgame(Display display) {
		file.ReadFileList();
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			int scene = SelectNumber(2, file.fileCount + 1);
			if (scene == 0)
				break;
			GameScreen game(display, file.fileList[scene - 1], 
				file.speed, file.debugMode, file.autoMode);
		}
	}

	void Config() {
		SelectNumber(3, 3);
		file.UpdateConfig();
	}

	/*_type 1,2,3
	_scene 選択肢が何個か*/
	int SelectNumber(int _type, int _scene) {
		int number = 1;
		FontSize(20);
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			if (input.PushOneframe_DOWN()) {
				number = (number + 1) % _scene;
			}
			else if (input.PushOneframe_UP()) {
				number = (number - 1) % _scene;
				if (number < 0) number += _scene;
			}
			if (input.PushOneframe_Decide())
				return number;

			ClearDrawScreen();
			if (_type == 1) {
				DrawString(0, 0, "Please Select Menu", GetColor(255, 255, 255));
				DrawString(20, fontsize * 3, "1.Play", GetColor(255, 255, 255));
				DrawString(20, fontsize * 4, "2.Config", GetColor(255, 255, 255));
				DrawString(20, fontsize * 5, "0.Finish", GetColor(255, 255, 255));
			}
			else if (_type == 2) {
				DrawString(0, 0, "Please Select Music", GetColor(255, 255, 255));
				int y = fontsize * 3;
				for (int i = 0; i < file.fileCount; i++) {
					DrawFormatString(20, y + fontsize * i, GetColor(255, 255, 255), "%d:", i + 1);
					DrawString(50, y + fontsize * i, file.fileList[i], GetColor(255, 255, 255));
				}
				DrawFormatString(20, y + fontsize * file.fileCount, GetColor(255, 255, 255), "%d:", 0);
				DrawString(50, y + fontsize * file.fileCount, "Cancel", GetColor(255, 255, 255));
			}
			else if (_type == 3) {
				DrawString(0, 0, "Config", GetColor(255, 255, 255));
				int y = 100;
				int SPEED_y = y, AutoMode_y = y + 20, DebugMode_y = y + 40;
				DrawString(20, SPEED_y, file.SPEED, GetColor(255, 255, 255));
				DrawString(20, AutoMode_y, file.AutoMode, GetColor(255, 255, 255));
				DrawString(20, DebugMode_y, file.DebugMode, GetColor(255, 255, 255));
				if (number == 0)
					DrawString(20, SPEED_y, file.SPEED, GetColor(255, 0, 0));
				else if (number == 1)
					DrawString(20, AutoMode_y, file.AutoMode, GetColor(255, 0, 0));
				else if (number == 2)
					DrawString(20, DebugMode_y, file.DebugMode, GetColor(255, 0, 0));

				if (input.PushOneframe_LEFT()) {
					if (number == 0) {
						file.speed -= 1;
						if (file.speed < 1) file.speed = 10;
					}
					if (number == 1)
						file.autoMode = !file.autoMode;
					else if (number == 2)
						file.debugMode = !file.debugMode;
				}
				else if(input.PushOneframe_RIGHT()){
					if (number == 0) {
						file.speed += 1;
						if (file.speed > 10) file.speed = 1;
					}
					if (number == 1)
						file.autoMode = !file.autoMode;
					else if (number == 2)
						file.debugMode = !file.debugMode;
				}
				int x = 140;
				DrawFormatString(x, SPEED_y, GetColor(255, 255, 255), "%d", file.speed);
				if (file.autoMode)
					DrawString(x, AutoMode_y, file.True, GetColor(255, 255, 255));
				else
					DrawString(x, AutoMode_y, file.False, GetColor(255, 255, 255));

				if (file.debugMode)
					DrawString(x, DebugMode_y, file.True, GetColor(255, 255, 255));
				else
					DrawString(x, DebugMode_y, file.False, GetColor(255, 255, 255));
			}
			DrawFormatString(20, fontsize * 2, GetColor(255, 255, 255), "Select number : %d", number);

			// タッチパネル テスト
			/*
			inTouch.SetTouch();
			inTouch.PrintTouch(250, 40);
			inTouch.PrintLog(520, 40);

			DrawCircle(400, 250, 100, GetColor(255, 255, 255), true);
			bool release = inTouch.GetRelease(id);
			id = inTouch.GetID_RangeCircle(400, 250, 100);//inTouch.GetID_RangeBox(300, 150, 200, 200, 1);
			if (id != NULL) {
				DrawFormatString(250, 130, GetColor(255, 255, 255), "InsideID : %d", id);
			}
			if (release) {
				DrawFormatString(450, 130, GetColor(255, 255, 255), "Release");
			}
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
			if (inTouch.LogCount > 0) {
				while (ProcessMessage() == 0 && input.ForcedTermination()) {
					if (input.PushOneframe_ChangeAutoMode())break;
				}
			}
			*/
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
		}
		return 0;
	}
};

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Display display;
	SetGraphMode(display.GetScreenX(), display.GetScreenY(), 16);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)
		return -1;
	SetDrawScreen(DX_SCREEN_BACK);

	GameSimulator game(display);

	DxLib_End();				// ＤＸライブラリ使用の終了処理
	return 0;				// ソフトの終了 
}
