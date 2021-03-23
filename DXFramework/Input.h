// Input class
// Stores keyboard and mouse input.

#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>

class Input
{
	struct Mouse
	{
		int x, y;
		bool left, right, isActive;
	};

public:
	Input();
	void ActivateInput();
	void DeactivateInput();
	inline bool IsActive() { return is_active_; }
	void SetKeyDown(WPARAM key);
	void SetKeyUp(WPARAM key);

	bool isKeyDown(int key);
	void setMouseX(int xPosition);
	void setMouseY(int yPosition);
	int getMouseX();
	int getMouseY();
	void setLeftMouse(bool down);
	void setRightMouse(bool down);
	bool isLeftMouseDown();
	bool isRightMouseDown();
	void setMouseActive(bool active);
	bool isMouseActive();

private:
	bool keys[256];
	Mouse mouse;
	bool is_active_;

};

#endif