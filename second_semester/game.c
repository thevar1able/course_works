#include "main.h"

struct game* initGame(int isAuto, int difficulty)
{
	struct game* newGame = (struct game*)malloc(sizeof(struct game));

	newGame->fieldSize = difficulty;
	newGame->isAuto = isAuto;

	newGame->nextMove = 0;

	newGame->score[0] = 700; 
	newGame->score[1] = 800;

	for (int i = 0; i < 100; ++i)
	{
		newGame->usedDictionary[i][0] = L'\0';
	}

	for (int i = 0; i < 7; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			newGame->field[i][j] = L' ';
			newGame->markField[i][j] = 0;	
		}
	}
	memset(newGame->markField, 0, 49);

	fillFirstWord(newGame);

	return newGame;
}

void fillFirstWord(struct game* curGame)
{
	wchar_t* firstWord = findWordInDictionary(curGame->fieldSize);
	if(firstWord)
	{
		int i;
		for(i = 0; i < curGame->fieldSize; ++i)
		{
			curGame->field[curGame->fieldSize / 2][i] = firstWord[i];
		}
		// for(i = 0; i < curGame->fieldSize; ++i)
		// {
		// 	curGame->field[(curGame->fieldSize / 2) + 1][i] = firstWord[i];
		// }
		pushWordToUsed(curGame, firstWord);
	}
}

wchar_t* findWordInDictionary(int len)
{
	FILE *f;
	f = fopen("dictionary", "r+");

	if(f)
	{
		wchar_t* tempStr = (wchar_t*)malloc(sizeof(wchar_t)*40);

		while(fscanf(f, "%ls\n", tempStr) != EOF)
		{
			if((wcslen(tempStr) == (unsigned int)len) && (rand() % 150 + 1 == 42))
			{
				fclose(f);
				printf("%ls\n", tempStr);
				return tempStr;
			}
		}
		fclose(f);
		return NULL;
	}

	fclose(f);
	return NULL;
}

wchar_t** checkCombos(wchar_t** combos, int strict)
{
	FILE *f, *fu;
	int i, len, s = 0;
	f = fopen("dictionary", "r+");
	fu = fopen("userdictionary", "r+");

	wchar_t** words = (wchar_t**)malloc(sizeof(wchar_t*) * 70);
	for(i = 0; i < 70; ++i)
	{
		words[i] = (wchar_t*)malloc(sizeof(wchar_t) * 30);
		words[i][0] = L'\0';
	}

	for(i = 0; wcscmp(combos[i], L""); ++i);
	len = i;

	if(f)
	{
		wchar_t* tempStr = (wchar_t*)malloc(sizeof(wchar_t)*20);
		if(strict)
		{
			while(fscanf(f, "%ls\n", tempStr) != EOF)
			{
				for(i = 0; i < len && wcscmp(tempStr, combos[i]); ++i);
					// printf("%ls and %ls == %i\n", tempStr, combos[i], wcscmp(tempStr, combos[i]));
				if(!wcscmp(tempStr, combos[i]))
				{
					wcscpy(words[s], combos[i]);
					s++;
				}
			}
			while(fscanf(fu, "%ls\n", tempStr) != EOF)
			{
				for(i = 0; i < len && wcscmp(tempStr, combos[i]); ++i);
					// printf("%ls and %ls == %i\n", tempStr, combos[i], wcscmp(tempStr, combos[i]));
				if(!wcscmp(tempStr, combos[i]))
				{
					wcscpy(words[s], combos[i]);
					s++;
				}
			}
		}
		else
		{
			while(fscanf(f, "%ls\n", tempStr) != EOF)
			{	
				for(i = 0; i < len; ++i)
				{
					if(wcslen(tempStr) - 1 == wcslen(combos[i]))
					{
						if(wcsstr(tempStr, combos[i]) || !wcsncmp(tempStr, combos[i], wcslen(combos[i])))
						{
							//printf("%ls and %ls\n", tempStr, combos[i]);
							wcscpy(words[s], tempStr);
							s++;
							if(s > 1 && !wcscmp(words[s], words[s - 1]))s--;
							break;
						}
					}

				}
			}
			while(fscanf(fu, "%ls\n", tempStr) != EOF)
			{	
				for(i = 0; i < len; ++i)
				{
					if(wcslen(tempStr) - 1 == wcslen(combos[i]))
					{
						if(wcsstr(tempStr, combos[i]) || !wcsncmp(tempStr, combos[i], wcslen(combos[i])))
						{
							//printf("%ls and %ls\n", tempStr, combos[i]);
							wcscpy(words[s], tempStr);
							s++;
							if(s > 1 && !wcscmp(words[s], words[s - 1]))s--;
							break;
						}
					}

				}
			}
		}
	}
	return words;
}

