#include "main.h"

int main(int argc, char *argv[])
{
	srand(time(NULL));
	setlocale(LC_ALL, "");

	struct graphic* graphics = initGraphics();
	startRender(graphics);

	SDL_Quit();		

	return 0;
}