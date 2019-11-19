#pragma comment(lib, "nclgl.lib")
#include "../../nclgl/Window.h"
#include "Renderer.h"

int main() {
	Window w("coursework", 1366, 768, false);
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