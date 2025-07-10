#pragma once
#include "WindowBridge.h"

class RenderingEngine
{
public:
	RenderingEngine();
	void draw();
	~RenderingEngine();
private:
	WindowBridge window;
};