wchar_t** findWordsInGame(struct game* curGame, struct point p, int shouldCheck)
{
	int i;
	wchar_t** foundCombos = (wchar_t**)malloc(sizeof(wchar_t*) * 200);
	for(i = 0; i < 200; ++i)
	{
		foundCombos[i] = (wchar_t*)malloc(sizeof(wchar_t) * 40);
		foundCombos[i][0] = L'\0';
	};

	for (int i = 0; i < 7; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			curGame->markField[i][j] = 0;
		}
	}
	
	struct point poi[2];
	poi[0] = p;
	poi[1] = makePoint(-1, -1);
	BFS(curGame, poi, 1);

	wchar_t tmpStr[2] = {curGame->field[p.x][p.y] == L' ' ? '\0' : curGame->field[p.x][p.y], '\0'};
	BFSWalk(curGame, p, tmpStr, foundCombos);

	for(i = 0; wcscmp(foundCombos[i], L""); ++i); //printf("%ls\n", foundCombos[i]);
	//printf("Total combos found: %i\n", i);

	wchar_t** foundWords;
	if(shouldCheck)foundWords = checkCombos(foundCombos, 1); else return foundCombos;

	for(i = 0; i < 150; ++i)free(foundCombos[i]);

	for(i = 0; wcscmp(foundWords[i], L""); ++i); //printf("%ls\n", foundWords[i]);
	//printf("Words found: %i\n", i);

	return foundWords;
}

void BFSWalk(struct game* curGame, struct point p, wchar_t* tmpStr, wchar_t** combos)
{
	int i, s = 0;
	int pos;
	struct point* points = (struct point*)malloc(sizeof(struct point) * 5);

	if((p.x - 1 > -1) && (curGame->markField[p.x - 1][p.y] > curGame->markField[p.x][p.y])) 
	{
		points[s] = makePoint(p.x - 1, p.y);
		s++;
	}
	if((p.x + 1 <= curGame->fieldSize) && (curGame->markField[p.x + 1][p.y] > curGame->markField[p.x][p.y])) 
	{
		points[s] = makePoint(p.x + 1, p.y);
		s++;
	}
	if((p.y - 1 > -1) && (curGame->markField[p.x][p.y - 1] > curGame->markField[p.x][p.y])) 
	{
		points[s] = makePoint(p.x, p.y - 1);
		s++;
	}
	if((p.y + 1 <= curGame->fieldSize) && (curGame->markField[p.x][p.y + 1] > curGame->markField[p.x][p.y])) 
	{
		points[s] = makePoint(p.x, p.y + 1);
		s++;
	}

	points[s].x = -1;

	for(i = 0; wcscmp(combos[i], L"") && i < 195; ++i);
	pos = i;
	
	for(i = 0; points[i].x != -1; ++i)
	{
		int j;
		wchar_t tempString[40];
		wcscpy(tempString, tmpStr);
		for(j = 0; tempString[j] != L'\0'; ++j);
		if(curGame->field[points[i].x][points[i].y] >= L'а' && curGame->field[points[i].x][points[i].y] <= L'я')
		{
			tempString[j] = curGame->field[points[i].x][points[i].y]; 
			tempString[j + 1] = L'\0';
		}
	
		if(wcslen(tempString) > 1)
		{
			//printf("%ls\n", tempString);
			if(pos < 200)wcscpy(combos[pos], tempString);
			pos++;
		}
	}

	for (i = 0; points[i].x != -1 && i < 5; ++i)
	{
		int j;
		wchar_t tempString[40];
		wcscpy(tempString, tmpStr);
		for(j = 0; tempString[j] != L'\0'; ++j);
		if(curGame->field[points[i].x][points[i].y] >= L'а' && curGame->field[points[i].x][points[i].y] <= L'я')
		{
			tempString[j] = curGame->field[points[i].x][points[i].y]; 
			tempString[j + 1] = L'\0';
		}
		BFSWalk(curGame, points[i], tempString, combos);
	}
}

