#pragma once
#include <string.h>

class FilePath {
private:
	char path[100] = "";

	char materialDir[11] = "materials\\";
	char NoteFileDir[10] = "NoteFile\\";
	char MusicDir[7] = "Music\\";
	char SEDir[4] = "SE\\";

	char ImageDir[7] = "Image\\";
	char backgroundDir[12] = "background\\";
	char LabelDir[7] = "Label\\";
	char TextDir[6] = "Text\\";
	char uiDir[4] = "ui\\";

	char ConfigName[11] = "Config.dll";
	char FileListName[13] = "FileList.txt";

	void GetMaterialpath() {
		strcpy_s(path, materialDir);
	}

	/*Notefile*/
	void GetNoteFilePath() {
		GetMaterialpath();
		strcat_s(path, NoteFileDir);
	}

	/*Music*/
	void GetMusicPath() {
		GetMaterialpath();
		strcat_s(path, MusicDir);
	}

	/*SE*/
	void GetSEPath() {
		GetMaterialpath();
		strcat_s(path, SEDir);
	}

	/*Image*/
	void GetImagePath() {
		GetMaterialpath();
		strcat_s(path, ImageDir);
	}

	/*Label*/
	void GetLabelPath() {
		GetImagePath();
		strcat_s(path, LabelDir);
	}

	/*ui*/
	void GetUIPath() {
		GetImagePath();
		strcat_s(path, uiDir);
	}

	/*background*/
	void GetbackgroundPath() {
		GetImagePath();
		strcat_s(path, backgroundDir);
	}

	/*Text*/
	void GetTextPath() {
		GetImagePath();
		strcat_s(path, TextDir);
	}

public:
	/*Config.dll*/
	char *GetConfigFile() {
		GetMaterialpath();
		strcat_s(path, ConfigName);
		return path;
	}

	/*FileList.txt*/
	char *GetFileList() {
		GetNoteFilePath();
		strcat_s(path, FileListName);
		return path;
	}

	/*Notefile*/
	char *GetNoteFilePath(char filename[]) {
		GetNoteFilePath();
		strcat_s(path, filename);
		return path;
	}

	/*Music*/
	char *GetMusicPath(char filename[]) {
		GetMusicPath();
		strcat_s(path, filename);
		return path;
	}

	/*SE*/
	char *GetSEPath(char filename[]) {
		GetSEPath();
		strcat_s(path, filename);
		return path;
	}

	/*Image*/
	char *GetImagePath(char filename[]) {
		GetImagePath();
		strcat_s(path, filename);
		return path;
	}

	/*Label*/
	char *GetLabelPath(char filename[]) {
		GetLabelPath();
		strcat_s(path, filename);
		return path;
	}
	/*ui*/
	char *GetUIPath(char filename[]) {
		GetUIPath();
		strcat_s(path, filename);
		return path;
	}

	/*background*/
	char *GetbackgroundPath(char filename[]) {
		GetbackgroundPath();
		strcat_s(path, filename);
		return path;
	}

	/*Text*/
	char *GetTextPath(char filename[]) {
		GetTextPath();
		strcat_s(path, filename);
		return path;
	}
};

static FilePath filepath;