#include "main.h"

struct game* initGame(int isAuto, int difficulty)
{
	struct game* newGame = (struct game*)malloc(sizeof(struct game));

	newGame->fieldSize = difficulty;
	newGame->isAuto = isAuto;

	newGame->nextMove = 0;

	newGame->score[0] = 0; 
	newGame->score[1] = 0;

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
		pushWordToUsed(curGame, firstWord);
		printf("%i&%i\n", wcslen(firstWord), wcslen(firstWord + sizeof(char)));
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
		while(fscanf(f, "%ls\n", tempStr) != EOF)
		{
			if((wcslen(tempStr) == (unsigned int)len))
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
		wchar_t tempStr[20];
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
	for(int i = 0; i < 200; ++i)
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

	//for(i = 0; i < 200; ++i)free(foundCombos[i]);

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
		BFSWalk(curGame, points[i], tempString, combos);
	}

	// for (i = 0; points[i].x != -1 && i < 5; ++i)
	// {
	// 	int j;
	// 	wchar_t tempString[40];
	// 	wcscpy(tempString, tmpStr);
	// 	for(j = 0; tempString[j] != L'\0'; ++j);
	// 	if(curGame->field[points[i].x][points[i].y] >= L'а' && curGame->field[points[i].x][points[i].y] <= L'я')
	// 	{
	// 		tempString[j] = curGame->field[points[i].x][points[i].y]; 
	// 		tempString[j + 1] = L'\0';
	// 	}
		
