#include "FileReader.cpp"
#include "InputKey.cpp"
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
	Graph noteGraph;
	InputKey input;
	double msec;
public:
	/*id = �y�ȃf�[�^
	*/
	GameScreen(Display _display, int id) {
		display = _display;
		ring.setGraph(LoadGraph("materials\\Image\\ring.png"));
		noteGraph.setGraph(LoadGraph("materials\\Image\\note1.png"));
		FileReader file;
		music = file.SelectReadFile(id);
		msec = 0;
		Start();
	}

	void Start() {
		/*�Q�[���J�n���Ԃ̎擾*/
		double start_time = GetNowCount();
		/*�Q�[�����e*/
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			/*�Q�[���i�s����*/
			msec = (double)((GetNowCount() - start_time) / 1000.0);

			DrawScreen();

			/*�e�m�[�c�̑���*/
			for (int i = 0; i < music.notecount; i++) {
				if (music.notes[i].getflag() && msec >= music.notes[i].gettime()) {
					music.notes[i].Move(15);
					noteGraph.DrawNote(music.notes[i].getend_x(), music.notes[i].getY(), 5, display);
				}
				/*��ʊO�ɏo���ꍇ*/
				if (music.notes[i].getY() > display.GetScreenY()) {
					music.notes[i].setflag(false);
				}
				/*
				int y = 64 + i * 16;
				DrawFormatString(0, y, GetColor(255, 255, 255), "%d", music.notes[i].getID());
				DrawFormatString(20, y, GetColor(255, 255, 255), "(%d,", music.notes[i].getType());
				DrawFormatString(40, y, GetColor(255, 255, 255), " %d,", music.notes[i].getend_x());
				DrawFormatString(70, y, GetColor(255, 255, 255), " %lf,", music.notes[i].gettime());
				DrawFormatString(130, y, GetColor(255, 255, 255), " %d)", music.notes[i].getY());
				*/
			}
			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 
			if (input.PushOneframe(KEY_INPUT_RETURN))
				break;
		}
	}

	/*��ɕ\����������́A�X�R�A�Ȃ�*/
	void DrawScreen() {
		printfDx("Game Screen\n");
		
		/*���ԕ\��*/
		int _msec = (int)(msec * 1000);
		DrawFormatString(0, 48, GetColor(255, 255, 255), "%d:", _msec / 1000);
		DrawFormatString(35, 48, GetColor(255, 255, 255), "%d", _msec % 1000);

		/*�����O�̕\��*/
		char c[256];
		sprintf_s(c, 256, "title = %s\n", music.title);
		printfDx(c);
		ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
	}
};
