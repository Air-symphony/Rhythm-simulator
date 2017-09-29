#include "Graphics.cpp"
#include "Note.cpp"

/*�y�ȃf�[�^
BGM,BPM,Note�Ȃǂ��Ǘ�
int sound, bpm, notecount;
Graph notegraph[5];
Note notes[1000];
*/
class Music{
public:
	char title[256];
	float bpm;
	int sound, notecount;
	Graph notegraph[5];
	Note notes[1000];
	Rhythm rhythm;

	Music() {

	}
};

/*�y�ȃf�[�^���X�g
id�ɑΉ�����.txt��ǂݍ��݁A���ʂ�y�ȃf�[�^��ԋp����
������
*/
class FileReader :public Music{
private:
	char filepath[256] = "materials\\";
	int FileHandle;
	int soundHandle;
	char *next;
	char *ctx;//�������p
public:
	FileReader() {
		sound = notecount = NULL;
		bpm = NULL;
	}
	//���ݒ�ABPM��
	void SetConfig() {
		char string[256];
		while (FileRead_eof(FileHandle) == 0)
		{
			// ��s�ǂݍ���
			FileRead_gets(string, 256, FileHandle);
			//�擪���番�����Ă����@a,b,c = a + b,c
			next = strtok_s(string, " ", &ctx);
			//������̔�r
			if (strcmp(next, "#Title") == 0) {
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(title, 256, "%s", next);
			}
			else if (strcmp(next, "#BackGround") == 0) {
				char background_name[256];
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(background_name, 256, "%s", next);
			}
			else if (strcmp(next, "#Song") == 0) {
				char sound_name[256];
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(sound_name, 256, "%s", next);
				try {
					char _filepath[256];
					strcpy_s(_filepath, filepath);
					strcat_s(_filepath, "Music\\");
					strcat_s(_filepath, sound_name);
					soundHandle = LoadSoundMem(_filepath);
					if (soundHandle == -1) {
						throw "Not SoundFile";
					}
				}
				catch (char* text) {
					clsDx();
					printfDx(text);
					ScreenFlip();
					WaitKey();
				}
			}
			else if (strcmp(next, "#BPM") == 0) {
				next = strtok_s(NULL, "", &ctx);
				char char_bpm[256];
				sprintf_s(char_bpm, 256, "%s", next);
				bpm = (float)atof(char_bpm);
			}
			else if (strcmp(next, "#end") == 0) {
				break;
			}
		}
	}

	void SetNote() {
		const int cher_size = 64;
		int noteID = 0;//�m�[�c�ɔԍ�������U��
		int bar_number = 0;//�����ߖڂ�
		while (FileRead_eof(FileHandle) == 0) {
			/*�ǂݍ��ݎ��̉��ۑ��p*/
			int _type[32], _first_x[32], _end_x[32];
			/*���̐߂̃m�[�c��*/
			int _notecount = 0;
			/*���̏��߂������q��*/
			int rhythm_note = 0;

			char string[256];
			FileRead_gets(string, 256, FileHandle);
			next = strtok_s(string, ",", &ctx);
			/*������̔�r*/
			if (strcmp(next, "#0") == 0 || strcmp(next, "#1") == 0) {
				/*�����ߖڂ��J�E���g*/
				if (strcmp(next, "#0") == 0) {
					bar_number++;
				}
				next = strtok_s(NULL, ":", &ctx);//�����ߖ�
				next = strtok_s(NULL, ":", &ctx);//rhythm & type note

				char _char[cher_size];
				sprintf_s(_char, cher_size, "%s", next);
				for (int i = 0; i < cher_size; i++) {
					/*�󕶎��ł���Γǂݍ��ݏI��*/
					if (strcmp(&_char[i], "") == 0) {
						rhythm_note = i;
						break;
					}
					/*�����R�[�h���琔�����o�� "0" = 0*/
					int _note = (int)(_char[i] - '0');
					if (_note > 0) {
						_type[_notecount] = _note;
						_notecount++;
					}
				}
				next = strtok_s(NULL, ":", &ctx);//first_x note
				sprintf_s(_char, cher_size, "%s", next);
				for (int i = 0; i < cher_size; i++) {
					if (strcmp(&_char[i], "") == 0) {
						break;
					}
					int _note = (int)(_char[i] - '0');
					_first_x[i] = _end_x[i] = _note;
				}
				next = strtok_s(NULL, ":", &ctx);//end_x note
				sprintf_s(_char, cher_size, "%s", next);
				for (int i = 0; i < cher_size; i++) {
					if (strcmp(&_char[i], "") == 0) {
						break;
					}
					int _note = (int)(_char[i] - '0');
					if (_note < 1 || 5 < _note) {
						break;
					}
					_end_x[i] = _note;
				}
			}
			/*#ChangeBPM 9, 167.00*/
			else if (strcmp(next, "#ChangeBPM") == 0) {
				next = strtok_s(NULL, ",", &ctx);
				next = strtok_s(NULL, " ", &ctx);
				char _char[cher_size];
				sprintf_s(_char, cher_size, "%s", next);
				bpm = (float)atof(_char);
			}
			for (int i = 0; i < _notecount; i++) {
				notes[noteID].setNote(noteID, _type[i], _first_x[i], _end_x[i], bar_number);
				noteID++;
			}
			notecount += _notecount;

			/*�f�o�b�O�p*/
			int _testID = noteID - _notecount;
			int y = 64 + 16 * bar_number;
			int x = 0;
			DrawFormatString(x, y, GetColor(255, 255, 255), "%d:", bar_number);
			x += 30;
			DrawFormatString(x, y, GetColor(255, 255, 255), "%d/", _notecount);
			x += 25; 
			DrawFormatString(x, y, GetColor(255, 255, 255), "%d, ", rhythm_note);
			x += 30;
			for (int i = 0; i < _notecount; i++) {
				DrawFormatString(x, y, GetColor(255, 255, 255), "%d", notes[_testID + i].getID());
				x += 20;
				DrawFormatString(x, y, GetColor(255, 255, 255), "(%d,", notes[_testID + i].getType());
				x += 20;
				DrawFormatString(x, y, GetColor(255, 255, 255), " %d,", notes[_testID + i].getfirst_x());
				x += 20;
				DrawFormatString(x, y, GetColor(255, 255, 255), " %d)", notes[_testID + i].getend_x());
				x += 35;
			}
		}
		FileRead_close(FileHandle);
		DrawFormatString(0, 48, GetColor(255, 255, 255), "fullcombo = %d", notecount);
		ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������
		WaitKey();
	}

	void ReadFile(int id) {
		try {
			char _filepath[256];
			strcpy_s(_filepath, filepath);
			strcat_s(_filepath, "NoteFile\\");
			if (id == 1) {
				strcat_s(_filepath, "TOKIMEKI.txt");
			}
			else if (id == 2) {
				strcat(_filepath, "Snow Wings.txt");
			}
			FileHandle = FileRead_open(_filepath);
			if (FileHandle == 0) {
				throw "Not NoteFile";
			}
			else {
				SetConfig();
				SetNote();
			}
		}
		catch (char* text) {
			clsDx();
			printfDx(text);
			ScreenFlip();
			WaitKey();
		}
		//ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 
		//WaitKey();
	}
};