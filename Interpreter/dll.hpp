#pragma once
#ifdef _WIN32
	#ifdef INTERPRETER_EXPORTS
		#define DLL __declspec(dllexport)
	#else
		#define DLL __declspec(dllimport)
	#endif
#else
	#define DLL
#endif