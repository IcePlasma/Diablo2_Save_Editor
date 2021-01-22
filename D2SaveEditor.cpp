#include "D2SaveEditor.h"

D2SaveEditor::D2SaveEditor()
{
	characterBuffer = NULL;
	ResetDefaults();
}

D2SaveEditor::~D2SaveEditor()
{
	if (characterBuffer != NULL)
	{
		delete[] characterBuffer;
		characterBuffer = NULL;
	}
}

//Reads in the data for the specified file  at the given file path.
bool D2SaveEditor::ReadFile(string& filePath)
{
	if (filePath.length() < MINIMUM_FILE_LENGTH)
	{
		cout << "Error! File path is below minimum length." << endl;
		return false;
	}
	if (characterBuffer != NULL)
	{
		delete[] characterBuffer;
		characterBuffer = NULL;
	}
	ResetDefaults();
	ifstream* inputFile = new ifstream(filePath, ios::binary);
	if (!inputFile->is_open())
	{
		delete inputFile;
		inputFile = NULL;
		cout << "Error! File path could not be resolved." << endl;
		return false;
	}
	currentFilePath = filePath;

	inputFile->seekg(0, ios::end);
	fileSize = inputFile->tellg();
	inputFile->seekg(0, ios::beg);

	bufferSize = fileSize * 2;
	characterBuffer = new char[bufferSize];

	inputFile->read(characterBuffer, fileSize);
	inputFile->close();
	delete inputFile;

	inputFile = NULL;
	nameChanged = false;
	return true;
}

void D2SaveEditor::ResetDefaults()
{
	fileSize = 0;
	bufferSize = 0;
	nameChanged = false;
	changesToSave = false;
	statStartIndex = 0;
	statEndIndex = 0;
	currentFilePath = "";
}

void D2SaveEditor::RunEditor()
{
	string filePath = "";
	cout << filePathPrompt << endl;
	cin >> filePath;
	RunEditor(filePath);
}

bool D2SaveEditor::ChangeLevel(unsigned int newLevel)
{
	if (newLevel < 1 || newLevel > MAXIMUM_LEVEL)
	{
		cout << "Invalid level entered!" << endl;
		return false;
	}
	if (characterBuffer == NULL || fileSize < HEADER_SIZE)
	{
		return false;
	}
	changesToSave = true;
	characterBuffer[LEVEL_INDEX] = newLevel;

	ModifyStat(LEVEL_ID, newLevel);
	ModifyStat(EXPERIENCE_ID, experienceMinimum[newLevel - 1]);

	DisplayStatsOrSeek(true, newLevel, fileSize);
	return true;
}

void D2SaveEditor::MaxAllStats()
{
	for (int i = 0; i < NUMBER_OF_STATS; i++)
	{
		if (i == LEVEL_ID)
		{
			i++;
			continue;
		}
		ModifyStat(i, 0xFFFFFF);
	}
	ChangeLevel(MAXIMUM_LEVEL);
}

// Modifies a given stat to be newValue. The given new value is masked to be the correct bit length, so values higher than the maximum will be truncated.
void D2SaveEditor::ModifyStat(int statID, int newValue)
{
	unsigned long long memoryValue = 0;
	unsigned int statIndex = statID;
	int shiftAmount = 0;
	if (!DisplayStatsOrSeek(false, statIndex, shiftAmount))
	{
		cout << endl << statIdToString[statID] << " not found in character file. Try raising its value." << endl;
		return;
	}
	if (statID <= BASE_MAX_STAMINA_ID && statID >= CURRENT_HEALTH_ID)
	{
		shiftAmount -= 8;
	}
	else if (statID == STASH_GOLD_ID && newValue > MAX_STASH_GOLD)
	{
		newValue = MAX_STASH_GOLD;
	}
	else if (statID == INVENTORY_GOLD_ID && newValue > MAX_INVENTORY_GOLD)
	{
		newValue = MAX_INVENTORY_GOLD;
	}
	memoryValue = *(long long*)(characterBuffer + statIndex);
	memoryValue &= (~(statIDToMask[statID] << shiftAmount));
	memoryValue |= (((long long)newValue) & statIDToMask[statID]) << shiftAmount;
	*(long long*)(characterBuffer + statIndex) = memoryValue;
}

