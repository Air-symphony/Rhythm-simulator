#include "Graphics.cpp"
#include "Note.cpp"
#include "InputKey.cpp"

/*�y�ȃf�[�^���X�g
id�ɑΉ�����.txt��ǂݍ��݁A���ʂ�y�ȃf�[�^��ԋp����
������
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
				// ��s�ǂ�
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

/*�y�ȃf�[�^
BGM,BPM,Note�Ȃǂ��Ǘ�
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
game���
Display display;
Music music;
Graph ring;
InputKey input;
GameScreen(Display _display, int id) 
id = �y�ȃf�[�^
*/
class GameScreen {
private:
	Display display;
	Music music;
	Graph ring;
	InputKey input;
public:
	/*id = �y�ȃf�[�^
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
			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 
			if (input.PushOneframe(KEY_INPUT_RETURN))
				break;
		}
	}

	void DrawScreen() {
		ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
	}
};