struct point makePoint(int x, int y)
{
	struct point temp;
	temp.x = x;
	temp.y = y;
	return temp;
}

void BFS(struct game* curGame, struct point* p, int level)
{
	int i, j, s = 0;
	struct point* points = (struct point*)malloc(sizeof(struct point)*16);
	struct point* temp;

	for(i = 0; p[i].x != -1; ++i)
	{
		curGame->markField[p[i].x][p[i].y] = level;
	}

	for(i = 0; p[i].x != -1; ++i)
	{
		temp = findClosestPoints(makePoint(p[i].x, p[i].y), curGame);
		for (j = 0; temp[j].x != -1; ++j)
		{
			points[s] = temp[j];
			s++;
		}
		points[s] = temp[j];
	}

	if(s > 0) BFS(curGame, points, level + 1); 
}

struct point* findClosestPoints(struct point p, struct game* curGame)
{
	int s = 0;
	struct point* points = (struct point*)malloc(sizeof(struct point) * 5);

	if((p.x - 1 > -1) && (curGame->field[p.x - 1][p.y] != ' ') && (!curGame->markField[p.x - 1][p.y])) 
	{
		points[s] = makePoint(p.x - 1, p.y);
		s++;
	}
	if((p.x + 1 <= curGame->fieldSize) && (curGame->field[p.x + 1][p.y] != ' ') && (!curGame->markField[p.x + 1][p.y])) 
	{
		points[s] = makePoint(p.x + 1, p.y);
		s++;
	}
	if((p.y - 1 > -1) && (curGame->field[p.x][p.y - 1] != ' ') && (!curGame->markField[p.x][p.y - 1])) 
	{
		points[s] = makePoint(p.x, p.y - 1);
		s++;
	}
	if((p.y + 1 <= curGame->fieldSize) && (curGame->field[p.x][p.y + 1] != ' ') && (!curGame->markField[p.x][p.y + 1])) 
	{
		points[s] = makePoint(p.x, p.y + 1);
		s++;
	}

	points[s].x = -1;

	return points;
}

int gameLoop(struct game* curGame)
{
	int x, y, x1, y1;
	wchar_t* tempChar = (wchar_t*)malloc(sizeof(wchar_t)*30);
	wchar_t insertChar;
	computerMove(curGame);
	// while(1)
	// {
	// 	printf("StartPos: \n");
	// 	scanf("%i %i", &x, &y);
	// 	printf("Word: \n");
	// 	scanf("\n%ls", tempChar);
	// 	printf("InsertPos: \n");
	// 	scanf("\n%i %i", &x1, &y1);
	// 	printf("InsertChar:\n");
	// 	scanf("\n%1lc", &curGame->field[x1][y1]);
	// 	insertChar = curGame->field[x1][y1];
	// 	wchar_t** words = findWordsInGame(curGame, makePoint(x, y), 0);

	// 	for (int i = 0; wcscmp(words[i], L""); ++i)
	// 	{
	// 		if(!wcscmp(tempChar, words[i]))
	// 		{
	// 			printf("%i\n", tempChar[0] == words[i][0]);
	// 			printf("Ок есть. +%i", wcslen(tempChar));
	// 			curGame->field[x1][y1] = insertChar;
	// 			computerMove(curGame);
	// 			break;
	// 		}
	// 		else
	// 		{
	// 			curGame->field[x1][y1] = L' ';
	// 		}
	// 	}
	// 	drawField1(curGame);

	// }
	return 0;
}