void D2SaveEditor::SavePrompt()
{
	char response = '\0';
	bool saveDecision = false;
	while (!saveDecision)
	{
		cout << "Would you like to save your changes before quitting? Enter y/n for yes or no." << endl;
		cin >> response;
		switch (response)
		{
			case 'y':
			case 'Y':
				SaveToFile();
			case 'n':
			case 'N':
				saveDecision = true;
				break;
			default:
				cout << "No valid answer picked!" << endl;
		}
	}
}

//Displays the text for each of the prompts options in DoDecision().
void D2SaveEditor::ChoicePrompts(int& choice)
{
	switch (choice)
	{
		case 1:
			cout << "Please pick a class: " << endl
				<< "0.    Amazon" << endl
				<< "1.    Sorceress" << endl
				<< "2.    Necromancer" << endl
				<< "3.    Paladin" << endl
				<< "4.    Barbarian" << endl
				<< "5.    Druid" << endl
				<< "6.    Assassin" << endl;
			break;
		case 2:
			cout << "Please enter a valid name: ";
			break;
		case 3:
			cout << (string)(characterBuffer + NAME_START) << endl;
			break;
		case 4:
		case 7:
			break;
		case 5:
			cout << filePathPrompt << endl;
			break;
		case 6:
			cout << "Caution! Only edit stats that show up when displaying the stats for this save. Which stat would you like to edit? " << endl
				<< STRENGTH_ID << ".    Strength" << endl
				<< ENERGY_ID << ".    Energy" << endl
				<< DEXTERITY_ID << ".    Dexterity" << endl
				<< VITALITY_ID << ".    Vitality" << endl
				<< STAT_POINTS_ID << ".    Stat Points" << endl
				<< SKILL_POINTS_ID << ".    Skill Points" << endl
				<< CURRENT_HEALTH_ID << ".    Current Health" << endl
				<< BASE_MAX_HEALTH_ID << ".    Max Health" << endl
				<< CURRENT_MANA_ID << ".    Current Mana" << endl
				<< BASE_MAX_MANA_ID << ".    Max Mana" << endl
				<< CURRENT_STAMINA_ID << ".   Current Stamina" << endl
				<< BASE_MAX_STAMINA_ID << ".   Max Stamina" << endl
				<< LEVEL_ID << ".   Level" << endl
				<< EXPERIENCE_ID << ".   Experience Points" << endl
				<< INVENTORY_GOLD_ID << ".   Gold in Inventory" << endl
				<< STASH_GOLD_ID << ".   Gold in Stash" << endl;
			break;
		default:
			cout << "Invalid choice entered." << endl;
			break;
	}
}

//Performs the chosen action of the user that they picked from the options in ChoicePromptOutput();
void D2SaveEditor::DoDecision(int& choice, string& filePath)
{
	string newCharacterName = "";
	unsigned int classChoice = -1;
	unsigned int statChoice = -1;
	switch (choice)
	{
		case CLASS_CHANGE_PROMPT:
			do
			{
				ChoicePrompts(choice);
				cin >> classChoice;
				cout << endl;
			} while (!ChangeClass(classChoice));
			break;
		case CHANGE_NAME_PROMPT:
			do
			{
				ChoicePrompts(choice);
				cin >> newCharacterName;
				cout << endl;
			} while (!ChangeName(newCharacterName));
			break;
		case DISPLAY_NAME_PROMPT:
			ChoicePrompts(choice);
			break;
		case SAVE_TO_NEW_FILE_PROMPT:
			SaveToFile();
			ChoicePrompts(choice);
			break;
		case READ_NEW_FILE_PROMPT:
			do
			{
				cout << filePathPrompt << endl;
				cin >> filePath;
			} while (!ReadFile(filePath));
			break;
		case EDIT_STAT_PROMPT:
			do
			{
				ChoicePrompts(choice);
				cin >> statChoice;
			} while (!ChangeStat(statChoice));
			break;
		case DISP_STAT_PROMPT:
			DisplayStatsOrSeek(true, classChoice, choice);
			break;
		case MAX_STATS_PROMPT:
			MaxAllStats();
			break;
		case MAX_SKILLS_PROMPT:
			MaxAllSkills();
			break;
		default:
			ChoicePrompts(choice);
			break;
	}
}