	// }
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
	// if(!points)return makePoint(-1, -1);

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


int computerMove(struct game* curGame, int reverse)
{
	struct wordGuess guessArray[150];
	struct wordGuess suggests[10];
	for (int i = 0; i < 10; ++i) suggests[i].word[0] = L'\0';
	int pos = 0;
	int max_len = 0;
	int max_pos = 0;
	wchar_t** words;

	if(reverse)
	{
		for (int i = curGame->fieldSize; i >= 0; --i)
		{
			for (int j = curGame->fieldSize; j >= 0; --j)
			{
				if(curGame->field[i][j] == L' ')
				{
					if( (curGame->field[i][j + 1] != L' ' && (j + 1 < curGame->fieldSize)) || 
						(curGame->field[i][j - 1] != L' ' && (j - 1 >= 0)) ||
						(curGame->field[i + 1][j] != L' ' && (i + 1 < curGame->fieldSize)) ||
						(curGame->field[i - 1][j] != L' ' && (i - 1 >= 0)) )
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
							//free(words[l]);
						}
						//free(words);
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < curGame->fieldSize; ++i)
		{
			for (int j = 0; j < curGame->fieldSize; ++j)
			{
				if(curGame->field[i][j] == L' ')
				{
					if( (curGame->field[i][j + 1] != L' ' && (j + 1 < curGame->fieldSize)) || 
						(curGame->field[i][j - 1] != L' ' && (j - 1 >= 0)) ||
						(curGame->field[i + 1][j] != L' ' && (i + 1 < curGame->fieldSize)) ||
						(curGame->field[i - 1][j] != L' ' && (i - 1 >= 0)) )
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
							//free(words[l]);
						}
						//free(words);
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
	struct point tempPoint = makePoint(-1, -1);
	for (i = 0; wcscmp(suggests[i].word, L"") && i < 10; ++i)
	{
		printf("%i\n", wcslen(suggests[i].word));
		printf("Предлагаемое слово - %ls\n", suggests[i].word);
		tempPoint = simpleWordCheck(curGame, suggests[i].word, suggests[i].pos);
		printf("Пробуем простой алгоритм\n");
		if(tempPoint.x >= 0)
		{
			printf("Найдена клетка для подстановки, %i,%i\n", tempPoint.y, tempPoint.x);
			curGame->field[tempPoint.y][tempPoint.x] = suggests[i].word[0];
			break;
		}
		else
		{
			printf("Смена алгоритма\n");
			// printf("dbg_before: %i, %i\n", tempPoint.y, tempPoint.x);
			tempPoint = recursiveWordCheck(curGame, suggests[i].word, suggests[i].pos);
			if(tempPoint.x >= 0 && tempPoint.x < curGame->fieldSize && curGame->field[tempPoint.y][tempPoint.x] == L' ' &&
				((curGame->field[tempPoint.y - 1][tempPoint.x] == suggests[i].word[wcslen(suggests[i].word) - 2] && tempPoint.y >= 0) ||
				(curGame->field[tempPoint.y + 1][tempPoint.x] == suggests[i].word[wcslen(suggests[i].word) - 2] && tempPoint.y < curGame->fieldSize) ||
				(curGame->field[tempPoint.y][tempPoint.x - 1] == suggests[i].word[wcslen(suggests[i].word) - 2] && tempPoint.x >= 0) ||
				(curGame->field[tempPoint.y][tempPoint.x + 1] == suggests[i].word[wcslen(suggests[i].word) - 2] && tempPoint.x < curGame->fieldSize)))
			{
				printf("Найдена клетка для подстановки\n");
				printf("%i %i\n", tempPoint.x, tempPoint.y);
				curGame->field[tempPoint.y][tempPoint.x] = suggests[i].word[wcslen(suggests[i].word) - 1];
				break;
			}
			else
			{
				// printf("Увы :(, (dbg: %i, %i)\n", tempPoint.y, tempPoint.x);
			}
		}
	}
	// drawField1(curGame);
	if(tempPoint.x >= 0)
	{
		pushWordToUsed(curGame, suggests[i].word);
		return wcslen(suggests[i].word);
	}
	else 
	{
		printf("Всё очень плохо :(\n");
		if(!reverse)computerMove(curGame, !reverse); else return 0;
	}
}

struct point simpleWordCheck(struct game* curGame, wchar_t* word, struct point pos)
{
	if(curGame->field[pos.y][pos.x] == L' ' && pos.x >= 0 && pos.x < curGame->fieldSize && pos.y < curGame->fieldSize && pos.y >= 0 &&
		(   (curGame->field[pos.y][pos.x + 1] == word[1] && (pos.x + 1 < curGame->fieldSize)) || 
			(curGame->field[pos.y][pos.x - 1] == word[1] && (pos.x - 1 >= 0)) ||
			(curGame->field[pos.y + 1][pos.x] == word[1] && (pos.y + 1 < curGame->fieldSize)) ||
			(curGame->field[pos.y - 1][pos.x] == word[1] && (pos.y - 1 >= 0))  ))
	{
		return pos;
	}
	return makePoint(-1, -1);
}

struct point recursiveWordCheck(struct game* curGame, wchar_t* word, struct point pos)
{
	struct point tempPoint;
	struct point p[4];
	int p_pos = 0;
	// printf("iteration\n");
	// if(wcslen(word) > 1)printf("looking for %lc in %i,%i\n", word[0], pos.y, pos.x); else if(wcslen(word) == 1) printf("looking for empty cell in %i, %i\n", pos.y, pos.x);
	if(wcslen(word) > 1)
	{
		if(curGame->field[pos.y][pos.x + 1] == word[0] && (pos.x + 1 < curGame->fieldSize))
		{ 
			// printf("%lc\n", word[0]);
			p[p_pos] = makePoint(pos.x + 1, pos.y);
			p_pos++;
		}
		if (curGame->field[pos.y][pos.x - 1] == word[0] && (pos.x - 1 >= 0)) 
		{
			// printf("%lc\n", word[0]);
			p[p_pos] = makePoint(pos.x - 1, pos.y);
			p_pos++;
		}
		if (curGame->field[pos.y + 1][pos.x] == word[0] && (pos.y + 1 < curGame->fieldSize)) 
		{
			// printf("%lc\n", word[0]);
			p[p_pos] = makePoint(pos.x, pos.y + 1);
			p_pos++;
		}
		if (curGame->field[pos.y - 1][pos.x] == word[0] && (pos.y - 1 >= 0)) 
		{
			// printf("%lc\n", word[0]);
			p[p_pos] = makePoint(pos.x, pos.y - 1);
			p_pos++;
		}

		if(p_pos > 1)
		{
			for (int i = 0; i < p_pos; ++i)
			{
				// printf("dbg: %i, %i\n", tempPoint.y, tempPoint.x);
				tempPoint = recursiveWordCheck(curGame, word + sizeof(char), p[i]);
				if(tempPoint.x != -1) { return tempPoint; break; }
			}
		}
		else if(p_pos == 1)
		{
			return recursiveWordCheck(curGame, word + sizeof(char), p[0]);
		}
		else
		{
			return makePoint(-1, -1);
		}
	}

	if(wcslen(word) == 1)
	{
		if(curGame->field[pos.y][pos.x + 1] == L' ' && (pos.x + 1 < curGame->fieldSize)) return makePoint(pos.x + 1, pos.y);
		if(curGame->field[pos.y][pos.x - 1] == L' ' && (pos.x - 1 >= 0)) return makePoint(pos.x - 1, pos.y);
		if(curGame->field[pos.y + 1][pos.x] == L' ' && (pos.y + 1 < curGame->fieldSize)) return makePoint(pos.x, pos.y + 1);
		if(curGame->field[pos.y - 1][pos.x] == L' ' && (pos.y - 1 >= 0)) return makePoint(pos.x, pos.y - 1);
		// printf("no empty cell\n");
		return makePoint(-1, -1);
	}
}


void loopMenu(struct graphic* graphics)
{
    int highlight = 0;
    SDL_Event event;

    drawHeader(graphics->screen);
    drawMainMenu(graphics->screen, highlight);
    graphics->state = G_MAIN;

    for(;;)
    {
        SDL_WaitEvent(&event);
        if(event.type == SDL_QUIT)
            break;
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_ESCAPE: return;
                case SDLK_UP: highlight--; break;
                case SDLK_DOWN: highlight++; break;
                case SDLK_RETURN: 
                    if(highlight == 0) { loopStartDialog(graphics); drawRect(graphics->screen, 0, 0, 800, 600, 0); drawHeader(graphics->screen); }
                    if(highlight == 1) loopSettings(graphics);
                    if(highlight == 2) return;
            }
            highlight = highlight > 2 ? 2 : highlight < 0 ? 0 : highlight;
        }
        graphics->state = G_MAIN;
        drawMainMenu(graphics->screen, highlight);
        SDL_Flip(graphics->screen);
    }
}

void loopSettings(struct graphic* graphics)
{
    SDL_Event event;
    if(!graphics->tempDiff) graphics->tempDiff = 3;

    drawSettings(graphics->screen, graphics->tempDiff);
    graphics->state = G_SETTINGS;

    for(;;)
    {
        SDL_WaitEvent(&event);
        if(event.type == SDL_QUIT) return;
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_ESCAPE: 
                case SDLK_RETURN: drawRect(graphics->screen, 195, 345, 410, 60, 0); return; break;
                case SDLK_LEFT: graphics->tempDiff -= 2; break;
                case SDLK_RIGHT: graphics->tempDiff += 2; break;
            }
            graphics->tempDiff = graphics->tempDiff > 7 ? 7 : graphics->tempDiff < 3 ? 3 : graphics->tempDiff;
        }
        drawSettings(graphics->screen, graphics->tempDiff);
        SDL_Flip(graphics->screen);
    }
}

void loopStartDialog(struct graphic* graphics)
{
    SDL_Event event;
    int players = 0;

    drawStartGameDialog(graphics->screen, players);
    graphics->state = G_STARTING; //deprecated

    for(;;)
    {
        SDL_WaitEvent(&event);
        if(event.type == SDL_QUIT) return;
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_ESCAPE: drawRect(graphics->screen, 155, 345, 490, 60, 0); return; break;
                case SDLK_LEFT: players--; break;
                case SDLK_RIGHT: players++; break;
                case SDLK_RETURN: loopGame(graphics, players); drawHeader(graphics->screen); return; break;
            }
            players = players > 0 ? 1 : players < 0 ? 0 : players;
        }
        drawStartGameDialog(graphics->screen, players);
        SDL_Flip(graphics->screen);
    }
}

