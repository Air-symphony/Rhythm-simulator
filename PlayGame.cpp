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
	Graph Line;
	int SE[2];
	InputKey input;//�{�^������
	double msec;//�o�ߎ���(s)
	double d_msec;//�f�o�b�O�p�A�ω��l

	/*����ʒu�̍��W*/
	const double judgePos_y = display.GetScreenY() - 75.0;//����ʒu
	/*�܂̔���ʒu�̊Ԋu�@judgePos_x * (1,2,3,4,5)*/
	const double judgePos_x = display.GetScreenX() / 6.0;
	/*���b�����Ĕ���ʒu�ɂ��ǂ蒅����*/
	double speed = 0.5;

	int score = 0, combo = 0;

	char judge_text[5][10] = { "PERFECT","GREAT", "NICE", "BAD", "MISS" };
	bool printjudge = false;
	int judge_number = 0;/*������e��ۑ��A���t���[���ŏ�����*/
	double printjudge_time = 0.0;
	int printjudge_number = 0;

	/*�����O�m�[�c�̏ꏊ��ێ�*/
	class HoldKey {
	public:
		int noteID = 0;
		int key = 0;
		void Set(int _noteID, int _key) {
			noteID = _noteID;
			key = _key;
		}
	};
	int holdKeyCount = 0;
	/*
	int noteID = 0;
	int key = 0;
	*/
	HoldKey holdkey[2];

	/*AutoMode�̐^�U*/
	bool autoMode = false;

	const double PERFECTtime = 0.066;
	const double GREATtime = 0.090;
	const double NICEtime = 0.115;
	const double BADtime = 0.130;
