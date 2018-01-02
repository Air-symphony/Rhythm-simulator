#include "PlayGame.cpp"
#include "FileReader.cpp"

/*�����̍����̎擾*/
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
		DecideButton.SetText("����", GetColor(0, 0, 0));
		DecideButton.SetCorrection(10);

		ExitButton.SetUI(ButtonGraph[1], ButtonGraph[0],
			100, display.GetScreenY() - 50, 5);
		ExitButton.SetSE(ButtonSE);
		ExitButton.SetText("�I��", GetColor(0, 0, 0));
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

		int arrowGraph[4];
		UI ArrowKey[4];
		LoadDivGraph("materials\\Image\\ui\\Arrow.png", 4, 2, 2,
			51, 51, arrowGraph);
		for (int i = 0; i < 4; i++) {
			ArrowKey[i].setGraph(arrowGraph[i]);
			ArrowKey[i].SetSE(ArrowSE);
		}
		ArrowKey[0].SetPos(display.GetScreenX() - 100, display.GetScreenY() - 300, 5);
		ArrowKey[1].SetPos(display.GetScreenX() - 100, display.GetScreenY() - 200, 5);
		ArrowKey[2].SetPos(display.GetScreenX() - 150, display.GetScreenY() - 250, 5);
		ArrowKey[3].SetPos(display.GetScreenX() - 50, display.GetScreenY() - 250, 5);

		int number = 0;
		int fileCount = file.fileCount;
		FontSize(20);
		MyDrawString str(20);
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			inTouch.Update();
			if (input.PushOneframe_DOWN() ||
				inTouch.ReleasedRangeBox(ArrowKey[1])) {
				number = (number + 1) % fileCount;
			}
			else if (input.PushOneframe_UP() ||
				inTouch.ReleasedRangeBox(ArrowKey[0])) {
				number = (number + (fileCount - 1)) % fileCount;
			}
			if (input.PushOneframe_Decide() ||
				inTouch.ReleasedRangeBox(DecideButton))
				return number;

			/*�R���t�B�O��ʂɑJ��*/
			if (inTouch.ReleasedRangeBox(ConfigButton)) Config();
			/*�I��*/
			if (inTouch.ReleasedRangeBox(ExitButton)) return -1;

			ClearDrawScreen();
			background.Draw_BackGround(255);

			menuBar.Draw(0, 20, 1);
			title.Draw(10, 30, 1);

			int y = fontsize * 6;
			for (int i = 0; i < file.fileCount; i++) {
				DrawFormatString(20, y + fontsize * i, GetColor(255, 255, 255), "%d:", i);
				DrawString(50, y + fontsize * i, file.PlayMusicList[i], GetColor(255, 255, 255));
				DrawString(370, y + fontsize * i, file.imageList[i], GetColor(255, 255, 255));
			}
			DrawFormatString(20, fontsize * 4, GetColor(255, 255, 255), "Select number : %d", number);


			DecideButton.SetID(inTouch.GetID_RangeBoxOneFrame(DecideButton));
			DecideButton.Draw(inTouch.PressRangeBox(DecideButton), str, 30);

			ExitButton.SetID(inTouch.GetID_RangeBoxOneFrame(ExitButton));
			ExitButton.Draw(inTouch.PressRangeBox(ExitButton), str, 30);

			ConfigButton.SetID(inTouch.GetID_RangeBoxOneFrame(ConfigButton));
			ConfigButton.Draw(inTouch.PressRangeBox(ConfigButton), str, 30);

			int IconCount = 2;
			for (int i = 0; i < IconCount; i++) {
				ArrowKey[i].SetID(inTouch.GetID_RangeBoxOneFrame(ArrowKey[i]));
				ArrowKey[i].Draw();
			}
			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 
		}
		return 0;
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

		/*���E�����擾*/
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

		/*�����̉摜�擾*/
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

			menuBar.Draw(0, 20, 1);
			title.Draw(10, 30, 1);
			for (int i = 0; i < 3; i++) configText[i].Draw();

			if (input.PushOneframe_Decide() ||
				inTouch.ReleasedRangeBox(DecideButton)) {
				file.UpdateConfig();
				return true;
			}

			/*���{�^���^�b�`*/
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

			/*�L�[�{�[�h����*/
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
			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 
		}
		return false;
	}
};

// �v���O������ WinMain ����n�܂�܂�
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
	DxLib_End();				// �c�w���C�u�����g�p�̏I������
	return 0;				// �\�t�g�̏I�� 
}
