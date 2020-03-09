#include "D2SaveEditor.h"

D2SaveEditor::D2SaveEditor()
{
	characterBuffer = NULL;
	fileSize = 0;
	nameChanged = false;
	changesToSave = false;
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
	fileSize = 0;
	bufferSize = 0;
	ifstream* inputFile = new ifstream(filePath, ios::binary);
	if (!inputFile->is_open())
	{
		delete inputFile;
		inputFile = NULL;
		cout << "Error! File path could not be resolved." << endl;
		return false;
	}
	nameChanged = false;
	changesToSave = false;
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
	for (int i = HEADER_SIZE; i < fileSize; i++)
	{
		if (characterBuffer[i] == characterBuffer[LEVEL_INDEX])
		{
			characterBuffer[i] = newLevel;
			break;
		}
	}
	characterBuffer[LEVEL_INDEX] = newLevel;
	return true;
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
			break;
		case 5:
			cout << filePathPrompt << endl;
			break;
		case 6:
			cout << "What do you want your new level to be?" << endl;
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
	unsigned int newLevel = -1;
	switch (choice)
	{
		case 1:
			do
			{
				ChoicePrompts(choice);
				cin >> classChoice;
				cout << endl;
			} while (!ChangeClass(classChoice));
			break;
		case 2:
			do
			{
				ChoicePrompts(choice);
				cin >> newCharacterName;
				cout << endl;
			} while (!ChangeName(newCharacterName));
			break;
		case 3:
			ChoicePrompts(choice);
			break;
		case 4:
			SaveToFile();
			ChoicePrompts(choice);
			break;
		case 5:
			do
			{
				cout << filePathPrompt << endl;
				cin >> filePath;
			} while (!ReadFile(filePath));
			break;
		case 6:
			do
			{
				ChoicePrompts(choice);
				cin >> newLevel;
			} while (!ChangeLevel(newLevel));
			break;
		default:
			ChoicePrompts(choice);
			break;
	}
}

void D2SaveEditor::ChoicePromptOutput(int& choice)
{
	cout << "What would you like to do now?" << endl
		<< "1. Change Class" << endl
		<< "2. Change Name" << endl
		<< "3. Display Current Name" << endl
		<< "4. Save to New File" << endl
		<< "5. Read In a New File" << endl
		<< "6. Change Level -> Doesn't quite work." << endl
		<< "7. Quit" << endl;
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
		if (choice == 7)
		{
			if (changesToSave)
			{
				SavePrompt();
			}
			break;
		}
		DoDecision(choice, filePath);
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
	//AA
	int startingIndex = currentFilePath.find_last_of('\\') + 1;
	if (startingIndex == string::npos)
	{
		startingIndex = 0;
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
	FindStatsDisplay();
	return true;
}

// Strength, Energy, Dexterity, Vitality are stored as 13 bit values in the save file. Can be higher than that in game, but not in the save file.
// FF 5F -> 1111 1111 0101 1111 -> 1111 1111 ???1 1111 = 13 bits. Pretty sure the ? is part of a code identifying the stat. Least significant byte
// first. Pretty sure it's 8 bits for  the stat id itself.
// 5F 60 -> 0101 1111 0110 0000 -> ???1 1111 011? ???? -> the question marks are where the bits for the stat id are I think.
// Actually, based on strength, I think it is 9 bit for the stat id.
// 00 FF FE
// 11 Bytes for strength, energy, dexterity, and vitality.
// Strength = 0
// Energy = 1
// Dexterity = 10
// Vitality = 11
// 00        FE        7F        80        FF        2F        E0        FF        0F        F8        FF -> every stat at cap of 8191.
// 0000 0000 1111 1110 0111 1111 1000 0000 1111 1111 0010 1111 1110 0000 1111 1111 0000 1111 1111 1000 1111 1111
// ???? ???? 1111 111? ??11 1111 1??? ???? 1111 1111 ???? 1111 111? ???? 1111 1111 ???? ??11 1111 1??? 1111 1111
bool D2SaveEditor::FindStatsDisplay()
{
	int index = HEADER_SIZE;
	for (; characterBuffer[index] != 'g' || characterBuffer[index + 1] != 'f'; index++)
	{

	}
	index += 2;
	int maxIndex;
	for (maxIndex = index; characterBuffer[maxIndex] != 'i' || characterBuffer[maxIndex + 1] != 'f'; maxIndex++)
	{

	}
	long long section = 0;
	int shifted = 0;
	// Assigns 1111 1111 0000 0000 0010 1111 1111 1111 1000 0000 0111 1111 1111 1110 0000 0000 -> Reverse order of above order.
	section = *(long long*)(characterBuffer + index);
	
	for (int i = 0; i < 16 && index < maxIndex; i++)
	{
		if (shifted > 8)
		{
			index += shifted / 8;
			shifted %= 8;
			section = *(long long*)(characterBuffer + index);
			section >>= shifted;
		}
		string outputString = "";
		int shift = 0;
		int idShift = STAT_ID_SIZE;
		int characteristicMask = 0;
		//This should be a dictionary.
		switch (section & STAT_ID_MASK)
		{
			case 0:
				outputString = "Strength: ";
				break;
			case 1:
				outputString = "Energy: ";
				break;
			case 2:
				outputString = "Dexterity: ";
				break;
			case 3:
				outputString = "Vitality: ";
				break;
			case 4:
				outputString = "Stat Points: ";
				break;
			case 5:
				outputString = "Skill Points: ";
				break;
			case 6:
				outputString = "Current Health: ";
				break;
			case 7:
				outputString = "Base Maximum Health: ";
				break;
			case 8:
				outputString = "Current Mana: ";
				break;
			case 9:
				outputString = "Base Maximum Mana: ";
				break;
			case 10:
				outputString = "Current Stamina: ";
				break;
			case 11:
				outputString = "Base Maximum Stamina: ";
			case 12:
				outputString = "Level: ";
				break;
			case 13:
				outputString = "Experience: ";
				break;
			case 14:
				outputString = "Inventory Gold: ";
				break;
			case 15:
				outputString = "Stash Gold: ";
				break;
			default:
				break;
		}
		switch (section & STAT_ID_MASK)
		{
			case 1:
			case 2:
			case 3:
			case 0:
				characteristicMask = STAT_MASK;
				shift = STAT_SIZE;
				break;
				//Stat Points. Stored as 11 bits.
			case 4:
				characteristicMask = STAT_POINT_MASK;
				shift = STAT_POINT_SIZE;
				break;
			case 5:
				characteristicMask = SKILL_POINT_MASK;
				shift = SKILL_POINT_SIZE;
				break;
			case 11:
			case 10:
			case 9:
			case 8:
			case 7:
			case 6:
				//This is a fixed point number for some reason. It has 8 practically pointless bits, so they are removed.
				characteristicMask = CURRENT_HEALTH_MASK >> 8;
				shift = CURRENT_HEALTH_SIZE - 8;
				section >>= 8;
				shifted += 8;
				break;
			case 12:
				characteristicMask = LEVEL_MASK;
				shift = LEVEL_SIZE;
				break;
			case 13:
				characteristicMask = EXPERIENCE_MASK;
				shift = EXPERIENCE_SIZE;
				break;
			case 15:
			case 14:
				characteristicMask = GOLD_MASK;
				shift = GOLD_SIZE;
				break;
		default:
			break;
		}
		if (outputString != "")
		{
			section >>= idShift;
			shifted += idShift;
			cout << outputString << (section & characteristicMask) << "    Index: " << index << "     Shifted: " << shifted << endl;
			section >>= shift;
			shifted += shift;
		}
	}

	return true;
}