bool D2SaveEditor::ChangeStat(int statChoice)
{
	if (statChoice > NUMBER_OF_STATS || statChoice < 0)
	{
		cout << statChoice << " is an invalid choice. Please select a stat from the provided choices" << endl;
		return false;
	}

	int newValue = 0;
	while (newValue < 1)
	{
		cout << endl << "Please enter a new positive value for " << statIdToString[statChoice];
		cin >> newValue;
	}
	
	if (statChoice == LEVEL_ID)
	{
		ChangeLevel(newValue);
	}
	else
	{
		ModifyStat(statChoice, newValue);
	}

	return true;
}

void D2SaveEditor::ChoicePromptOutput(int& choice)
{
	cout << "What would you like to do now?" << endl
		<< CLASS_CHANGE_PROMPT << ". Change Class" << endl
		<< CHANGE_NAME_PROMPT << ". Change Name" << endl
		<< DISPLAY_NAME_PROMPT << ". Display Current Name" << endl
		<< SAVE_TO_NEW_FILE_PROMPT << ". Save to New File" << endl
		<< READ_NEW_FILE_PROMPT << ". Read In a New File" << endl
		<< EDIT_STAT_PROMPT<< ". Edit Stats (including level, life, gold, etc)" << endl
		<< DISP_STAT_PROMPT << ". Display All Stats" << endl
		<< MAX_STATS_PROMPT << ". Maximize All Stats (including level, life, gold, etc)" << endl
		<< MAX_SKILLS_PROMPT << ". Maximize All Skills" << endl
		<< QUIT_PROMPT << ". Quit" << endl;
	cin >> choice;
}

//Performs a loop of prompts for the user to determine what they want to do.
void D2SaveEditor::RunEditor(string& filePath)
{
	while (!ReadFile(filePath))
	{
		cout << filePathPrompt << endl;
		cin >> filePath;
	}
	cout << "File successfully read in." << endl;
	while (true)
	{
		int choice = 0;
		ChoicePromptOutput(choice);
		if (choice == QUIT_PROMPT)
		{
			if (changesToSave)
			{
				SavePrompt();
			}
			break;
		}
		DoDecision(choice, filePath);
		cout << endl;
	}
}

bool D2SaveEditor::ChangeName(string& newName)
{
	if (newName.length() > NAME_SIZE || newName.length() <= 1)
	{
		cout << "Invalid name entered!" << endl;
		return false;
	}
	if (characterBuffer == NULL || fileSize < HEADER_SIZE)
	{
		return false;
	}
	unsigned int nameIndex = 0;
	if ((string)(characterBuffer + NAME_START) == newName && !nameChanged)
	{
		cout << "Error! Name entered is the same as before!" << endl;
		return false;
	}
	for (; nameIndex < newName.length(); nameIndex++)
	{
		characterBuffer[NAME_START + nameIndex] = newName[nameIndex];
	}
	for (; nameIndex <= NAME_SIZE; nameIndex++)
	{
		characterBuffer[NAME_START + nameIndex] = 0;
	}
	changesToSave = true;
	nameChanged = true;
	return true;
}

bool D2SaveEditor::ChangeClass(unsigned int newClass)
{
	if (characterBuffer == NULL)
	{
		cout << "Error! Character not loaded in yet.";
		return false;
	}
	if (newClass > ASSASSIN || fileSize < HEADER_SIZE)
	{
		cout << "Error! Value for new class is not valid.";
		return false;
	}
	characterBuffer[CLASS_INDEX] = newClass;
	changesToSave = true;
	return true;
}

