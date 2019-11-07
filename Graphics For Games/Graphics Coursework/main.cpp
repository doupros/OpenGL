#include "../../nclgl/Window.h"
#include "../../nclgl/OGLRenderer.h"


#pragma comment(lib, "nclgl.lib")

int main() {
	Window w("coursework", 800, 600, false);
	if (!w.HasInitialised()){
		return -1;
	}



	return 0;
}