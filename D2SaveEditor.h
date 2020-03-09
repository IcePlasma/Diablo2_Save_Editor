#ifndef D2_SAVE_EDITOR
#define D2_SAVE_EDITOR
#define NAME_SIZE 15
#define NAME_START 20
#define HEADER_SIZE 765
#define STAT_ID_SIZE 9
#define STAT_ID_MASK 511
#define STAT_SIZE 13
#define STAT_POINT_SIZE 11
#define SKILL_POINT_SIZE 9
#define CURRENT_HEALTH_SIZE 21
#define LEVEL_SIZE 8
#define EXPERIENCE_SIZE 32
#define GOLD_SIZE 25
#define STAT_MASK 8191
#define STAT_POINT_MASK 2047
#define SKILL_POINT_MASK 511
#define CURRENT_HEALTH_MASK 0x1FFFFF
#define LEVEL_MASK 0xFF
#define EXPERIENCE_MASK 0xFFFFFFFF
#define GOLD_MASK 0x1FFFFFF
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
	const unsigned int experienceMinimum[150] = 
	{
		0, 500, 1500, 3500, 6500, 11500, 16000, 22000, 29000, 37000,
		46000, 56000, 67000, 79000, 92000, 106000, 121000, 137000,
		154000, 172000, 191000, 211000, 232000, 254000, 277000,
		301000, 326000, 352000, 379000, 407000, 436000, 466000,
		497000, 529000, 562000, 596000, 631000, 667000, 704000,
		742000, 781000, 821000, 862000, 904000, 947000, 991000,
		1036000, 1082000, 1129000, 1177000, 1226000, 1276000,
		1327000, 1379000, 1432000, 1486000, 1541000, 1597000,
		1654000, 1712000, 1771000, 1831000, 1892000, 1954000,
		2017000, 2081000, 2146000, 2212000, 2279000, 2347000,
		2416000, 2486000, 2557000, 2629000, 2702000, 2776000,
		2851000, 2927000, 3004000, 3082000, 3161000, 3241000,
		3322000, 3404000, 3487000, 3571000, 3656000, 3742000,
		3829000, 3917000, 4006000, 4096000, 4187000, 4279000,
		4372000, 4466000, 4561000, 4657000, 4754000, 4852000,
		4951001, 5051003, 5152007, 5254013, 5357021, 5461032,
		5566047, 5672067, 5779093, 5887126, 5996168, 6106221,
		6106221, 6329375, 6442483, 6556619, 6671790, 6788004,
		6905273, 7023609, 7143029, 7263555, 7385213, 7508034,
		7632061, 7757344, 7883947, 8011948, 8141447, 8272568,
		8405465, 8540331, 8677406, 8816991, 8959461, 9105282,
		9255041, 9409466, 9569467, 9736181, 9911026, 10095774,
		10292636, 10504370, 10734423, 10987094, 11267753, 
		11267753, 11941504, 12353407
	};
	bool nameChanged;
	bool changesToSave;
	bool FindStatsDisplay();
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
