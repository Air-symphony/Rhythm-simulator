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

	/*����ʒu��y���W*/
	const double judgePos = display.GetScreenY() - 75.0;//����ʒu
	/*���b�����Ĕ���ʒu�ɂ��ǂ蒅����*/
	double speed = 0.6;

	int score = 0, combo = 0;
	int judge_number = 0;

	double PERFECTtime = 0.033;
	double GREATtime = 0.060;
	double NICEtime = 0.085;
	double BADtime = 0.110;
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
		double start_time = GetNowCount();
		PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK);

		/*�������I������̃m�[�c��for������O��*/
		int process = 0;
		/*�Q�[�����e*/
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			ClearDrawScreen();
			clsDx();
			/*�Q�[���i�s����*/
			msec = (double)((GetNowCount() - start_time) / 1000.0) + 0.5;
			//msec = (double)((GetNowCount() - start_time) / 1000.0) - 8.5f;
			DrawScreen();

			bool printjudge = false;
			int debugcount = 0;
			/*�e�m�[�c�̑���*/
			for (int i = 0; i < music.notecount; i++) {
				/*�����ς݂̏ꍇ*/
				if (music.notes[i].getflag() == -1) {
					/*��ʉ��̏ꍇ�́Afor�Ɋ܂߂Ȃ�*/
					if (music.notes[i].getY() > display.GetScreenY()) {
						//continue;
					}
					/*��ʓ��y�щ�ʏ�̏ꍇ*/
					else {
						/*��ʓ��ɂ��鎞�Ԃ̏ꍇ*/
						if (music.notes[i].gettime() <= (msec + speed)) {
							double dt = (msec + speed) - (music.notes[i].gettime());
							music.notes[i].ToMove(judgePos, dt, speed);
						}
					}
				}
				/*��ʏ㕔�ő���*/
				if (music.notes[i].getflag() == 0) {
					/*���Ԃ�������\��*/
					if (music.notes[i].gettime() <= (msec + speed)) {
						music.notes[i].setflag(1);
					}
					else {
						//continue;
					}
				}
				/*��ʓ��ɕ\������Ă���ꍇ*/
				if (music.notes[i].getflag() == 1) {
					/*��ʓ��̏ꍇ�͕\��*/
					if (music.notes[i].getY() <= display.GetScreenY()) {
						double dt = (msec + speed) - music.notes[i].gettime();
						music.notes[i].ToMove(judgePos, dt, speed);
						noteGraph[music.notes[i].getType() - 1].DrawNote(music.notes[i].getend_x(), music.notes[i].getY(), 5, display);
						
						int n = Judge(i);
						if (-1 != n && !printjudge){
							judge_number = n;
							printjudge = true;
						}
					}
					/*��ʉ��̏ꍇ�͏���*/
					else {
						music.notes[i].setflag(-1);
					}
				}
				/*�f�o�b�O�p*/
				if (music.notes[i].gettime() <= (msec + speed) &&
					music.notes[i].getflag() != -1) {
					debugcount++;
					DrawFormatString(0, 96, GetColor(255, 255, 255), "ID type,pos,time, Y");
					int y = 96 + debugcount * 16;
					DrawFormatString(0, y, GetColor(255, 255, 255), "%d", music.notes[i].getID());
					DrawFormatString(30, y, GetColor(255, 255, 255), "[%d ", music.notes[i].getType());
					DrawFormatString(50, y, GetColor(255, 255, 255), " %d ", music.notes[i].getend_x());
					DrawFormatString(70, y, GetColor(255, 255, 255), " %lf ", music.notes[i].gettime());
					DrawFormatString(160, y, GetColor(255, 255, 255), " %d]", music.notes[i].getY());
				}
			}

			if (printjudge) {
				//char text[5][10] = {"PERFECT","GREAT", "NICE", "BAD", "MISS" };
				//DrawFormatString(350, display.GetScreenY / 2, GetColor(255, 255, 255), "%d", text[judge_number]);
			}

			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 
			if (input.PushOneframe_Debug()) {
				StopSoundMem(music.soundHandle);
				DeleteSoundMem(music.soundHandle);
				break;
			}
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

		char c[256];
		sprintf_s(c, 256, "title = %s\n", music.title);
		printfDx(c);

		/*�����O�̕\��*/
		//ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
		
		DrawLine(0, (int)judgePos, display.GetScreenX(), (int)judgePos, GetColor(255, 0, 0));
		int y = (int)((judgePos / speed) * (PERFECTtime / 2.0));
		DrawLine(0, (int)judgePos - y, display.GetScreenX(), (int)judgePos - y, GetColor(0, 255, 0));
		DrawLine(0, (int)judgePos + y, display.GetScreenX(), (int)judgePos + y, GetColor(0, 255, 0));
		y = (int)((judgePos / speed) * (GREATtime / 2.0));
		DrawLine(0, (int)(judgePos - y), display.GetScreenX(), (int)(judgePos - y), GetColor(0, 0, 255));
		DrawLine(0, (int)(judgePos + y), display.GetScreenX(), (int)(judgePos + y), GetColor(0, 0, 255));
		y = (int)((judgePos / speed) * (NICEtime / 2.0));
		DrawLine(0, (int)judgePos - y, display.GetScreenX(), (int)judgePos - y, GetColor(255, 255, 0));
		DrawLine(0, (int)judgePos + y, display.GetScreenX(), (int)judgePos + y, GetColor(255, 255, 0));
	}

	/*�{�^������̋L�q*/
	/*return -1, 0,1,2,3*/
	int Judge(int i) {
		/*BAD������̏ꍇ*/
		if (abs(music.notes[i].gettime() - msec) <= BADtime / 2.0) {
			/*�����ꂽ���Ԃ̎擾*/
			double _clear_time = abs(music.notes[i].gettime() - msec);

			/*�{�^����������Ă�����*/
			if (input.PushOneframe_PlayGame(music.notes[i].getend_x())) {
				music.notes[i].setflag(-1);
				if (_clear_time <= PERFECTtime / 2.0) {
					combo++;
					score += 600;
					return 0;
				}
				else if (_clear_time <= GREATtime / 2.0) {
					combo++;
					score += 400;
					return 1;
				}
				else if (_clear_time <= NICEtime / 2.0) {
					combo = 0;
					score += 200;
					return 2;
				}
				else {
					combo = 0;
					score += 100;
					return 3;
				}
			}
		}
		return -1;
	}

	/*��Βl�̎擾*/
	double abs(double t) {
		if (t < 0) return (-t);
		return t;
	}
};
