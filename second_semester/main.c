#include "main.h"

struct game* fakeInit()
{
	struct game* newGame = (struct game*)malloc(sizeof(struct game));
	for (int i = 0; i < 100; ++i)
	{
		newGame->usedDictionary[i][0] = L'\0';
	}

	newGame->fieldSize = 7;
	// memset(newGame->field, 0, 49); 

	for (int i = 0; i < 7; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			newGame->field[i][j] = L' ';	
		}
	}

	memset(newGame->markField, 0, 49);

	fillFirstWord(newGame);
	return newGame;
}

int main()
{
	srand(time(NULL));
	setlocale(LC_ALL, "");
	if(__DEBUG__)
	{
		struct game* newGame = fakeInit();
		gameLoop(newGame);
	}
	else
	{
		struct graphic* graphics = initGraphics();

		startRender(graphics);
		SDL_Quit();		
	}

	return 0;
}