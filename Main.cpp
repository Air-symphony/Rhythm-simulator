#include "PlayGame.cpp";

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
	int musiccount = 2 + 1;
	InputKey input;

public:
	GameSimulator(Display display) {
		//scene = 0;
		while (ProcessMessage() == 0) {
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
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			int scene = SelectNumber(2, musiccount);
			if (scene == 0)
				break;
			GameScreen game(display, scene);
		}
	}

	void Config() {
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			printfDx("Config");
			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 
			if (input.PushOneframe(KEY_INPUT_RETURN))
				break;
		}
	}

	int SelectNumber(int _type, int _scene) {
		int number = 1;
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			if (input.PushOneframe(KEY_INPUT_UP)) {
				number = (number + 1) % _scene;
			}
			else if (input.PushOneframe(KEY_INPUT_DOWN)) {
				number = (number - 1) % _scene;
				if (number < 0) number += _scene;
			}
			if (input.PushOneframe(KEY_INPUT_RETURN))
				return number;

			ClearDrawScreen();
			clsDx();
			if (_type == 1) {
				printfDx("Please Select Menu.\n");
				printfDx("1.Play 2.Config 0.Finish\n");
			}
			else if (_type == 2) {
				printfDx("Please Select Music.\n");
				printfDx("1.Snow Wings 2.TOKIMEKI 0.Cancel\n");
			}
			printfDx("number = %d", number);
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