void writeToUserDictionary(wchar_t* str)
{
	FILE *f;
	f = fopen("userdictionary", "a+");

	if(f)
	{
		fprintf(f, "%ls\n", str);
	}

	fclose(f);
}

void pushWordToUsed(struct game* curGame, wchar_t* str)
{
	int i;
	for(i = 0; wcscmp(curGame->usedDictionary[i], L"") && i < 50; ++i);
	wcscpy(curGame->usedDictionary[i], str);
}

int wasWordPlayed(struct game* curGame, wchar_t* str)
{
	int i;
	for(i = 0; wcscmp(curGame->usedDictionary[i], L""); ++i)
	{
		if(!wcscmp(curGame->usedDictionary[i], str)) return 1;
	}

	return 0;
}

int checkIfSubstring(wchar_t* haystack, wchar_t* needle)
{
	int retCode = 0;
	if(!wcsstr(haystack, needle) && (haystack[0] != needle[0]))
	{
		strrev(haystack);
		retCode = !wcsstr(haystack, needle) && (haystack[0] != needle[0]) ? 1 : 0;
		strrev(haystack);
	}

	return retCode;
}

void strrev(wchar_t *p)
{
	wchar_t *q = p;
	while(q && *q) ++q;
	for(--q; p < q; ++p, --q)
    	*p = *p ^ *q,
    	*q = *p ^ *q,
    	*p = *p ^ *q;
}


int computerMove(struct game* curGame)
{
	struct wordGuess guessArray[150];
	struct wordGuess suggests[10];
	int pos = 0;
	int max_len = 0;
	int max_pos = 0;
	wchar_t** words;

	for (int i = 0; i < curGame->fieldSize; ++i)
	{
		for (int j = 0; j < curGame->fieldSize; ++j)
		{
			if(curGame->field[i][j] == L' ')
			{
				if( curGame->field[i][j + 1] != L' ' || 
					curGame->field[i][j - 1] != L' ' ||
					curGame->field[i + 1][j] != L' ' ||
					curGame->field[i - 1][j] != L' ' )
				{
					words = findWordsInGame(curGame, makePoint(i, j), 0);
					words = checkCombos(words, 0);
					for (int l = 0; wcscmp(words[l], L""); ++l)
					{
						if(!wasWordPlayed(curGame, words[l]) &&
							checkIfSubstring(curGame->usedDictionary[0], words[l]))
						{
							wcscpy(guessArray[pos].word, words[l]);
							guessArray[pos].pos = makePoint(j, i);
							pos++;
						}
					}
				}
			}
		}
	}

	// for (int l = 0; l < pos; ++l)printf("%ls at %i,%i\n", guessArray[l].word, guessArray[l].pos.x, guessArray[l].pos.y);

	for (int i = 0; i < pos; ++i)
	{
		if(wcslen(guessArray[i].word) == max_len)
		{
			suggests[max_pos] = guessArray[i];
			max_pos++;
		}
		if(wcslen(guessArray[i].word) > max_len)
		{
			max_len = wcslen(guessArray[i].word);
			suggests[0] = guessArray[i];
			max_pos = 1;
		}
	}
	for (int i = max_pos + 1; i < 10; ++i)
	{
		suggests[i] = guessArray[i - max_pos];
	}

	int i;
	struct point tempPoint;
	for (i = 0; i < max_pos + 1; ++i)
	{
		printf("Предлагаемое слово - %ls\n", suggests[i].word);
		tempPoint = simpleWordCheck(curGame, suggests[i].word, suggests[i].pos);
		printf("Пробуем простой алгоритм\n");
		if(tempPoint.x >= 0)
		{
			printf("Найдена клетка для подстановки\n");
			curGame->field[tempPoint.y][tempPoint.x] = suggests[i].word[0];
			break;
		}
		else
		{
			printf("Смена алгоритма\n");
			tempPoint = recursiveWordCheck(curGame, suggests[i].word, suggests[i].pos);
			if(tempPoint.x >= 0)
			{
				printf("Найдена клетка для подстановки\n");
				printf("%i %i\n", tempPoint.x, tempPoint.y);
				curGame->field[tempPoint.y][tempPoint.x] = suggests[i].word[wcslen(suggests[i].word) - 1];
				break;
			}
			else
			{
				//try again
			}
		}
	}

	if(tempPoint.x >= 0)
	{
		pushWordToUsed(curGame, suggests[i].word);
		return wcslen(suggests[i].word);
	}
	else return 0;
}

