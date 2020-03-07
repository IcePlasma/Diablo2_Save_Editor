#ifndef D2_SAVE_EDITOR
#define D2_SAVE_EDITOR
#define NAME_SIZE 15
#define NAME_START 20
#define HEADER_SIZE 765
#define CHECKSUM_INDEX 12
#define CLASS_INDEX 40
#define LEVEL_INDEX 43
#define AMAZON 0
#define SORCERESS 1
#define NECROMANCER 2
#define PALADIN 3
#define BARBARIAN 4
#define DRUID 5
#define ASSASSIN 6
#define EXTENSION_LENGTH 4
#define FILE_EXTENSION ".d2s"
#define MINIMUM_FILE_LENGTH 6
#define MAXIMUM_LEVEL 150
#define LEVEL_INDEX 43
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// A basic save editor for .d2s (Diablo 2 save) files. Supports changing classes, name, and level
class D2SaveEditor
{
public:
	~D2SaveEditor();
	D2SaveEditor();
	void RunEditor();
	void RunEditor(string&);

private:
	const string filePathPrompt = "Please enter the full file path to the save file you wish to edit.";
	char* characterBuffer;
	int fileSize;
	int bufferSize;
	string currentFilePath;
	bool nameChanged;
	bool changesToSave;
	bool ReadFile(string&);
	bool ChangeName(string&);
	bool ChangeClass(unsigned int);
	bool ChangeLevel(unsigned int);
	void DoDecision(int&, string&);
	void ChoicePromptOutput(int&);
	bool SaveToFile();
	void SetCheckSum();
	void SavePrompt();
	void ChoicePrompts(int&);
};

#endif
