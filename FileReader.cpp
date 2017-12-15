#include "Dxlib.h"

class FileReader {
private: 
	int FileHandle = 0;
	char FileListpath[50] = "materials\\NoteFile\\FileList.txt";
	int ConfigFileHandle = 0;
	char Configpath[25] = "materials\\Config.dll";
	char *next;
	char *ctx;//“à•”—˜—p
public:
	char AutoMode[15] = "AutoMode";
	char DebugMode[15] = "DebugMode";
	char SPEED[15] = "SPEED";
	char connectText[2] = ":";
	char True[5] = "true";
	char False[6] = "false";

	char PlayMusicList[10][256];
	char fileList[10][256];
	int fileCount = 0;
	bool autoMode = false, debugMode = false;
	int speed = 1;

	void ReadFileList() {
		FileHandle = FileRead_open(FileListpath);
		if (FileHandle == 0) {
			char error[256];
			strcpy_s(error, FileListpath);
			strcat_s(error, " is not found.");
			throw error;
		}
		else {
			fileCount = 0;
			ClearDrawScreen();
			while (FileRead_eof(FileHandle) == 0) {
				char string[256];
				FileRead_gets(string, 256, FileHandle);

				next = strtok_s(string, ":", &ctx);
				strcpy_s(PlayMusicList[fileCount], next);
				next = strtok_s(NULL, ",", &ctx);
				if (next != NULL) {
					strcpy_s(fileList[fileCount], next);
				}
				else {
					strcpy_s(fileList[fileCount], PlayMusicList[fileCount]);
				}
				fileCount++;
			}
		}
		FileRead_close(FileHandle);
	}
	void ReadConfig() {
		try {
			ConfigFileHandle = FileRead_open(Configpath);
			if (ConfigFileHandle == 0) {
				char error[256];
				strcpy_s(error, Configpath);
				strcat_s(error, " is not found.");
				throw error;
			}
			else {
				ClearDrawScreen();
				while (FileRead_eof(ConfigFileHandle) == 0) {
					char string[256];
					FileRead_gets(string, 256, ConfigFileHandle);

					next = strtok_s(string, connectText, &ctx);
					if (strcmp(next, AutoMode) == 0) {
						next = strtok_s(NULL, ",", &ctx);
						if (strcmp(next, "0") == 0 || strcmp(next, False) == 0)
							autoMode = false;
						else if (strcmp(next, "1") == 0 || strcmp(next, True) == 0)
							autoMode = true;
					}
					else if (strcmp(next, DebugMode) == 0) {
						next = strtok_s(NULL, ",", &ctx);
						if (strcmp(next, "0") == 0 || strcmp(next, False) == 0)
							debugMode = false;
						else if (strcmp(next, "1") == 0 || strcmp(next, True) == 0)
							debugMode = true;
					}
					else if (strcmp(next, SPEED) == 0) {
						speed = 0;
						next = strtok_s(NULL, ",", &ctx);
						for (int i = 0; i < 2; i++) {
							int c = (int)(next[i] - '0');
							for (int j = 0; j < (1 - i); j++) {
								c *= 10;
							}
							speed += c;
						}
					}
				}
				FileRead_close(ConfigFileHandle);
			}
		}
		catch (char* text) {
			ClearDrawScreen();
			clsDx();
			printfDx(text);
			ScreenFlip();
			WaitKey();
			clsDx();
		}
	}

	bool UpdateConfig() {
		FILE *fp;
		int err_no = fopen_s(&fp, Configpath, "w");
		if (err_no != 0) return false;

		ClearDrawScreen();
		char string[100];
		strcpy_s(string, "");
		strcat_s(string, 15, AutoMode);
		strcat_s(string, connectText);
		fprintf(fp, string);

		if (autoMode) fprintf(fp, True);
		else fprintf(fp, False);

		strcpy_s(string, "\n");
		strcat_s(string, DebugMode);
		strcat_s(string, connectText);
		fprintf(fp, string);
		if (debugMode) fprintf(fp, True);
		else fprintf(fp, False);

		strcpy_s(string, "\n");
		strcat_s(string, SPEED);
		strcat_s(string, connectText);
		fprintf(fp, string);
		if (speed == 10) fprintf(fp, "10");
		else {
			fprintf(fp, "0");
			char c[5];
			sprintf_s(c, 5, "%d", speed);
			fprintf(fp, c);
		}
		
		fclose(fp);
		return true;
	}
};