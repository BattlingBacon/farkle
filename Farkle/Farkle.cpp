//TODO:
//implement progress bar

/*********************************************************************************************************************************************************************************************************
THIS PROGRAM MAKES SOME ASSUMPTIONS ABOUT THE GAME FARKLE:
1. If you have another choice to not Farkle, taking three 2's (200 points) is a bad idea (so unless they are the only 3 remaining dice, don't take them if you can)
2. If you have multiple 5's and 1's, and no more than 2 5's and 3 1's, take a single 1 (100 points) over taking a 1 and a 5 or any othre combination out of the roll (unless those are the remaining dice)

NOTES:
1. Each 0 in row index's 1-6 represents a dice that cannot be used because it was taken out in a previous role.
2. The first index on each row of the array determines if the previous set of dice rolls was finished (farkled) or contninued in this turn. 0 = continued, 1 = new set but same turn, 2 = new turn (farkled).
*********************************************************************************************************************************************************************************************************/
#include <string> 
#include <iostream>
#include <time.h>
#include <iomanip>
#include <stdlib.h>
#include <fstream>

using namespace std;

bool inputVal(char, char, string);
void populateDataFile();
void rollDice(int[], int);
int whenToStop(int);
int farkleOrPoints(int[]); //tests for a farkle or if points can be gained and takes out the die that can be taken out
void removeDice(int[], int); //replaces used dice with 0's so that they don't get rerolled
void progressBar(int, int); //this will later be used to try and predict how long it will take before a process is finished.

bool unlimited = false; //unlimited is mainly meant to see the highest score the computer will get over millions of farkles. not unlimited is meant for testing the algorithms to make sure everything is working as intended by having the computer display step by step when it reaches a goal.
int continues; //when the player rolls the dice, doesn't farkle, and keeps rolling their set
int allUsed; //when all 6 dice are used without farkling
int points;
int diceLeft;
int farkles;
int diceRolled; //how many individual dice have been rolled

//keeping track of the amount of times certian dicerolls happen that earn points
int fives = 0;
int ones = 0;
int twoTriples = 0;
int triples = 0;
int quadruples = 0;
int pentuples = 0;
int sextuples = 0;
int straights = 0;
int threePairs = 0;
int quadrupleWithPair = 0;

int highestScore = 0;
int farklesM = 0; //farkles in millions

bool percentageBarEnabled = false;
int forceStopRolls;
int forceStopRollsOrigonal;
bool graphing = false;

string outputString; //stores data for current turn in case it meets the goal so it can be printed

