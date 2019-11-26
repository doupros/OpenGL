#pragma comment(lib, "nclgl.lib")
#include "../../nclgl/Window.h"
#include "Renderer.h"


bool sceneOne = true;
void ToggleSeneSlect() {
	sceneOne = !sceneOne;
}

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
	w.ShowOSPointer(false);
	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			renderer.TogglePostProssing();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			renderer.ToggleAutoCamera();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
			ToggleSeneSlect();
		}

		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.MoveLight();
		if (sceneOne){
			renderer.RenderScene();
		}
		else {
			renderer.RenderSceneTwo();
		}
	}
	return 0;
}