//Calculates the value of the checksum and sets the value in the characterBuffer.
void D2SaveEditor::SetCheckSum()
{
	int checksum = 0;
	*(DWORD*)(characterBuffer + CHECKSUM_INDEX) = 0;
	for (int iterate = 0; iterate < fileSize; iterate++)
	{
		checksum = (checksum * 2) + (((unsigned int)characterBuffer[iterate]) & 0xFF) + (checksum < 0);
	}
	*(DWORD*)(characterBuffer + CHECKSUM_INDEX) = checksum;
}

//#define SKIP_ERROR_CHECK_SAVE
//Save the changes to the file in the directory specified by currentFilePath.
bool D2SaveEditor::SaveToFile()
{
#ifndef SKIP_ERROR_CHECK_SAVE
	if (!changesToSave)
	{
		return true;
	}
	if (characterBuffer == NULL || fileSize < HEADER_SIZE)
	{
		return false;
	}
	//The file name has to match the character name, and I don't want to save over the previous file.
	//Therefore, the name must be changed in order to save the changes.
	if (!nameChanged)
	{
		cout << "You have chosen to save to a file without first changing the name." << endl;
		string unsetName = "";
		do
		{
			cout << "Enter a valid new name now: ";
			cin >> unsetName;
			cout << endl;
		} while (!ChangeName(unsetName));
	}
#endif // !SKIP_ERROR_CHECK_SAVE
	int startingIndex = currentFilePath.find_last_of('\\');
	if (startingIndex == string::npos)
	{
		startingIndex = 0;
	}
	else
	{
		startingIndex++;
	}
	currentFilePath.replace(startingIndex, startingIndex + NAME_SIZE + EXTENSION_LENGTH, (string)(characterBuffer + NAME_START) + FILE_EXTENSION);
	SetCheckSum();

	ofstream* outputFile = new ofstream(currentFilePath, ios::binary | ios::trunc | ios::out);
	if (!outputFile->is_open())
	{
		delete outputFile;
		outputFile = NULL;
		return false;
	}
	outputFile->write(characterBuffer, fileSize);
	outputFile->close();
	delete outputFile;

	outputFile = NULL;
	changesToSave = false;
	return true;
}

// Strength, Energy, Dexterity, Vitality are stored as 13 bit values in the save file. Can be higher than that in game, but not in the save file.
// FF 5F -> 1111 1111 0101 1111 -> 1111 1111 ???1 1111 = 13 bits. Pretty sure the ? is part of a code identifying the stat. Least significant byte
// first. Pretty sure it's 8 bits for  the stat id itself.
// 5F 60 -> 0101 1111 0110 0000 -> ???1 1111 011? ???? -> the question marks are where the bits for the stat id are I think.
// Actually, based on strength, I think it is 9 bit for the stat id. It is 9 bit.
// 00 FF FE
// 11 Bytes for strength, energy, dexterity, and vitality all together + their id.
// Strength = 0
// Energy = 1
// Dexterity = 10
// Vitality = 11
// 00        FE        7F        80        FF        2F        E0        FF        0F        F8        FF		-> every stat at cap of 8191.
// 0000 0000 1111 1110 0111 1111 1000 0000 1111 1111 0010 1111 1110 0000 1111 1111 0000 1111 1111 1000 1111 1111
// ???? ???? 1111 111? ??11 1111 1??? ???? 1111 1111 ???? 1111 111? ???? 1111 1111 ???? ??11 1111 1??? 1111 1111

//Finds the starting and ending index of the stats.
bool D2SaveEditor::FindStats()
{
	if (characterBuffer == NULL)
	{
		return false;
	}
	statStartIndex = HEADER_SIZE;
	for (; characterBuffer[statStartIndex] != 'g' || characterBuffer[statStartIndex + 1] != 'f'; statStartIndex++)
	{

	}
	statStartIndex += 2;
	for (statEndIndex = statStartIndex; characterBuffer[statEndIndex] != 'i' || characterBuffer[statEndIndex + 1] != 'f'; statEndIndex++)
	{

	}
	return true;
}

