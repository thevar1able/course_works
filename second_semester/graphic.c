#include "main.h"

void drawField1(struct game* curGame)
{
	int i, j;
	printf("\n");
	for(j = 0; j < curGame->fieldSize; ++j)
	{
		printf("+");
		for (i = 0; i < curGame->fieldSize; ++i)
		{
			printf("---+");
		}
		printf("\n|");
		for (i = 0; i < curGame->fieldSize; ++i)
		{
			printf(" %1lc |", curGame->field[j][i]);
		}
		printf("\n");
	}
	printf("+");
	for (i = 0; i < curGame->fieldSize; ++i)
	{
		printf("---+");
	}
	printf("\n");
}

void startRender(struct graphic* graphics)
{
    loopMenu(graphics);
    return;
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
                    if(highlight == 0) loopStartDialog(graphics);
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
                case SDLK_RETURN: loopGame(graphics, players); drawHeader(graphics->screen); break;
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
                                    curGame->nextMove = !curGame->nextMove;
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
                                            curGame->nextMove = !curGame->nextMove;
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
                    case SDLK_UP: graphics->selCell.y--; break;
                    case SDLK_DOWN: graphics->selCell.y++; break;
                    case SDLK_LEFT: graphics->selCell.x--; break;
                    case SDLK_RIGHT: graphics->selCell.x++; break;
                    case SDLK_ESCAPE: moveState = 0; acceptMove = 0; tmpStr[0] = L'\0'; insChar[0] = L'\0'; break;
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

void drawReadAnythingDialog(struct graphic* graphics, wchar_t* anything)
{
    char *tmpStr = (char*)malloc(sizeof(char)*70);
    SDL_Rect rDialog = {211, 270};

    drawRect(graphics->screen, 195, 255, 410, 55, 0);
    drawRect(graphics->screen, 199, 259, 402, 52, 0xffffff);
    drawRect(graphics->screen, 200, 260, 400, 50, 0);
    drawRect(graphics->screen, 202, 262, 396, 46, 0xffffff);
    drawRect(graphics->screen, 203, 263, 394, 44, 0);

    SDL_Color txtColor = {255, 255, 255};
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 25);
    sprintf(tmpStr, "%ls", anything);
    SDL_Surface *dialog = TTF_RenderUTF8_Blended(font, tmpStr, txtColor);

    SDL_BlitSurface(dialog, NULL, graphics->screen, &rDialog);

    free(font);
    SDL_FreeSurface(dialog);
    free(tmpStr);
}

struct graphic* initGraphics()
{
    const int screen_width = 800;
    const int screen_height = 600;

    struct graphic* graphics = (struct graphic*)malloc(sizeof(struct graphic));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Yet another shitty game", NULL);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_EnableUNICODE(true);
    TTF_Init();

    graphics->screen = SDL_SetVideoMode(screen_width, screen_height, 32, SDL_HWSURFACE);
    graphics->tempDiff = 7;

    return graphics;
}

void drawRect(SDL_Surface* screen, int x, int y, int width, int height, int color)
{
    SDL_Rect rect = {x, y, width, height};
    SDL_FillRect(screen, &rect, color);
}

void drawHeader(SDL_Surface* screen)
{
    SDL_Rect rLogo = {50, 25};

    SDL_Surface *logo = IMG_Load("logo.png");
    SDL_BlitSurface(logo, NULL, screen, &rLogo);
}

void drawMainMenu(SDL_Surface* screen, int highlight)
{
    SDL_Color txtColor = {255, 255, 255};
    SDL_Rect rNewGame = {320, 400};
    SDL_Rect rSettings = {333, 435};
    SDL_Rect rQuit = {363, 470};

    drawRect(screen, 300, 400, 200, 120, 0);

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 25);
    if(!font)SDL_Quit();
    SDL_Surface *newGame = TTF_RenderUTF8_Blended(font, "Начать игру", txtColor);
    SDL_Surface *settings = TTF_RenderUTF8_Blended(font, "Настройки", txtColor);
    SDL_Surface *quit = TTF_RenderUTF8_Blended(font, "Выход", txtColor);

    drawRect(screen, 300, 416 + (35 * highlight), 200, 2, 0xffffff);

    SDL_BlitSurface(newGame, NULL, screen, &rNewGame);
    SDL_BlitSurface(settings, NULL, screen, &rSettings);
    SDL_BlitSurface(quit, NULL, screen, &rQuit);

    free(font);
    SDL_FreeSurface(newGame);
    SDL_FreeSurface(settings);
    SDL_FreeSurface(quit);

    // drawRect(screen, 400, 0, 1, 600, 0xffffff);
}

void drawSettings(SDL_Surface* screen, int diff)
{
    char *tmpStr = (char*)malloc(sizeof(char)*30);
    SDL_Rect rDiff = {250, 360};

    drawRect(screen, 195, 345, 410, 55, 0);
    drawRect(screen, 199, 349, 402, 52, 0xffffff);
    drawRect(screen, 200, 350, 400, 50, 0);
    drawRect(screen, 202, 352, 396, 46, 0xffffff);
    drawRect(screen, 203, 353, 394, 44, 0);

    SDL_Color txtColor = {255, 255, 255};
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 25);
    sprintf(tmpStr, "Размер поля: ◀  %i  ▶", diff);
    SDL_Surface *difficulty = TTF_RenderUTF8_Blended(font, tmpStr, txtColor);

    SDL_BlitSurface(difficulty, NULL, screen, &rDiff);

    free(font);
    SDL_FreeSurface(difficulty);
    free(tmpStr);
}

