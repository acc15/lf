#include <catch2/catch_session.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char* argv[]) {
#ifdef WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
	return Catch::Session().run(argc, argv);
}