#include "Graphics.cpp"
#include "Note.cpp"
#include "ReadNoteFile.cpp"
#include "InputKey.cpp"

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
		music.setMusic(id);
		Start();
	}

	void Start() {
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			printfDx("Game Screen\n");
			char c[256];
			//文字列の結合
			sprintf_s(c, 256, "title = %s\n", music.title);
			printfDx(c); 
			/*char b[256];
			sprintf_s(b, 256, "background = %s\n", music.background_name);
			printfDx(b); 
			char a[256];
			sprintf_s(a, 256, "songfile = %s\n", music.sound_name);
			printfDx(a);*/
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
