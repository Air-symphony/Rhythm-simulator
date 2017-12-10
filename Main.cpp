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
	/*0: 上, 1: 下, 2: 左, 3: 右, 決定*/
	UI ArrowKey[4], Button;
	Graph background;
public:
	GameSimulator(Display display) {
		file.ReadConfig();
		Layout(display);

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
	void Layout(Display display) {
		background.setGraph(LoadGraph("materials\\Image\\background\\Bg_6009.jpg"));
		background.setDisplay(display);
		char _imagepath[256];
		for (int i = 0; i < 4; i++) {
			strcpy_s(_imagepath, "materials\\Image\\ui\\");
			char _number[256];
			sprintf_s(_number, 256, "Arrow%d.png", i + 1);
			strcat_s(_imagepath, _number);
			ArrowKey[i].setGraph(LoadGraph(_imagepath));
			ArrowKey[i].SetSE(LoadSoundMem("materials\\SE\\cursor.mp3"));
		}
		ArrowKey[0].SetPos(display.GetScreenX() - 100, display.GetScreenY() - 300, 5);
		ArrowKey[1].SetPos(display.GetScreenX() - 100, display.GetScreenY() - 200, 5);
		ArrowKey[2].SetPos(display.GetScreenX() - 150, display.GetScreenY() - 250, 5);
		ArrowKey[3].SetPos(display.GetScreenX() - 50, display.GetScreenY() - 250, 5);
		Button.SetUI(LoadGraph("materials\\Image\\ui\\Button.png"),
			display.GetScreenX() - 100, display.GetScreenY() - 100, 5);
		Button.SetSE(LoadSoundMem("materials\\SE\\Decision.mp3"));
		Button.SetText("決定", GetColor(0, 0, 0));
		Button.SetCorrection(10);
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
		MyDrawString str(20);
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			inTouch.Update();
			if (input.PushOneframe_DOWN() || 
				inTouch.ReleasedRangeBox(ArrowKey[1])) {
				number = (number + 1) % _scene;
			}
			else if (input.PushOneframe_UP() ||
				inTouch.ReleasedRangeBox(ArrowKey[0])) {
				number = (number - 1) % _scene;
				if (number < 0) number += _scene;
			}
			if (input.PushOneframe_Decide() ||
				inTouch.ReleasedRangeBox(Button))
				return number;

			ClearDrawScreen();
			background.Draw_BackGround(255);
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

				if (input.PushOneframe_LEFT() ||
					inTouch.ReleasedRangeBox(ArrowKey[2])) {
					if (number == 0) {
						file.speed -= 1;
						if (file.speed < 1) file.speed = 10;
					}
					if (number == 1)
						file.autoMode = !file.autoMode;
					else if (number == 2)
						file.debugMode = !file.debugMode;
				}
				else if(input.PushOneframe_RIGHT() ||
					inTouch.ReleasedRangeBox(ArrowKey[3])){
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

			Button.SetID(inTouch.GetID_RangeBoxOneFrame(Button));
			Button.Draw(str, 30);
			int IconCount = 2;
			if (_type == 3) IconCount = 4;
			for (int i = 0; i < IconCount; i++) {
				ArrowKey[i].SetID(inTouch.GetID_RangeBoxOneFrame(ArrowKey[i]));
				ArrowKey[i].Draw();
			}
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