void loopGame(struct graphic* graphics, int players)
{
    SDL_Event event;
    int moveState = 0;
    int acceptMove = 0;
    int escCounter = 0;
    struct point startPos, insertPos;
    wchar_t* tmpStr = (wchar_t*)malloc(sizeof(wchar_t)*50); tmpStr[0] = L'\0';
    wchar_t* pushStr = (wchar_t*)malloc(sizeof(wchar_t)*50);
    wchar_t insChar[2] = {L'\0'};
    struct game* curGame = initGame(players, graphics->tempDiff);
    curGame->nextMove = 0;
    graphics->selCell = makePoint(0, 0);

    drawRect(graphics->screen, 0, 0, 800, 600, 0);
    drawStatusBar(graphics, curGame);
    drawField(graphics, curGame, acceptMove);
    SDL_Flip(graphics->screen);

    for(;;)
    {
        if(moveState == 5) moveState = 0;
        if(moveState == 6) moveState = 2;
        SDL_WaitEvent(&event);
        if(event.type == SDL_QUIT) 
        {
            SDL_Quit(); 
            exit(0);
        }
        if(event.type == SDL_KEYDOWN)
        {
            if(moveState == 1)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    { 
                        moveState = 0;
                        acceptMove = 0;
                        drawRect(graphics->screen, 195, 345, 410, 60, 0);
                        tmpStr[0] = L'\0';
                        break;
                    }
                    case SDLK_RETURN:
                    {
                        moveState = 6;
                        break;
                    }
                    default:
                    {
                        int i;
                        if(event.key.keysym.unicode >= L'а' && event.key.keysym.unicode <= L'я')
                        {
                            for (i = 0; i < 50 && tmpStr[i] != L'\0'; ++i);
                            if(i < 10)
                            {
                                tmpStr[i] = event.key.keysym.unicode;
                                tmpStr[i + 1] = L'\0';
                            }
                        }
                        if(event.key.keysym.unicode == 8)
                        {
                            for (i = 0; i < 30 && tmpStr[i] != L'\0'; ++i);
                            tmpStr[i - 1] = L'\0';
                        }
                        printf("%i\n", i);
                        break;
                    }
                }
            }
            if(moveState == 3)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    { 
                        moveState = 0;
                        acceptMove = 0;
                        drawRect(graphics->screen, 195, 345, 410, 60, 0);
                        insChar[0] = L'\0';
                        break;
                    }
                    case SDLK_RETURN:
                    {
                        curGame->field[insertPos.y][insertPos.x] = insChar[0];
                        wchar_t** words = findWordsInGame(curGame, makePoint(startPos.y, startPos.x), 1);
                        int shouldClean = 1;

                        for (int i = 0; wcscmp(words[i], L""); ++i)
                        {
                            // printf("%ls, %ls, %i\n", tmpStr, words[i], wcscmp(tmpStr, words[i]));
                            if(!wcscmp(tmpStr, words[i]))
                            {
                                if(wasWordPlayed(curGame, tmpStr))
                                {
                                    shouldClean = -1;
                                    printf("Уже юыло.\n");
                                    loopWordAlreadyPlayed(graphics);
                                }
                                else
                                {
                                    printf("Ок есть. +%i\n", wcslen(tmpStr));
                                    curGame->score[curGame->nextMove] += wcslen(tmpStr);
                                    curGame->field[insertPos.y][insertPos.x] = insChar[0];
                                    drawRect(graphics->screen, 195, 345, 410, 60, 0);
                                    shouldClean = 0;
                                    if(!players)curGame->nextMove = !curGame->nextMove;
                                    pushWordToUsed(curGame, tmpStr);
                                }
                                break;
                            }
                        }
                        if(shouldClean > 0)
                        {
                            wchar_t** words = findWordsInGame(curGame, makePoint(startPos.y, startPos.x), 0);
                            for (int i = 0; wcscmp(words[i], L""); ++i)
                            {
                                if(!wcscmp(tmpStr, words[i]))
                                {
                                    printf("Найдено неизвестное слово.\n");
                                    if(wasWordPlayed(curGame, tmpStr))
                                    {
                                        shouldClean = -1;
                                        printf("Уже юыло.\n");
                                        loopWordAlreadyPlayed(graphics);
                                    }
                                    else
                                    {
                                        if(loopAddWord(graphics))
                                        {
                                            curGame->score[curGame->nextMove] += wcslen(tmpStr);
                                            curGame->field[insertPos.y][insertPos.x] = insChar[0];
                                            drawRect(graphics->screen, 195, 345, 410, 60, 0);
                                            shouldClean = 0;
                                            if(!players)curGame->nextMove = !curGame->nextMove;
                                            pushWordToUsed(curGame, tmpStr);
                                            writeToUserDictionary(tmpStr);
                                        }
                                    }
                                    break;
                                }
                            }
                        }

                        acceptMove = 0;
                        tmpStr[0] = L'\0';
                        insChar[0] = L'\0';
                        moveState = 5;

                        if(players && !shouldClean)
                        {
                            if(!checkEndGame(curGame))
                            {
                                int computerScores = computerMove(curGame, 0);
                                if(computerScores)
                                {
                                    curGame->score[1] += computerScores;
                                }
                                else
                                {
                                    printf("Компьютер сдался =\\ \n");
                                    loopWhoWon(curGame, graphics, 2);
                                    return;
                                }
                            }
                            else
                            {
                                loopWhoWon(curGame, graphics, curGame->score[0] > curGame->score[1] ? 2 : 1);
                                return;
                            }
                        }
                        else if(!shouldClean)
                        {
                            if(checkEndGame(curGame))
                            {
                                loopWhoWon(curGame, graphics, curGame->score[0] > curGame->score[1] ? 1 : 0);
                            }
                        }

                        if(shouldClean)
                        {
                            curGame->field[insertPos.y][insertPos.x] = L' ';
                            if(shouldClean > 0)
                            {
                                drawReadAnythingDialog(graphics, L"Слово не найдено!");
                                moveState = -1;
                            }
                        }
                        break;
                    }
                    default:
                    {
                        int i;
                        if(event.key.keysym.unicode >= L'а' && event.key.keysym.unicode <= L'я')
                        {
                            for (i = 0; i < 3 && insChar[i] != L'\0'; ++i);
                            if(i < 1)
                            {
                                insChar[i] = event.key.keysym.unicode;
                                insChar[i + 1] = L'\0';
                            }
                        }
                        if(event.key.keysym.unicode == 8)
                        {
                            for (i = 0; i < 30 && insChar[i] != L'\0'; ++i);
                            insChar[i - 1] = L'\0';
                        }
                        printf("%i\n", i);
                        break;
                    }
                }
            }
            if(!moveState || moveState == 2)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_UP: graphics->selCell.y--; escCounter = 0; break;
                    case SDLK_DOWN: graphics->selCell.y++; escCounter = 0; break;
                    case SDLK_LEFT: graphics->selCell.x--; escCounter = 0; break;
                    case SDLK_RIGHT: graphics->selCell.x++; escCounter = 0; break;
                    case SDLK_BACKSPACE: computerMove(curGame, 0); break;
                    case SDLK_ESCAPE: moveState = 0; acceptMove = 0; tmpStr[0] = L'\0'; insChar[0] = L'\0'; escCounter++;
                                if(escCounter == 2)
                                {
                                    if(loopGiveUp(curGame, graphics))
                                    {
                                        loopWhoWon(curGame, graphics, curGame->isAuto ? 1 : curGame->nextMove);
                                        return;
                                    }
                                } 
                            break;
                    case SDLK_RETURN:
                        if(moveState == 0)
                        {
                            moveState = 1;
                            acceptMove = 1;
                            startPos = graphics->selCell;
                        }
                        if(moveState == 2 && curGame->field[graphics->selCell.y][graphics->selCell.x] == L' ')
                        {
                            insertPos = graphics->selCell;
                            moveState = 3;
                        }
                }
            }
            if(moveState == -1)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: 
                    {
                        moveState = 0;
                    }
                }
            }

            graphics->selCell.x = graphics->selCell.x > curGame->fieldSize - 1 ? curGame->fieldSize - 1 : graphics->selCell.x < 0 ? 0 : graphics->selCell.x;
            graphics->selCell.y = graphics->selCell.y > curGame->fieldSize - 1 ? curGame->fieldSize - 1 : graphics->selCell.y < 0 ? 0 : graphics->selCell.y;

            drawStatusBar(graphics, curGame);
            drawRect(graphics->screen, 195, 345, 410, 60, 0);
            drawField(graphics, curGame, acceptMove);
            if(moveState == 1)
            {
                wcscpy(pushStr, L"Введите слово: ");
                wcscat(pushStr, tmpStr);
                drawReadAnythingDialog(graphics, pushStr);
            }
            if(moveState == 3)
            {
                wcscpy(pushStr, L"Введите символ: ");
                wcscat(pushStr, insChar);
                drawReadAnythingDialog(graphics, pushStr);
            }
            if(moveState == -1) drawReadAnythingDialog(graphics, L"Слово не найдено!");
            SDL_Flip(graphics->screen);
        }
    }

    return;
}