int main()
{
	int graphNum = 150;
	int testingCode = 1;
	while (testingCode != 0)
	{
		cout << "Testing code (type -1 for help): ";
		cin >> testingCode;

		if (testingCode == -1)
		{
			cout << "Type in a number to try a work in progress feature.\n\n0 = continue normally\n1 = Test to see what are the chances of farkling and what are the gains with x amount of dice\n";
		}
		else if (testingCode == 1)
		{
			int x = 0;
			cout << "How many Farkles with each quantity of dice left (Minimum: 100 | Recomendation: More than 10000): ";
			cin >> x;
			if (x != 0)
				cout << "\nYou should stop on average with " << whenToStop(x) << " dice left.\n";
		}
		else if (testingCode == 2)
		{
			cout << "Generating all possible outcomes in data.txt...\n";
			populateDataFile();
			cout << "Done!\n\n";
		}
		else if (testingCode == 3)
		{
			cout << "Choose to farkle after x rolls: ";
			cin >> forceStopRolls;
			forceStopRolls++;
			forceStopRollsOrigonal = forceStopRolls;
		}
		else if (testingCode == 4)
		{
			graphing = true;
			cout << "Distribution graphNum mode enabled. The game will go to " << graphNum << " farkles.";
		}
		else if (testingCode != 0)
		{
			cout << "That is not a valid input. Please type \"-1\" for help.";
		}
		cout << endl;
	}

	//initializing all the variables to be used to keep track of the results of the tests and other purposes
	int metGoal = 0;
	int times = 0;
	int setsRolled = 0;
	int rolledM = 0; //dice rolled in millions
	int highestScore = 0;
	int farklesCurrent = 0;
	string setSpace = "";

	//**************************** SETUP THE POINT GOALS ****************************
	int goal = 0;
	int baseGoal = 0;

	if (!graphing)
	{
		cout << "Enter 0 for Unlimited. I reccomend making these numbers greater than 10000 to prevent output spam\n\n" << "Goal: ";
		cin >> goal;
		if (goal != 0)
		{
			cout << "How many times to meet the goal: ";
			cin >> times;
		}
		else
		{
			unlimited = true;
			cout << "Show me all the roles before the farkle if the score was over: ";
			cin >> baseGoal;
		}
	}


	bool goalWaiting = false;
	int graphOverride = 0;
	srand(time(NULL));

	if (!graphing)
	{
		if (times == 0)
			unlimited = true;
		else
		{
			unlimited = false;
		}
	}
	else
	{
		graphOverride = graphNum;
	}

	int diceSet[6] = { 0 };

	cout << "Rolling the dice...\n\n";

	if (unlimited) // set up the format for output
	{
		if (goal != 0)
		{
			cout << " Farkles (millions) |   Highest   |  Goal Met" << endl;
		}
		else
		{
			cout << " Farkles (millions) |   Highest   |  Rolls (millions)" << endl;
		}
	}

	ofstream graphFile;
	if (graphing)
		graphFile.open("graphNum.txt");

	int code = 1; //code is what happened in the previous role. 0 = continued, 1 = new set but same turn, 2 = new turn (farkled)
	while (metGoal < times || unlimited || graphOverride > 0)
	{
		if (graphing)
			graphOverride--;

		if (forceStopRollsOrigonal != 0)
		{
			if (forceStopRolls > 0)
			{
				rollDice(diceSet, code);
				forceStopRolls--;
			}
			else
			{
				forceStopRolls = forceStopRollsOrigonal;
				code = 2;
			}
		}
		else
		{
			rollDice(diceSet, code);
		}

		if (forceStopRollsOrigonal != 0)
		{
			if (forceStopRolls != 0)
			{
				code = farkleOrPoints(diceSet); //determine the outcome of the dice's results
			}
			else
			{
				code = 2;
			}
		}
		else
		{
			code = farkleOrPoints(diceSet); //determine the outcome of the dice's results
		}


		setsRolled++;

		if (points >= goal && goalWaiting == false)
		{
			metGoal++;
			goalWaiting = true;
		}
		else if (goalWaiting)
		{
			if (code == 2) //if we farkle again
			{
				goalWaiting = false; //allow the metGaol to increase again
			}
		}

		if (points > highestScore)
			highestScore = points;

		if (code == 2) // farkled
		{
			if (graphing)
			{
				graphFile << points << endl;
			}

			points = 0;
			if (unlimited)
			{
				if (goal == 0 && highestScore >= baseGoal && baseGoal != 0) //determine output format
				{
					baseGoal = highestScore + 50;
					if (forceStopRollsOrigonal != 0)
					{
						if (forceStopRolls == 0)
						{
							outputString += "Force-Farkled!\n";
						}
						else
						{
							outputString += "Farkled!\n";
						}
					}
					else
					{
						outputString += "Farkled!\n";
					}
					cout << endl << outputString << endl;
					outputString = "";
					cout << " Farkles (millions) |   Highest   |  Rolls (millions)" << endl;
				}
				else if (highestScore >= goal && goal != 0)
				{
					goal = highestScore + 50;
					outputString += "Farkled!\n";
					cout << endl << outputString << endl;
					outputString = "";

					cout << " Farkles (millions) |   Highest   |  Goal Met" << endl;
				}
			}
			outputString = "";
		}

		if (code == 1)
		{
			outputString += to_string(points) + ", New Dice Set\n";
		}
		else
		{
			outputString += to_string(points) + '\n';
		}

		if (unlimited && farkles >= 1000000)
		{
			farkles -= 1000000;
			farklesM++;
			farkles = 0;
		}

		if (unlimited && diceRolled >= 1000000)
		{
			diceRolled -= 1000000;
			rolledM++;
			diceRolled = 0;
		}

		if (farklesM < 0)
			unlimited = false;

		if (farklesM > farklesCurrent) // if we've gotten another million farkles
		{
			farklesCurrent = farklesM;
			if (goal != 0)
			{
				cout << setw(12) << farklesM << "        |" << setw(9) << highestScore << "    |" << setw(8) << metGoal << endl;
			}
			else
			{
				cout << setw(12) << farklesM << "        |" << setw(9) << highestScore << "    |" << setw(12) << rolledM << endl;
			}
		}
	}

	cout << endl << "Finished all " << diceRolled << " rolls. Analyzing results.\n";

	cout << "Highest Score: " << highestScore << endl;
	cout << "The goal was met: " << metGoal << " times." << endl;
	cout << "Average farkles before goal: " << farkles / metGoal << endl;
	cout << "Continues: " << continues << endl;
	cout << "Farkles: " << farkles << endl;
	cout << "Average Rolls Before Farkling: " << diceRolled / farkles << endl;
	cout << "All Dice Used: " << allUsed << endl;
	cout << "Fives: " << fives << endl;
	cout << "Ones: " << ones << endl;
	cout << "Triples: " << triples << endl;
	cout << "Quadruples: " << quadruples << endl;
	cout << "Pentuples: " << pentuples << endl;
	cout << "Sextuples: " << sextuples << endl;
	cout << "Straights: " << straights << endl;
	cout << "Three Pairs: " << threePairs << endl;
	cout << "Quadruples With Pair: " << quadrupleWithPair << endl;
	cout << "Two Triples: " << twoTriples << endl;
	cout << "Top 5 highestScore scores: " << endl;
	graphFile.close();

	if (graphing)
	{
		cout << "\nGraphing...\n\n";
		int current = 0;
		ifstream graphFileIn;
		graphFileIn.open("graphNum.txt");
		int arr[6] = { 0 };
		for (int i = 0; i < 150; i++)
		{
			graphFileIn >> current;
			current /= 1000;
			if (current > 6)
				current = 6;
			arr[current]++;
		}
		
		for (int i = 0; i < 5; i++)
		{
			cout << setw(4) << i * 1000  << " through " << setw(4) << i * 1000 + 1000 << ": ";
			for (int j = 0; j < arr[i]; j++)
				cout << "*";
			cout << endl;
		}
		cout << "5000 and higher  : ";
		for (int j = 0; j < arr[6]; j++)
			cout << "*";

		cout << "\n\n* = 1\n";
		graphFileIn.close();
	}

	

	return 0;
}

