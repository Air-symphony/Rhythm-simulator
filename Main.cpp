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
/*
�I�����
int scenecount = 3;
=> 0 �I���A1 �Q�[����ʁA2 �ݒ���
int scene = 0;
InputKey input;
*/
class GameSimulator {
private:
	int scenecount = 3;
	int musiccount = 3 + 1;
	InputKey input;
	FileReader file;
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
			int scene = SelectNumber(2, musiccount);
			if (scene == 0)
				break;
			GameScreen game(display, file.fileList[scene - 1], file.debugMode, file.autoMode);
		}
	}

	void Config() {
		SelectNumber(3, 2);
		file.UpdateConfig();
		//file.ReadConfig();
	}

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
				int y = 100;
				DrawString(0, 0, "Config", GetColor(255, 255, 255));
				DrawString(20, y, "AutoMode:", GetColor(255, 255, 255));
				DrawString(20, y + 30, "DebugMode:", GetColor(255, 255, 255));
				if (number == 0)
					DrawString(20, y, "AutoMode", GetColor(255, 0, 0));
				else if (number == 1)
					DrawString(20, y + 30, "DebugMode", GetColor(255, 0, 0));

				if (input.PushOneframe_LEFT() || input.PushOneframe_RIGHT()) {
					if (number == 0)
						file.autoMode = !file.autoMode;
					else if (number == 1)
						file.debugMode = !file.debugMode;
				}

				int x = 140;
				if (file.autoMode)
					DrawString(x, y, "true", GetColor(255, 255, 255));
				else
					DrawString(x, y, "false", GetColor(255, 255, 255));

				if (file.debugMode)
					DrawString(x, y + 30, "true", GetColor(255, 255, 255));
				else
					DrawString(x, y + 30, "false", GetColor(255, 255, 255));
			}
			DrawFormatString(20, fontsize * 2, GetColor(255, 255, 255), "Select number : %d", number);
			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 
		}
		return 0;
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

	DxLib_End();				// �c�w���C�u�����g�p�̏I������
	return 0;				// �\�t�g�̏I�� 
}
