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
	Graph noteGraph[4];
	InputKey input;//�{�^������
	double msec;//�o�ߎ���

	int score = 0;
	int combo = 0;
public:
	/*id = �y�ȃf�[�^
	*/
	GameScreen(Display _display, int id) {
		display = _display;
		ring.setGraph(LoadGraph("materials\\Image\\ring.png"));
		for (int i = 1; i <= 4; i++) {
			char _imagepath[256];
			strcpy_s(_imagepath, "materials\\Image\\Note");
			char _number[256];
			sprintf_s(_number, 256, "%d.png", i);
			strcat_s(_imagepath, _number);
			noteGraph[i - 1].setGraph(LoadGraph(_imagepath));
		}
		FileReader file;
		music = file.SelectReadFile(id);
		msec = 0;
		Start();
	}

	void Start() {
		/*�Q�[���J�n���Ԃ̎擾*/
		PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK);
		double start_time = GetNowCount();

		/*�������I������̃m�[�c��for������O��*/
		int process = 0;
		/*�Q�[�����e*/
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			/*�Q�[���i�s����*/
			msec = (double)((GetNowCount() - start_time) / 1000.0);
			//msec = (double)((GetNowCount() - start_time) / 1000.0) - 6.5f;
			DrawScreen();

			/*�e�m�[�c�̑���*/
			for (int i = process; i < music.notecount; i++) {
				/*��ʏ㕔�ő���*/
				if (music.notes[i].getflag() == 0) {
					/*���Ԃ�������\��*/
					if (music.notes[i].gettime() <= msec) {
						music.notes[i].setflag(1);
					}
					else {
						continue;
					}
				}
				/*�����ς݂̏ꍇ*/
				if (music.notes[i].getflag() == -1) {
					/*��ʉ��̏ꍇ�́Afor�Ɋ܂߂Ȃ�*/
					if (music.notes[i].getY() > display.GetScreenY()){
						process = i;
					}
					/*��ʓ��y�щ�ʏ�̏ꍇ*/
					else {
						/*��ʓ��ɂ��鎞�Ԃ̏ꍇ*/
						if (music.notes[i].gettime() <= msec) {
							music.notes[i].Move(15);
						}
					}
				}
				/*��ʓ��ɕ\������Ă���ꍇ*/
				if (music.notes[i].getflag() == 1) {
					/*��ʓ��̏ꍇ�͕\��*/
					if (music.notes[i].getY() <= display.GetScreenY()) {
						music.notes[i].Move(7);
						noteGraph[music.notes[i].getType() - 1].DrawNote(music.notes[i].getend_x(), music.notes[i].getY(), 5, display);
						/*������̏ꍇ*/
						if (-200 + display.GetScreenY() <= music.notes[i].getY() && 
							music.notes[i].getY() <= display.GetScreenY()) {
							/*�{�^����������Ă�����*/
							if (input.PushOneframe_PlayGame(music.notes[i].getend_x())) {
								music.notes[i].setflag(-1);
								if (-150 <= music.notes[i].getY() - display.GetScreenY() <= 50) {
									combo++;
									score += 500;
								}
								else {
									score += 300;
								}
							}
						}
					}
					/*��ʉ��̏ꍇ�͏���*/
					else {
						music.notes[i].setflag(-1);
					}
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
		DrawFormatString(0, 64, GetColor(255, 255, 255), "score = %d", score);
		DrawFormatString(0, 80, GetColor(255, 255, 255), "combo = %d", combo);

		/*�����O�̕\��*/
		char c[256];
		sprintf_s(c, 256, "title = %s\n", music.title);
		printfDx(c);
		ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
		DrawLine(0, display.GetScreenY() - 200, display.GetScreenX(), display.GetScreenY() - 200, GetColor(0, 255, 0));
	}
};
