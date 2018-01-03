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

class GameSimulator {
private:
	InputKey input;
	InputTouch inTouch;
	Display display;
	FileReader file;

	ButtonUI ConfigButton, DecideButton, ExitButton;
	int ArrowSE = 0, ButtonSE = 0;
public:
	GameSimulator(Display _display) {
		display = _display;
		file.ReadConfig();
		file.ReadFileList();

		ButtonLayout();
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			int scene = SelectMusic();
			if (scene == -1) break;

			GameScreen game(display, file.fileList[scene],
				file.speed, file.debugMode, file.autoMode);
		}
	}

private:
	void ButtonLayout() {
		ButtonSE = LoadSoundMem("materials\\SE\\Decision.ogg");
		ArrowSE = LoadSoundMem("materials\\SE\\cursor.ogg");
		int ButtonGraph[3];
		LoadDivGraph("materials\\Image\\ui\\Buttons.png", 3, 1, 3,
			124, 50, ButtonGraph);
		DecideButton.SetUI(ButtonGraph[2], ButtonGraph[0],
			display.GetScreenX() - 100, display.GetScreenY() - 50, 5);
		DecideButton.SetSE(ButtonSE);
		DecideButton.SetText("決定", GetColor(0, 0, 0));
		DecideButton.SetCorrection(10);

		ExitButton.SetUI(ButtonGraph[1], ButtonGraph[0],
			100, display.GetScreenY() - 50, 5);
		ExitButton.SetSE(ButtonSE);
		ExitButton.SetText("終了", GetColor(0, 0, 0));
		ExitButton.SetCorrection(10);

		int ConfigButtonGraph[2];
		LoadDivGraph("materials\\Image\\ui\\ConfigButton.png", 2, 1, 2,
			51, 51, ConfigButtonGraph);
		ConfigButton.SetUI(ConfigButtonGraph[0], ConfigButtonGraph[1],
			display.GetScreenX() - 100, 50, 5);
		ConfigButton.SetSE(ButtonSE);
	}

	int SelectMusic() {
		Graph background;
		background.setGraph(LoadGraph("materials\\Image\\background\\Bg_6009.jpg"));
		background.setDisplay(display);

		Graph menuBar;
		menuBar.setDisplay(display);
		menuBar.setGraph(LoadGraph("materials\\Image\\ui\\MenuBar.png"));
		Graph title;
		title.setDisplay(display);
		title.setGraph(LoadGraph("materials\\Image\\Text\\Select.png"));
		Graph musicTitleBar;
		musicTitleBar.setDisplay(display);
		musicTitleBar.setGraph(LoadGraph("materials\\Image\\ui\\UnderBar.png"));

		int arrowGraph[4];
		UI ArrowKey[2];
		LoadDivGraph("materials\\Image\\ui\\Arrow.png", 4, 2, 2,
			51, 51, arrowGraph);
		for (int i = 0; i < 2; i++) {
			ArrowKey[i].setGraph(arrowGraph[i + 2]);
			ArrowKey[i].SetSE(ArrowSE);
		}
		ArrowKey[0].SetPos(70, display.GetScreenY() / 2, 5);
		ArrowKey[1].SetPos(display.GetScreenX() - 70, display.GetScreenY() / 2, 5);

		int num = 0;
		int fileCount = file.fileCount;
		Graph musicImage[10];
		for (int i = 0; i < fileCount; i++) {
			char path[100];
			strcpy_s(path, file.imagepath);
			strcat_s(path, file.imageList[i]);
			musicImage[i].setDisplay(display);
			musicImage[i].setGraph(LoadGraph(path));
		}
		Graph musicImageFrame;
		musicImageFrame.setDisplay(display);
		musicImageFrame.setGraph(LoadGraph("materials\\Image\\ui\\MusicWindow.png"));

		FontSize(20);
		MyDrawString str(20);
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			inTouch.Update();
			if (input.PushOneframe_RIGHT() ||
				inTouch.ReleasedRangeBox(ArrowKey[1])) {
				num = (num + 1) % fileCount;
			}
			else if (input.PushOneframe_LEFT() ||
				inTouch.ReleasedRangeBox(ArrowKey[0])) {
				num = (num + (fileCount - 1)) % fileCount;
			}
			if (input.PushOneframe_Decide() ||
				inTouch.ReleasedRangeBox(DecideButton))
				return num;

			/*コンフィグ画面に遷移*/
			if (inTouch.ReleasedRangeBox(ConfigButton)) Config();
			/*アプリ終了*/
			if (inTouch.ReleasedRangeBox(ExitButton)) return -1;

			ClearDrawScreen();

			background.Draw_BackGround(255);
			menuBar.Draw(10, 20, 1);
			title.Draw(20, 30, 1);
			musicTitleBar.DrawExtend(200, display.GetScreenY() - 20, 
				display.GetScreenX() - 220, 80, 7);

			musicImage[(num + (fileCount - 1)) % fileCount].
				DrawExtend(display.GetScreenX() / 2 - 200, display.GetScreenY() / 2,
				170, 170, 5);
			musicImage[(num + 1) % fileCount].
				DrawExtend(display.GetScreenX() / 2 + 200, display.GetScreenY() / 2,
				170, 170, 5);
			musicImage[num].DrawExtend(display.GetScreenX() / 2, display.GetScreenY() / 2,
				220, 220, 5);
			musicImageFrame.DrawExtend(display.GetScreenX() / 2, display.GetScreenY() / 2,
				220, 220, 5);
			DrawString(230, display.GetScreenY() - 80, file.PlayMusicList[num], GetColor(0, 0, 0));


			DecideButton.SetID(inTouch.GetID_RangeBoxOneFrame(DecideButton));
			DecideButton.Draw(inTouch.PressRangeBox(DecideButton), str, 30);

			ExitButton.SetID(inTouch.GetID_RangeBoxOneFrame(ExitButton));
			ExitButton.Draw(inTouch.PressRangeBox(ExitButton), str, 30);

			ConfigButton.SetID(inTouch.GetID_RangeBoxOneFrame(ConfigButton));
			ConfigButton.Draw(inTouch.PressRangeBox(ConfigButton), str, 30);

			for (int i = 0; i < 2; i++) {
				ArrowKey[i].SetID(inTouch.GetID_RangeBoxOneFrame(ArrowKey[i]));
				ArrowKey[i].Draw();
			}
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
		}
		return -1;
	}

	bool Config() {
		Graph background;
		background.setDisplay(display);
		background.setGraph(LoadGraph("materials\\Image\\background\\Bg_2004.dds.png"));

		Graph menuBar;
		menuBar.setDisplay(display);
		menuBar.setGraph(LoadGraph("materials\\Image\\ui\\MenuBar.png"));
		Graph title;
		title.setDisplay(display);
		title.setGraph(LoadGraph("materials\\Image\\Text\\Config.png"));
		UI configText[3];
		for (int i = 0; i < 3; i++) {
			configText[i].setDisplay(display);
			configText[i].SetPos(200, 150 + i * 50, 5);
		}
		configText[0].setGraph(LoadGraph("materials\\Image\\Text\\SPEED.png"));
		configText[1].setGraph(LoadGraph("materials\\Image\\Text\\AutoMode.png"));
		configText[2].setGraph(LoadGraph("materials\\Image\\Text\\DebugMode.png"));

		/*左右だけ取得*/
		int arrowGraph[4];
		LoadDivGraph("materials\\Image\\ui\\Arrow.png", 4, 2, 2,
			51, 51, arrowGraph);
		UI configArrowKey[3][2];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				configArrowKey[i][j].setGraph(arrowGraph[j + 2]);
				configArrowKey[i][j].SetSE(ArrowSE);
				configArrowKey[i][j].SetPos(400 + j * 100, 150 + i * 50, 5);
			}
		}

		/*数字の画像取得*/
		int NumberGraph[10];
		LoadDivGraph("materials\\Image\\Text\\ConfigNumber.png", 10, 10, 1,
			15, 30, NumberGraph);
		Graph num[10];
		for (int i = 0; i < 10; i++) {
			num[i].setDisplay(display);
			num[i].setGraph(NumberGraph[i]);
		}

		int keyboradNum = 0;
		int count = file.fileCount;
		FontSize(20);
		MyDrawString str(20);

		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			inTouch.Update();

			ClearDrawScreen();
			background.Draw_BackGround(255);

			menuBar.Draw(10, 20, 1);
			title.Draw(20, 30, 1);
			for (int i = 0; i < 3; i++) configText[i].Draw();

			if (input.PushOneframe_Decide() ||
				inTouch.ReleasedRangeBox(DecideButton)) {
				file.UpdateConfig();
				return true;
			}

			/*矢印ボタンタッチ*/
			for (int i = 0; i < 3; i++) {
				if (inTouch.ReleasedRangeBox(configArrowKey[i][0])) {
					if (i == 0) {
						file.speed--;
						if (file.speed < 1) file.speed = 10;
					}
					else if (i == 1)
						file.autoMode = !file.autoMode;
					else if (i == 2)
						file.debugMode = !file.debugMode;
				}
				else if (inTouch.ReleasedRangeBox(configArrowKey[i][1])) {
					if (i == 0) {
						file.speed++;
						if (file.speed > 10) file.speed = 1;
					}
					else if (i == 1)
						file.autoMode = !file.autoMode;
					else if (i == 2)
						file.debugMode = !file.debugMode;
				}
			}

			/*キーボード操作*/
			if (input.PushOneframe_UP()) {
				keyboradNum = (keyboradNum + 2) % 3;
			}
			else if (input.PushOneframe_DOWN()) {
				keyboradNum = (keyboradNum + 1) % 3;
			}
			if (input.PushOneframe_LEFT()) {
				if (keyboradNum == 0) {
					file.speed--;
					if (file.speed < 1) file.speed = 10;
				}
				if (keyboradNum == 1)
					file.autoMode = !file.autoMode;
				else if (keyboradNum == 2)
					file.debugMode = !file.debugMode;
			}
			else if (input.PushOneframe_RIGHT()) {
				if (keyboradNum == 0) {
					file.speed++;
					if (file.speed > 10) file.speed = 1;
				}
				if (keyboradNum == 1)
					file.autoMode = !file.autoMode;
				else if (keyboradNum == 2)
					file.debugMode = !file.debugMode;
			}

			DecideButton.SetID(inTouch.GetID_RangeBoxOneFrame(DecideButton));
			DecideButton.Draw(inTouch.PressRangeBox(DecideButton), str, 30);

			if (file.speed < 10) num[file.speed].Draw(450, 150, 5);
			else {
				num[1].Draw(450, 150, 6);
				num[0].Draw(450, 150, 4);
			}
			num[file.autoMode].Draw(450, 200, 5);
			num[file.debugMode].Draw(450, 250, 5);

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 2; j++) {
					configArrowKey[i][j].SetID(inTouch.GetID_RangeBoxOneFrame(configArrowKey[i][j]));
					configArrowKey[i][j].Draw();
				}
			}
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
		}
		return false;
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

	InitGraph();
	InitSoundMem();
	DxLib_End();				// ＤＸライブラリ使用の終了処理
	return 0;				// ソフトの終了 
}
