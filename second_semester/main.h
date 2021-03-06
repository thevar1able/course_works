#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#define __DEBUG__ 0

#define true 1
#define false 0

#define G_MAIN 1
#define G_SETTINGS 2
#define G_STARTING 3 
#define G_GAME 4

struct point
{
	int x;
	int y;
};

struct graphic
{
	SDL_Surface* screen;
	TTF_Font* font;
	int state;
	int tempDiff;
	struct point selCell;
};

struct game
{
	wchar_t field[7][7];
	int markField[7][7];
	wchar_t usedDictionary[100][30];
	int difficulty;
	int isAuto;
	int nextMove;
	int fieldSize;
	int score[1];
};

struct wordGuess
{
	wchar_t word[30];
	struct point pos;
};

int main(int argc, char *argv[]);

struct game* initGame(int, int);
void fillFirstWord(struct game*);
wchar_t* findWordInDictionary(int len);
struct point* findClosestPoints(struct point, struct game*);
struct point makePoint(int, int);
void BFS(struct game*, struct point*, int);
wchar_t** findWordsInGame(struct game*, struct point, int);
void BFSWalk(struct game*, struct point, wchar_t*, wchar_t**);
wchar_t** checkCombos(wchar_t**, int);
void writeToUserDictionary(wchar_t*);
void pushWordToUsed(struct game*, wchar_t*);
int wasWordPlayed(struct game*, wchar_t*);
int checkIfSubstring(wchar_t*, wchar_t*);
int computerMove(struct game*, int);
struct point simpleWordCheck(struct game*, wchar_t*, struct point);
struct point recursiveWordCheck(struct game*, wchar_t*, struct point);
int loopGiveUp(struct game*, struct graphic*);
void loopWhoWon(struct game*, struct graphic*, int);
int checkEndGame(struct game*);
void strrev(wchar_t*);

void startRender(struct graphic*);
struct graphic* initGraphics(void);
void drawRect(SDL_Surface*, int, int, int, int, int);
void drawHeader(SDL_Surface*);
void drawSettings(SDL_Surface*, int);
void drawMainMenu(SDL_Surface*, int);
void drawMenu(SDL_Surface*);
void drawStartGameDialog(SDL_Surface*, int);
void loopSettings(struct graphic*);
void loopMenu(struct graphic*);
void loopStartDialog(struct graphic*);
void loopWordAlreadyPlayed(struct graphic*);
void loopGame(struct graphic*, int);
int loopAddWord(struct graphic*);
void drawStatusBar(struct graphic*, struct game*);
void drawField(struct graphic*, struct game*, int);
void drawReadAnythingDialog(struct graphic*, wchar_t*);