void rollDice(int diceSet[], int code)
{
	diceLeft = 6;
	for (int i = 0; i < 6; i++)
	{
		if (code > 0)
		{
			diceSet[i] = rand() % 6 + 1;
			diceRolled++;
		}
		else if (code == 0)
		{
			if (diceSet[i] != 0) //if the dice wasn't taken aside for points, role a new number
			{
				diceSet[i] = rand() % 6 + 1;
				diceRolled++;
			}
			else
			{
				diceLeft--;
			}
		}
	}
}

void populateDataFile() //creates a file with every dice roll combination (to be analyzed in future code).
{
	ofstream dataFile;
	dataFile.open("data.txt");

	string finalNum = "";
	int baseSeven = 0;
	int addZeroes = 0;
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			for (int k = 0; k < 7; k++)
			{
				for (int l = 0; l < 7; l++)
				{
					for (int m = 0; m < 7; m++)
					{
						for (int n = 0; n < 7; n++)
						{
							finalNum = to_string(baseSeven);
							addZeroes = 6 - finalNum.length();
							for (int h = 1; h < finalNum.length(); h += 2)
								finalNum.insert(h, " ");

							dataFile << finalNum;
							for (; addZeroes > 0; addZeroes--)
								dataFile << " 0";

							dataFile << endl;
							baseSeven += 1;
						}
						baseSeven = baseSeven / 10 * 10 + 10;
					}
					baseSeven = baseSeven / 100 * 100 + 100;
				}
				baseSeven = baseSeven / 1000 * 1000 + 1000;
			}
			baseSeven = baseSeven / 10000 * 10000 + 10000;
		}
		baseSeven = baseSeven / 100000 * 100000 + 100000;
	}
	dataFile.close();
}


