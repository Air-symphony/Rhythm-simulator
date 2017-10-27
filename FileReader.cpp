#include "Dxlib.h"

class FileReader {
private: 
	int FileHandle;
	char FileListpath[50] = "materials\\NoteFile\\FileList.txt";
	int ConfigFileHandle;
	char Configpath[50] = "materials\\Config.dll";
	char *next;
	char *ctx;//“à•”—˜—p
public:
	char fileList[10][256];
	int fileCount = 0;
	bool autoMode, debugMode;

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

				next = strtok_s(string, ",", &ctx);
				strcpy_s(fileList[fileCount], next);
				fileCount++;
			}
		}
		FileRead_close(FileHandle);
	}
	void ReadConfig() {
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

				next = strtok_s(string, ":", &ctx);
				if (strcmp(next, "AutoMode") == 0) {
					next = strtok_s(NULL, ",", &ctx);
					if (strcmp(next, "0") == 0 || strcmp(next, "false") == 0)
						autoMode = false;
					else if (strcmp(next, "1") == 0 || strcmp(next, "true") == 0)
						autoMode = true;
				}
				else if (strcmp(next, "DebugMode") == 0) {
					next = strtok_s(NULL, ",", &ctx);
					if (strcmp(next, "0") == 0 || strcmp(next, "false") == 0)
						debugMode = false;
					else if (strcmp(next, "1") == 0 || strcmp(next, "true") == 0)
						debugMode = true;
				}
			}
		}
		FileRead_close(ConfigFileHandle);
	}

	bool UpdateConfig() {
		FILE *fp;
		int err_no = fopen_s(&fp, Configpath, "w");
		if (err_no != 0) return false;

		fprintf(fp, "AutoMode:");
		if (autoMode) fprintf(fp, "true");
		else fprintf(fp, "false");

		fprintf(fp, "\nDebugMode:");
		if (debugMode) fprintf(fp, "true");
		else fprintf(fp, "false");
		
		fclose(fp);
		return true;
	}
};