void D2SaveEditor::MaxAllSkills()
{
	if (statStartIndex < HEADER_SIZE)
	{
		if (!FindStats())
		{
			return;
		}
	}
	changesToSave = true;
	int skillStartIndex = statEndIndex + 2;
	int skillEnd;
	switch (characterBuffer[CLASS_INDEX])
	{
		case NECROMANCER:
		case BARBARIAN:
		case DRUID:
			skillEnd = 35;
			break;
		default:
			skillEnd = 36;
			break;
	}
	//necro, barb, druid
	for (int i = 0; i < skillEnd; i++)
	{
		characterBuffer[skillStartIndex + i] = 0x96;
	}
}

bool D2SaveEditor::DisplayStatsOrSeek(bool displayStats, unsigned int& statToSeek, int& bitShiftAtIndex)
{
	if (statStartIndex < HEADER_SIZE)
	{
		if (!FindStats())
		{
			return false;
		}
	}
	int index = statStartIndex;
	long long section = 0;
	int shifted = 0;
	section = *(long long*)(characterBuffer + statStartIndex);
	
	for (int i = 0; i < NUMBER_OF_STATS && index < statEndIndex; i++)
	{
		if (displayStats)
		{
			cout << endl;
		}
		if (shifted > 8)
		{
			index += shifted / 8;
			shifted %= 8;
			section = *(long long*)(characterBuffer + index);
			section >>= shifted;
		}

		int shift = 0;
		int characteristicMask = 0;
		unsigned int statID = section & STAT_ID_MASK;
		if (statID <= NUMBER_OF_STATS)
		{
			switch (statID)
			{
				case ENERGY_ID:
				case DEXTERITY_ID:
				case VITALITY_ID:
				case STRENGTH_ID:
					//Strength, Energy, Dexterity, Vitality. Stored as 13 bits each.
					characteristicMask = STAT_MASK;
					shift = STAT_SIZE;
					break;
					//Stat Points. Stored as 11 bits.
				case STAT_POINTS_ID:
					characteristicMask = STAT_POINT_MASK;
					shift = STAT_POINT_SIZE;
					break;
				case SKILL_POINTS_ID:
					characteristicMask = SKILL_POINT_MASK;
					shift = SKILL_POINT_SIZE;
					break;
				case BASE_MAX_STAMINA_ID:
				case CURRENT_STAMINA_ID:
				case BASE_MAX_MANA_ID:
				case CURRENT_MANA_ID:
				case BASE_MAX_HEALTH_ID:
				case CURRENT_HEALTH_ID:
					//This is a fixed point number for some reason. It has 8 practically pointless bits, so they are removed.
					characteristicMask = CURRENT_HEALTH_MASK >> 8;
					shift = CURRENT_HEALTH_SIZE - 8;
					section >>= 8;
					shifted += 8;
					break;
				case LEVEL_ID:
					characteristicMask = LEVEL_MASK;
					shift = LEVEL_SIZE;
					break;
				case EXPERIENCE_ID:
					characteristicMask = EXPERIENCE_MASK;
					shift = EXPERIENCE_SIZE;
					break;
				case STASH_GOLD_ID:
				case INVENTORY_GOLD_ID:
					characteristicMask = GOLD_MASK;
					shift = GOLD_SIZE;
					break;
				default:
					break;
			}
			section >>= STAT_ID_SIZE;
			shifted += STAT_ID_SIZE;
			if (displayStats)
			{
				cout << (statIdToString[statID]) << (section & characteristicMask) << endl;
			}
			else if(statToSeek == statID)
			{
				statToSeek = index;
				bitShiftAtIndex = shifted;
				return true;
			}
			section >>= shift;
			shifted += shift;
		}
	}
	return displayStats;
}