int farkleOrPoints(int diceSet[])
{
	int farkleTest = 0; //if int farkle stays at 0, this roll was a farkle
	int outcomes[6] = { 0 };
	bool notFinished = true; //if false, we will still check for individual 1's and 5's
	bool isNotSextuple = true; //if false, there was a sextuple so we are done
	bool fivesOnes = true; //are there only 5s and 1s?
	int newPoints = 0;

	if (unlimited)
	{
		for (int i = 0; i < 6; i++)
		{
			outputString += to_string(diceSet[i]) + ", ";
		}
	}



	for (int i = 0; i < 6; i++) //outcomes is how many of which numbers are present on the dice
	{
		if (diceSet[i] > 0)
		{
			outcomes[diceSet[i] - 1]++;
		}
	}

	//the following all just figures out what points can be gained
	int sextuplesTest = 0;
	for (int i = 0; i < 6; i++)
	{
		if (outcomes[i] == 6)
		{
			sextuplesTest++;
			newPoints = 3000;
			notFinished = false;
			isNotSextuple = false;
			farkleTest++;
			fivesOnes = false;
			removeDice(diceSet, 0);
		}
	}

	int straightsTest = 0;
	for (int i = 0; i < 6; i++)
	{
		if (outcomes[i] == 1)
			straightsTest++;
	}
	if (straightsTest == 6)
	{
		straightsTest = 1;
		newPoints = 1500;
		notFinished = false;
		isNotSextuple = false;
		farkleTest++;
		fivesOnes = false;
		removeDice(diceSet, 0);
	}
	else
	{
		straightsTest = 0;
	}

	int twoTripletsTest = 0;
	for (int i = 0; i < 6; i++)
	{
		if (outcomes[i] == 3)
			twoTripletsTest++;
	}
	if (twoTripletsTest == 2)
	{
		fivesOnes = false;
		twoTripletsTest = 1;
		newPoints = 2500;
		notFinished = false;
		isNotSextuple = false;
		farkleTest++;
		removeDice(diceSet, 0);
	}
	else
	{
		twoTripletsTest = 0;
	}

	int threePairsTest = 0;
	for (int i = 0; i < 6; i++)
	{
		if (outcomes[i] == 2)
			threePairsTest++;

		if (threePairsTest == 3)
		{
			threePairsTest = 1;
			fivesOnes = false;
			newPoints = 1500;
			notFinished = false;
			isNotSextuple = false;
			removeDice(diceSet, 0);
		}
	}

	int pentuplesTest = 0;
	for (int i = 0; i < 6; i++)
	{
		if (outcomes[i] == 5 && notFinished)
		{
			fivesOnes = false;
			pentuplesTest++;
			newPoints = 2000;
			notFinished = false;

			removeDice(diceSet, i + 1);
		}
	}

	int quadruplesTest = 0;
	int quadruplesWithPairTest = 0;

	for (int i = 0; i < 6; i++)
	{
		if (outcomes[i] == 4 && notFinished)
		{
			quadruplesTest++;
			fivesOnes = false;
			notFinished = false;
			farkleTest++;
			newPoints = 1000;
			for (int j = 0; j < 6; j++)
			{
				if (outcomes[j] == 2)
				{
					quadruplesWithPairTest++;
					fivesOnes = false;
					quadruplesTest--;
					newPoints = 1500;
					isNotSextuple = false;
					removeDice(diceSet, 0);
				}
			}
			removeDice(diceSet, i + 1);
		}
	}

	int tripplesTest = 0;
	for (int i = 0; i < 6; i++)
	{
		if (outcomes[i] == 3 && notFinished)
		{
			if (i + 1 != 2 && i + 1 != 1)
			{
				tripplesTest++;
				fivesOnes = false;
				farkleTest++;
				removeDice(diceSet, i + 1);

				newPoints = (i + 1) * 100;
			}
		}
	}

	int fivesTest = 0;
	int onesTest = 0;
	int total = 0;
	for (int i = 0; i < 6; i++)
	{
		total += outcomes[i];
	}

	if (fivesOnes && total <= 3)
	{
		if (outcomes[0] == total)
		{
			newPoints += 100 * total;
			onesTest += total;
			farkleTest++;
			removeDice(diceSet, 0);
		}
		else if (outcomes[4] == total)
		{
			newPoints += 50 * total;
			fivesTest += total;
			farkleTest++;
			removeDice(diceSet, 0);
		}
	}

	if (fivesOnes)
	{
		if (outcomes[0] > 0)
		{
			newPoints += 100;
			onesTest++;
			farkleTest++;
			removeDice(diceSet, -1);
		}
		else if (outcomes[4] > 0)
		{
			newPoints += 50;
			fivesTest++;
			farkleTest++;
			removeDice(diceSet, -5);
		}
	}
	if (farkleTest == 0)
	{
		for (int i = 0; i < 6; i++)
		{
			if (outcomes[i] == 3 && notFinished)
			{
				{
					newPoints = 200;
					tripplesTest++;
					fivesOnes = false;
					farkleTest++;
					removeDice(diceSet, i + 1);
				}
			}
		}
	}

	//update tickers
	fives += fivesTest;
	ones += onesTest;
	triples += tripplesTest;
	quadruples += quadruplesTest;
	pentuples += pentuplesTest;
	sextuples += sextuplesTest;
	straights += straightsTest;
	threePairs += threePairsTest;
	quadrupleWithPair += quadruplesWithPairTest;
	twoTriples += twoTripletsTest;
	points += newPoints;


	if (diceSet[0] == 0 && diceSet[1] == 0 && diceSet[2] == 0 && diceSet[3] == 0 && diceSet[4] == 0 && diceSet[5] == 0)
	{
		allUsed++;
		return 1;
	}

	if (farkleTest == 0)
	{
		farkles++;
		return 2;
	}

	continues++;
	return 0;
}