void drawStartGameDialog(SDL_Surface* screen, int num)
{
    char *tmpStr = (char*)malloc(sizeof(char)*50);

    drawRect(screen, 155, 345, 490, 55, 0);
    drawRect(screen, 159, 349, 482, 52, 0xffffff);
    drawRect(screen, 160, 350, 480, 50, 0);
    drawRect(screen, 162, 352, 476, 46, 0xffffff);
    drawRect(screen, 163, 353, 474, 44, 0);

    SDL_Color txtColor = {255, 255, 255};
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 25);
    sprintf(tmpStr, "Количество игроков: ◀  %s  ▶", num ? "один" : "два");
    SDL_Surface *startDialog = TTF_RenderUTF8_Blended(font, tmpStr, txtColor);

    int w;
    TTF_SizeUTF8(font, tmpStr, &w, NULL);
    SDL_Rect rDialog = {(800 - w) / 2, 360};

    SDL_BlitSurface(startDialog, NULL, screen, &rDialog);

    TTF_CloseFont(font);
    SDL_FreeSurface(startDialog);
    free(tmpStr);
}

void drawStatusBar(struct graphic* graphics, struct game* curGame)
{
    char *strPlayer1 = (char*)malloc(sizeof(char)*30);
    char *strPlayer2 = (char*)malloc(sizeof(char)*30);
    SDL_Color txtColor = {255, 255, 255};
    SDL_Rect rPlayer1 = {10, 0};
    SDL_Rect rPlayer2 = {440, 0};
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 25);

    sprintf(strPlayer1, "%s %16i X", curGame->isAuto ? "Игрок" : "Игрок 1", curGame->score[0]);
    sprintf(strPlayer2, "%i", curGame->score[1]);

    SDL_Surface* player1 = TTF_RenderUTF8_Blended(font, strPlayer1, txtColor);
    SDL_Surface* player2_score = TTF_RenderUTF8_Blended(font, strPlayer2, txtColor);
    SDL_Surface* player2_str = TTF_RenderUTF8_Blended(font, curGame->isAuto ? "Компьютер" : "Игрок 2", txtColor);

    int w;
    TTF_SizeUTF8(font, curGame->isAuto ? "Компьютер" : "Игрок 2", &w, NULL);
    SDL_Rect rPlayer2_str = {790 - w, 0};

    drawRect(graphics->screen, 0, 0, 800, 50, 0);

    SDL_BlitSurface(player1, NULL, graphics->screen, &rPlayer1);
    SDL_BlitSurface(player2_score, NULL, graphics->screen, &rPlayer2);
    SDL_BlitSurface(player2_str, NULL, graphics->screen, &rPlayer2_str);

    TTF_CloseFont(font);
}

void drawField(struct graphic* graphics, struct game* curGame, int acceptMove)
{
    SDL_Color txtColor = {255, 255, 255};
    SDL_Color invTxtColor = {231, 76, 60};
    const int cellSize = 64;
    const int topMargin = (800 - cellSize * curGame->fieldSize) / 2;
    const int leftMargin = (600 - cellSize * curGame->fieldSize) / 2 - 20;
    char tmpChar[3];
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 47);

    drawRect(graphics->screen, 176, 56, cellSize * 7, cellSize * 7, 0);

    for (int i = 0; i < curGame->fieldSize; ++i)
    {
        for (int j = 0; j < curGame->fieldSize; ++j)
        {
            SDL_Rect rLetter = {topMargin + 17 + cellSize * j, leftMargin + 3 + cellSize * i};
            sprintf(tmpChar, "%1lc", curGame->field[i][j]);
            SDL_Surface* letter = TTF_RenderUTF8_Blended(font, tmpChar, txtColor);
            SDL_BlitSurface(letter, NULL, graphics->screen, &rLetter);
        }
    }

    drawRect(graphics->screen, topMargin + cellSize * graphics->selCell.x, leftMargin + cellSize * graphics->selCell.y, cellSize, cellSize, acceptMove ? 0x67ce48 : 0xe74c3c);
    SDL_Rect rLetter = {topMargin + 17 + cellSize * graphics->selCell.x, leftMargin + 3 + cellSize * graphics->selCell.y};
    sprintf(tmpChar, "%1lc", curGame->field[graphics->selCell.y][graphics->selCell.x]);
    SDL_Surface* letter = TTF_RenderUTF8_Blended(font, tmpChar, txtColor);
    SDL_BlitSurface(letter, NULL, graphics->screen, &rLetter);

    drawRect(graphics->screen, topMargin, leftMargin, 2, curGame->fieldSize * cellSize, 0xffffff);
    drawRect(graphics->screen, topMargin, leftMargin, curGame->fieldSize * cellSize, 2, 0xffffff);
    drawRect(graphics->screen, topMargin, leftMargin + curGame->fieldSize * cellSize, curGame->fieldSize * cellSize, 2, 0xffffff);
    drawRect(graphics->screen, topMargin + curGame->fieldSize * cellSize, leftMargin, 2, curGame->fieldSize * cellSize, 0xffffff);
    for (int i = 1; i < curGame->fieldSize; ++i)
    {
        drawRect(graphics->screen, topMargin + i * cellSize, leftMargin, 2, curGame->fieldSize * cellSize, 0xffffff);
        drawRect(graphics->screen, topMargin, leftMargin + i * cellSize, curGame->fieldSize * cellSize, 2, 0xffffff);
    }

    TTF_CloseFont(font);
}