#include <catch2/catch_session.hpp>

#include <locale>

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char* argv[]) {
	std::locale::global(std::locale("C.UTF-8"));
#ifdef WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
	return Catch::Session().run(argc, argv);
}