int checkEndGame(struct game* curGame)
{
    for (int i = 0; i < curGame->fieldSize; ++i)
    {
        for (int j = 0; j < curGame->fieldSize; ++j)
        {
            if(curGame->field[i][j] == L' ')return 0;
        }
    }
    return 1;
}

int loopGiveUp(struct game* curGame, struct graphic* graphics)
{
    SDL_Event event;

    drawReadAnythingDialog(graphics, L"Сдаться?");
    SDL_Flip(graphics->screen);

    for(;;)
    {
        SDL_WaitEvent(&event);
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_RETURN: return 1; break;
                case SDLK_ESCAPE: return 0; break;
            }
        }
    } 
}

void loopWhoWon(struct game* curGame, struct graphic* graphics, int player)
{
    SDL_Event event;

    drawReadAnythingDialog(graphics, player ? player == 2 ? L"Игрок выиграл" : curGame->isAuto ? L"Компьютер выиграл" : L"Игрок выиграл" : L"Второй игрок выиграл");

    SDL_Flip(graphics->screen);

    for(;;)
    {
        SDL_WaitEvent(&event);
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_RETURN:
                case SDLK_ESCAPE: return; break;
            }
        }
    }
}

int loopAddWord(struct graphic* graphics)
{
    SDL_Event event;

    drawReadAnythingDialog(graphics, L"Добавить в словарь?");
    SDL_Flip(graphics->screen);

    for(;;)
    {
        SDL_WaitEvent(&event);
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_RETURN: return 1; break;
                case SDLK_ESCAPE: return 0; break;
            }
        }
    }
}

void loopWordAlreadyPlayed(struct graphic* graphics)
{
    SDL_Event event;

    drawReadAnythingDialog(graphics, L"Слово повторяется");
    SDL_Flip(graphics->screen);

    for(;;)
    {
        SDL_WaitEvent(&event);
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_RETURN:
                case SDLK_ESCAPE: return; break;
            }
        }
    }
}