public:
	/*id = �y�ȃf�[�^
	*/
	GameScreen(Display _display, int id) {
		display = _display;
		ring.setGraph(LoadGraph("materials\\Image\\ring.png"));
		ring.setDisplay(display);
		Line.setDisplay(display);
		for (int i = 1; i <= 4; i++) {
			char _imagepath[256];
			strcpy_s(_imagepath, "materials\\Image\\Note");
			char _number[256];
			sprintf_s(_number, 256, "%d.png", i);
			strcat_s(_imagepath, _number);
			noteGraph[i - 1].setGraph(LoadGraph(_imagepath));
			noteGraph[i - 1].setDisplay(display);
		}
		SE[0] = LoadSoundMem("materials\\SE\\PERFECT SE.ogg");
		SE[1] = LoadSoundMem("materials\\SE\\�t���b�N SE.wav");
		ChangeVolumeSoundMem(255 * 50 / 100, SE[0]);
		ChangeVolumeSoundMem(255 * 50 / 100, SE[1]);

		FileReader file;
		music = file.SelectReadFile(id);
		music.setX(judgePos_x);
		msec = 0;
		Start();
	}

	void Start() {
		/*�Q�[���J�n���Ԃ̎擾(ms)*/
		PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK, TRUE);
		/*�Ȃ��n�܂鎞�ԁ@GetNowCount() - music.offset*/
		double start_time = (double)GetNowCount() - (double)music.offset;

		/*�~�܂�n�߂�����(ms)*/
		double stop_time = 0.0;

		autoMode = true;
		/*�Q�[�����e*/
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			/*�Q�[���i�s����(s)*/
			msec = ((double)GetNowCount() - start_time) / 1000.0;
			//msec = (double)((GetNowCount() - start_time) / 1000.0) - 8.5f; �g�L���L

			/*�ꎞ��~*//*���X�Ƀm�[�c�������Ȃ邩���H*/
			if (stop_time <= 0 && input.PushOneframe_Stop()) {
				stop_time = (double)GetNowCount();
				StopSoundMem(music.soundHandle);
			}

			/*timestop��*/
			if (stop_time > 0) {
				msec = (stop_time - start_time) / 1000.0;
				DrawFormatString(display.GetScreenX() - 100, 10, GetColor(255, 255, 255), "%s", "��~��");
				
				if (input.PushOneframe_Decide()) {
					PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK, FALSE);
					double d_time = (double)GetNowCount();

					start_time += (d_time - stop_time);
					msec = ((double)GetNowCount() - start_time) / 1000.0;
					stop_time = 0.0;
				}
			}

			/*��ɕ\�����������*/
			ClearDrawScreen();
			clsDx();
			DrawScreen();

			/*������e��ۑ��A���t���[���ŏ�����*/
			judge_number = -1;
			int debugcount = 0;

			/*�e�m�[�c�̑���*/
			for (int i = 0; i < music.notecount; i++) {
				/*�����O�m�[�c�̕ێ����*/
				for (int j = 0; j < holdKeyCount; j++) {
					if (holdkey[j].key == 0) break;

					/*���������Ȃ����\��*/
					if (input.LongHoldKey(holdkey[j].key) || autoMode) {
						Line.Draw_LinkLine(
							(int)(music.notes[holdkey[j].noteID].getend_x() * judgePos_x),
							judgePos_y,
							music.notes[music.notes[holdkey[j].noteID].getlongNoteID()].getX(),
							music.notes[music.notes[holdkey[j].noteID].getlongNoteID()].getY()
						);
					}
					/*�������𗣂��Ă��܂�����*/
					else{
						music.notes[holdkey[j].noteID].setflag(-1);
						holdkey[j].noteID = holdkey[j].key = 0;
						holdKeyCount--;
						if (j == 0 && holdkey[1].key > 0) {
							holdkey[0].noteID = holdkey[1].noteID;
							holdkey[0].key = holdkey[1].key;
							holdkey[1].noteID = holdkey[1].key = 0;
							j--;
						}
						judge_number = 4;
					}
				}
				
				/*�����ς݂̏ꍇ*/
				if (music.notes[i].getflag() == -1) {
					/*��ʉ��̏ꍇ�́Afor�Ɋ܂߂Ȃ�*/
					if (music.notes[i].getY() > display.GetScreenY()) {
						continue;
					}
					/*��ʓ��y�щ�ʏ�̏ꍇ*/
					else {
						/*��ʓ��ɂ��鎞�Ԃ̏ꍇ*/
						if (music.notes[i].gettime() <= (msec + speed)) {
							double dt = (msec + speed) - (music.notes[i].gettime());
							music.notes[i].ToMove(judgePos_x, judgePos_y, dt, speed);
						}
					}
				}
				/*��ʏ㕔�ő���*/
				if (music.notes[i].getflag() == 0) {
					/*���Ԃ�������\��*/
					if (music.notes[i].gettime() <= (msec + speed)) {
						music.notes[i].setflag(1);
					}
				}
				/*��ʓ��ɕ\������Ă���ꍇ*/
				if (music.notes[i].getflag() == 1) {
					/*��ʓ��̏ꍇ�͕\��*/
					if (music.notes[i].getY() <= display.GetScreenY()) {
						double dt = (msec + speed) - music.notes[i].gettime();
						music.notes[i].ToMove(judgePos_x, judgePos_y, dt, speed);
						
						//�����O�m�[�c�A��������ꍇ
						if (music.notes[i].getlongNoteID() > 0) {
							/*��ʏ�ɂ���ꍇ*/
							if (music.notes[i].getflag() >= 0) {
								Line.Draw_LinkLine(music.notes[i].getX(), music.notes[i].getY(),
									music.notes[music.notes[i].getlongNoteID()].getX(),
									music.notes[music.notes[i].getlongNoteID()].getY()
								);
							}
						}
						//�t���b�N�̘A��������ꍇ
						if (music.notes[i].getlinkNoteID() > 0) {
							Line.Draw_LinkLine(music.notes[i].getX(), music.notes[i].getY(),
								music.notes[music.notes[i].getlinkNoteID()].getX(),
								music.notes[music.notes[i].getlinkNoteID()].getY()
							);
						}
						//��������������ꍇ
						if (music.notes[i].getsideNoteID() > 0) {
							//�������\�����̏ꍇ���A������id�̕����������ꍇ(�����I��������)
							if (music.notes[music.notes[i].getsideNoteID()].getflag() == 1 &&
								i > music.notes[i].getsideNoteID()
								) {
								Line.Draw_LinkLine(music.notes[i].getX(), music.notes[i].getY(),
									music.notes[music.notes[i].getsideNoteID()].getX(),
									music.notes[music.notes[i].getsideNoteID()].getY()
								);
								noteGraph[music.notes[music.notes[i].getsideNoteID()].getType() - 1]
									.DrawNote(music.notes[music.notes[i].getsideNoteID()].getX(), music.notes[music.notes[i].getsideNoteID()].getY(), speed, dt);

							}
						}
						/*�m�[�c�̕`��*/
						noteGraph[music.notes[i].getType() - 1].DrawNote(music.notes[i].getX(), music.notes[i].getY(), speed, dt);
						
						/*������e*/
						if (autoMode) AutoMode(i);
						else PlayKey(i);
					}
					/*��ʉ��̏ꍇ�͏���*/
					else {
						music.notes[i].setflag(-1);
						if (music.notes[i].getType() == 4) {
							music.notes[music.notes[i].getlongNoteID()].setflag(-1);
						}
						for (int j = 0; j < holdKeyCount; j++) {
							if (music.notes[holdkey[j].noteID].getlongNoteID() == i) {
								holdkey[j].noteID = holdkey[j].key = 0;
								holdKeyCount--;
								if (j == 0 && holdkey[1].key > 0) {
									holdkey[0].noteID = holdkey[1].noteID;
									holdkey[0].key = holdkey[1].key;
									holdkey[1].noteID = holdkey[1].key = 0;
									j--;
								}
							}
						}
						combo = 0;
						judge_number = 4;
					}
				}
				/*�f�o�b�O�p*/
				/*if (music.notes[i].gettime() <= (msec + speed) &&
					music.notes[i].getflag() != -1) {
					debugcount++;
					DrawFormatString(0, 96, GetColor(255, 255, 255), "ID type pos time (x,y)");
					int y = 96 + debugcount * 16;
					DrawFormatString(0, y, GetColor(255, 255, 255), "%d", music.notes[i].getID());
					DrawFormatString(30, y, GetColor(255, 255, 255), " %d ", music.notes[i].getType());
					DrawFormatString(50, y, GetColor(255, 255, 255), " %d ", music.notes[i].getfirst_x());
					DrawFormatString(70, y, GetColor(255, 255, 255), "-%d ", music.notes[i].getend_x());
					DrawFormatString(90, y, GetColor(255, 255, 255), " %lf ", music.notes[i].gettime());
					DrawFormatString(180, y, GetColor(255, 255, 255), "(%d", music.notes[i].getX());
					DrawFormatString(220, y, GetColor(255, 255, 255), ",%d)", music.notes[i].getY());
				}*/
			}
			DrawFormatString(0, 200, GetColor(255, 255, 255), "holdKeyCount = %d", holdKeyCount);
			for (int j = 0; j < 2; j++) {
				DrawFormatString(0, 230 + j * 30, GetColor(255, 255, 255), "(%d,", holdkey[j].noteID);
				DrawFormatString(40, 230 + j * 30, GetColor(255, 255, 255), "%d)", holdkey[j].key);
			}
			/*�����\������K�v�����������Ƃ�*/
			if (judge_number != -1) {
				printjudge_time = msec;
				printjudge_number = judge_number;
				printjudge = true;
			}
			/*���茋�ʂ̕\��(1�b)*/
			if (printjudge) {
				DrawFormatString(350, display.GetScreenY() / 2, GetColor(255, 255, 255), "%s", judge_text[printjudge_number]);
				printjudge = ((msec - printjudge_time) <= 1.0);
			}

			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 

						 /*�Q�[����ʂ̋����I��*/
			if (input.PushOneframe_Debug()) {
				StopSoundMem(music.soundHandle);
				DeleteSoundMem(music.soundHandle);
				DeleteSoundMem(SE[0]);
				DeleteSoundMem(SE[1]);
				break;
			}
		}
	}

	/*��ɕ\����������́A�X�R�A�Ȃ�*/
	void DrawScreen() {
		printfDx("Game Screen\n");

		/*���ԕ\��*/
		int _msec = (int)(msec * 1000.0);
		DrawFormatString(0, 48, GetColor(255, 255, 255), "%d:", _msec / 1000);
		DrawFormatString(35, 48, GetColor(255, 255, 255), "%d(s)", _msec % 1000);
		/*�X�R�A�\��*/
		DrawFormatString(0, 64, GetColor(255, 255, 255), "score = %d", score);
		DrawFormatString(0, 80, GetColor(255, 255, 255), "combo = %d", combo);

		/*1�t���[��������̕b��*/
		DrawFormatString(100, 48, GetColor(255, 255, 255), "%d(ms)", (int)((msec - d_msec) * 1000.0));
		d_msec = msec;

		char c[256];
		sprintf_s(c, 256, "title = %s\n", music.title);
		printfDx(c);

		/*�I�[�g���[�h�̐؂�ւ�*/
		if (input.PushOneframe_ChangeAutoMode()) {
			autoMode = !autoMode;
		}
		if (autoMode) {
			DrawString(0, display.GetScreenY() - 20, "AutoMode", GetColor(255, 255, 255));
		}

		/*�����O�̕\��*/
		//ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
		/*����͈͂̕\��*/
		DrawLine(0, (int)judgePos_y, display.GetScreenX(), (int)judgePos_y, GetColor(255, 0, 0));
		int y = (int)((judgePos_y / speed) * (PERFECTtime / 2.0));
		DrawLine(0, (int)judgePos_y - y, display.GetScreenX(), (int)judgePos_y - y, GetColor(0, 255, 0));
		DrawLine(0, (int)judgePos_y + y, display.GetScreenX(), (int)judgePos_y + y, GetColor(0, 255, 0));
		y = (int)((judgePos_y / speed) * (GREATtime / 2.0));
		DrawLine(0, (int)(judgePos_y - y), display.GetScreenX(), (int)(judgePos_y - y), GetColor(0, 0, 255));
		DrawLine(0, (int)(judgePos_y + y), display.GetScreenX(), (int)(judgePos_y + y), GetColor(0, 0, 255));
		y = (int)((judgePos_y / speed) * (NICEtime / 2.0));
		DrawLine(0, (int)judgePos_y - y, display.GetScreenX(), (int)judgePos_y - y, GetColor(255, 255, 0));
		DrawLine(0, (int)judgePos_y + y, display.GetScreenX(), (int)judgePos_y + y, GetColor(255, 255, 0));
	}

	/*�{�^������̋L�q*/
	void PlayKey(int i) {
		/*BAD������̏ꍇ*/
		if (abs(music.notes[i].gettime() - msec) <= BADtime / 2.0) {
			/*�����O�m�[�c�I�_�̔���*/
			for (int j = 0; j < holdKeyCount; j++) {
				if (holdkey[j].key == 0) break;
				
				if (music.notes[holdkey[j].noteID].getlongNoteID() == i) {
					/*�{�^����������Ă�����*/
					if (!input.LongHoldKey(holdkey[j].key)) {
						Judge(i);

						holdkey[j].noteID = holdkey[j].key = 0;
						holdKeyCount--;
						if (j == 0 && holdkey[1].key > 0) {
							holdkey[0].noteID = holdkey[1].noteID;
							holdkey[0].key = holdkey[1].key;
							holdkey[1].noteID = holdkey[1].key = 0;
						}
					}
				}
			}
			/*�m�[�c�̔���*/
			if (input.PushOneframe_PlayGame(music.notes[i].getend_x())) {
				Judge(i);
				music.notes[i].setflag(-1);
			}
		}
	}

	void Judge(int i) {
		/*�����ꂽ���Ԃ̎擾*/
		double _clear_time = abs(music.notes[i].gettime() - msec);

		if (_clear_time <= GREATtime / 2.0) {
			/*�����O�m�[�c�Ȃ�*/
			if (music.notes[i].getType() == 4) {
				holdkey[holdKeyCount].Set(i, music.notes[i].getend_x());
				holdKeyCount++;
			}
		}
		if (_clear_time <= PERFECTtime / 2.0) {
			combo++;
			score += 300;
			PlaySoundMem(SE[0], DX_PLAYTYPE_BACK, TRUE);
			judge_number = 0;
			return;
		}
		else if (_clear_time <= GREATtime / 2.0) {
			combo++;
			score += 150;
			judge_number = 1;
			return;
		}
		else if (_clear_time <= NICEtime / 2.0) {
			combo = 0;
			score += 100;
			judge_number = 2;
			return;
		}
		else {
			combo = 0;
			score += 50;
			judge_number = 3;
			return;
		}
	}

	/*�I�[�g���[�h�̓��e*/
	void AutoMode(int i) {
		/*���S�o�[���z�����炷��*/
		if (music.notes[i].gettime() <= msec) {
			/*�t���b�N��*/
			if (music.notes[i].getType() == 1 || music.notes[i].getType() == 3) {
				PlaySoundMem(SE[1], DX_PLAYTYPE_BACK, TRUE);
			}
			else {
				PlaySoundMem(SE[0], DX_PLAYTYPE_BACK, TRUE);
			}
			combo++;
			score += 300;
			judge_number = 0;
			music.notes[i].setflag(-1);
			/*�����O�m�[�c�Ȃ�*/
			if (music.notes[i].getType() == 4) {
				holdkey[holdKeyCount].Set(i, music.notes[i].getend_x());
				holdKeyCount++;
			}
			for (int j = 0; j < holdKeyCount; j++) {
				if (music.notes[holdkey[j].noteID].getlongNoteID() == i) {
					holdkey[j].noteID = holdkey[j].key = 0;
					holdKeyCount--;
					if (j == 0 && holdkey[1].key > 0) {
						holdkey[0].noteID = holdkey[1].noteID;
						holdkey[0].key = holdkey[1].key;
						holdkey[1].noteID = holdkey[1].key = 0;
						j--;
					}
				}
			}
		}
	}

	/*��Βl�̎擾*/
	double abs(double t) {
		if (t < 0) return (-t);
		return t;
	}
};