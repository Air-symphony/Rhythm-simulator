#include "Graphics.cpp"
#include "Note.cpp"
#include "InputKey.cpp"

/*楽曲データリスト
idに対応する.txtを読み込み、譜面や楽曲データを返却する
未実証
*/
class MusicList {
public:
	int sound, bpm, notecount;

	MusicList() {
		sound = bpm = notecount = NULL;
	}
	void setMusicList(int id) {
		if (id == 1) {
			int FileHandle;
			char String[256];

			try {
				FileHandle = FileRead_open("C:\\Users\\admin\\Desktop\\ui\\sample.txt");
				if (FileHandle == 0) {
					throw "Not File";
				}
				// 一行読む
				FileRead_gets(String, 256, FileHandle);

				while (String != NULL) {
					FileRead_gets(String, 256, FileHandle);
				}
			}
			catch (char* text) {
				clsDx();
				printfDx(text);
			}
			//sound = NULL;
			bpm = 138;
			notecount = 466;
		}
	}
};

/*楽曲データ
BGM,BPM,Noteなどを管理
int sound, bpm, notecount;
Graph notegraph[5];
Note notes[1000];
*/
class Music :public MusicList {
private:
	int sound, bpm, notecount;
	Graph notegraph[5];
	Note notes[1000];
public:
	Music() {
		sound = bpm = notecount = NULL;
	}
	void setMusic(int id) {
		setMusicList(id);
		for (int i = 0; i < notecount; i++) {
			notes[i].setNote(i + 1, 1, (i % 5) + 1, i * 30);
		}
	}
};

/*
game画面
Display display;
Music music;
Graph ring;
InputKey input;
GameScreen(Display _display, int id) 
id = 楽曲データ
*/
class GameScreen {
private:
	Display display;
	Music music;
	Graph ring;
	InputKey input;
public:
	/*id = 楽曲データ
	*/
	GameScreen(Display _display, int id) {
		display = _display;
		ring.setGraph(LoadGraph("C:\\Users\\admin\\Desktop\\ui\\ring.png"));
		//music.setMusic(id);
		Start();
	}

	void Start() {
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			printfDx("Game Screen");
			DrawScreen();
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
			if (input.PushOneframe(KEY_INPUT_RETURN))
				break;
		}
	}

	void DrawScreen() {
		ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
	}
};