void removeDice(int diceSet[], int num)
{


	if (num == 0) //remove every dice
	{
		for (int i = 0; i < 6; i++)
		{
			diceSet[i] = 0;
		}
	}

	if (num > 0 && num < 7) //remove selected dice
	{
		for (int i = 0; i < 6; i++)
		{
			if (diceSet[i] == num)
				diceSet[i] = 0;
		}
	}

	bool done = false;
	if (num == -1) //remove a dice with a 1
	{
		for (int i = 0; i < 6; i++)
		{
			if (diceSet[i] == 1 && done == false)
			{
				diceSet[i] = 0;
				done = true;
			}
		}
	}

	if (num == -5) //remove a dice with a 5
	{
		for (int i = 0; i < 6; i++)
		{
			if (diceSet[i] == 5 && done == false)
			{
				diceSet[i] = 0;
				done = true;
			}
		}
	}
}

/***********************************
//testing code: 1
//purpose: tested over a number of farkles, setsRolled, calculate after how many sets rolled you should stop rolling after on average
//stores data in the file "temp.txt"
***********************************/
int whenToStop(int setsRolled)
{
	if (setsRolled < 100)
		setsRolled = 100;

	ofstream tempFile;
	tempFile.open("temp.txt");

	int rolledSets = 0;
	int diceSet[6] = { 0 };
	int pointsBeforeFarkle = 0;

	for (int i = 1; i <= 6; i++)
	{
		while (rolledSets < setsRolled)
		{
			for (int j = 0; j < i; j++)
			{
				diceSet[j] = 1;
			}
			for (int j = i; j < 6; j++)
			{
				diceSet[j] = 0;
			}

			rollDice(diceSet, 0);

			farkleOrPoints(diceSet);

			tempFile << points << endl;
			points = 0;
			rolledSets++;
		}
		rolledSets = 0;
	}

	tempFile.close();

	ifstream tempFileIn;
	tempFileIn.open("temp.txt");

	int pointsAverage[6] = { 0 };
	int newPoints = 0;
	float chanceOfFarkle[6] = { 0 };

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < setsRolled; j++)
		{
			tempFileIn >> newPoints;
			pointsAverage[i] += newPoints;

			if (newPoints == 0)
				chanceOfFarkle[i]++;
		}
	}

	int bestQuantityDiceLeft = 0;

	cout << "Average points with x amount of dice left before a Farkle:" << endl;
	for (int i = 0; i < 6; i++)
	{
		pointsAverage[i] /= setsRolled;
		chanceOfFarkle[i] /= setsRolled;
		cout << i + 1 << " Dice left: " << pointsAverage[i] << " extra points with a " << setprecision(4) << chanceOfFarkle[i] * 100 << "% chance of Farkling on that turn." << endl;
	}

	for (int i = 0; i < 6; i++)
	{
		if (pointsAverage[i] > bestQuantityDiceLeft)
		{
			bestQuantityDiceLeft = i;
		}
	}

	tempFileIn.close();

	return bestQuantityDiceLeft;
}

void progressBar(int currentProgress, int progressOutOf) //this will later be used to try and predict how long it will take before a process is finished.
{
	//will do stuff later
}

//input validation takes 3 parameters. The char that should return true, the char that returns false, the prompt to be validated.
bool inputVal(char optionTrue, char optionFalse, string prompt)
{
	char secondTrue = optionTrue + 32; //gets lowercase version of the acceptable char
	char secondFalse = optionFalse + 32; //gets lowercase version of the acceptable char
	char choice;

	cout << prompt << endl;
	cin >> choice;

	while (choice != optionTrue && choice != optionFalse && choice != secondTrue && choice != secondFalse)
	{
		cout << "Invalid input! " << prompt << endl;
		cin >> choice;
	}

	if (choice == optionTrue || choice == secondTrue)
		return true;
	if (choice == optionFalse || choice == secondFalse)
		return false;
}