#include "../../nclgl/Window.h"
#include "../../nclgl/OGLRenderer.h"
#include "Renderer.h"

#pragma comment(lib, "nclgl.lib")

int main() {
	Window w("coursework", 800, 600, false);
	if (!w.HasInitialised()){
		return -1;
	}
	Renderer renderer(w);

	if (!renderer.HasInitialised()) {
		return -1;
	}
	w.LockMouseToWindow(true);
//w.ShowOSPointer(true);
	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}
	return 0;
}