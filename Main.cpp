#include "PlayGame.cpp";

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
	int scene = 0;
	InputKey input;

public:
	GameSimulator(Display display) {
		//scene = 0;
		while (ProcessMessage() == 0) {
			scene = Menu();
			if (scene == 0) 
				break;
			else if (scene == 1) 
				Playgame(display);
			else if (scene == 2) 
				Config();
		}
	}

	void Playgame(Display display) {
		GameScreen game(display, 1);
	}

	void Config() {
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			printfDx("Config");
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
			if (input.PushOneframe(KEY_INPUT_RETURN))
				break;
		}
	}

	int Menu() {
		int type = 0;
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			if (input.PushOneframe(KEY_INPUT_UP)) {
				type = (type + 1) % scenecount;
			}
			else if (input.PushOneframe(KEY_INPUT_DOWN)) {
				type = (type - 1) % scenecount;
				if (type < 0) type += scenecount;
			}
			if (input.PushOneframe(KEY_INPUT_RETURN))
				return type;

			ClearDrawScreen();
			clsDx();
			printfDx("Please Select Type.\n");
			printfDx("Type = %d", type);
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
