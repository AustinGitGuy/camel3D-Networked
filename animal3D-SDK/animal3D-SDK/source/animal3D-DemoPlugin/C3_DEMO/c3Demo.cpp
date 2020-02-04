#include "C3_DEMO/c3DemoState.h"

#include <GL/glew.h>

void c3demoRender(c3_DemoState const* demoState) {

	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw some text
	a3textDraw(demoState->text, 0, 0, -1, 1, 1, 1, 1, "Total time: %f", demoState->renderTimer->totalTime);
}

void c3demoUpdate(c3_DemoState const* demoState) {

}

void c3demoInput(c3_DemoState const* demoState) {

}

void c3demoNetworkingSend(c3_DemoState const* demoState) {

}

void c3demoNetworkingRecieve(c3_DemoState const* demoState) {
	//Copy for loop to here
}