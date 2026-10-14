#include "main.h"

int main(void)
{
	try {
		Core core;
		return 0;
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Engine Crash", MB_ICONERROR);
		return -1;
	}
}