struct point simpleWordCheck(struct game* curGame, wchar_t* word, struct point pos)
{
	if(curGame->field[pos.y][pos.x] == L' ' && 
		(   curGame->field[pos.y][pos.x + 1] == word[1] || 
			curGame->field[pos.y][pos.x - 1] == word[1] ||
			curGame->field[pos.y + 1][pos.x] == word[1] ||
			curGame->field[pos.y - 1][pos.x] == word[1]  ))
	{
		return pos;
	}
	return makePoint(-1, -1);
}

struct point recursiveWordCheck(struct game* curGame, wchar_t* word, struct point pos)
{
	struct point tempPoint = makePoint(pos.x, pos.y);
	struct point prevPos = makePoint(pos.x, pos.y);

	printf("%lc\n", curGame->field[pos.y][pos.x]);
	for (int i = 0; i < wcslen(word) - 1; ++i)
	{
		prevPos = pos;
		pos = tempPoint;
		// printf("iteration\n");
		// printf("looking for %lc\n", word[i]);
		if(curGame->field[tempPoint.y][tempPoint.x + 1] == word[i] && (tempPoint.x + 1 < curGame->fieldSize)) tempPoint = makePoint(tempPoint.x + 1, tempPoint.y);
		else if (curGame->field[tempPoint.y][tempPoint.x - 1] == word[i] && (tempPoint.x - 1 >= 0)) tempPoint = makePoint(tempPoint.x - 1, tempPoint.y);
		else if (curGame->field[tempPoint.y + 1][tempPoint.x] == word[i] && (tempPoint.y + 1 < curGame->fieldSize)) tempPoint = makePoint(tempPoint.x, tempPoint.y + 1);
		else if (curGame->field[tempPoint.y - 1][tempPoint.x] == word[i] && (tempPoint.y - 1 >= 0)) tempPoint = makePoint(tempPoint.x, tempPoint.y - 1);
		if(tempPoint.x == pos.x && tempPoint.y == pos.y) 
		{
			if(curGame->field[tempPoint.y][tempPoint.x - 1] == word[i] && (tempPoint.x - 1 >= 0)) tempPoint = makePoint(tempPoint.x + 1, tempPoint.y);
			else if (curGame->field[tempPoint.y][tempPoint.x + 1] == word[i] && (tempPoint.x + 1 < curGame->fieldSize)) tempPoint = makePoint(tempPoint.x - 1, tempPoint.y);
			else if (curGame->field[tempPoint.y - 1][tempPoint.x] == word[i] && (tempPoint.y - 1 >= 0)) tempPoint = makePoint(tempPoint.x, tempPoint.y - 1);
			else if (curGame->field[tempPoint.y + 1][tempPoint.x] == word[i] && (tempPoint.y + 1 < curGame->fieldSize)) tempPoint = makePoint(tempPoint.x, tempPoint.y + 1);
		}
		if(tempPoint.x == pos.x && tempPoint.y == pos.y) return makePoint(-1, -1);
		if(tempPoint.x == prevPos.x && tempPoint.y == prevPos.y) return makePoint(-1, -1);
	}
	if(curGame->field[tempPoint.y][tempPoint.x + 1] == L' ') return makePoint(tempPoint.x + 1, tempPoint.y);
	if(curGame->field[tempPoint.y][tempPoint.x - 1] == L' ') return makePoint(tempPoint.x - 1, tempPoint.y);
	if(curGame->field[tempPoint.y + 1][tempPoint.x] == L' ') return makePoint(tempPoint.x, tempPoint.y + 1);
	if(curGame->field[tempPoint.y - 1][tempPoint.x] == L' ') return makePoint(tempPoint.x, tempPoint.y - 1);
	return makePoint(-1, -1);
}