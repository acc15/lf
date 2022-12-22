#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <config/config.hpp>

#include "../test_util.hpp"

using namespace leafsync;

void cmp_mirror(const config_mirror& l, const config_mirror& r) {
	CHECK( l.mode == r.mode );
	CHECK( l.path == r.path );
}

void cmp_sync(const config_sync& l, const config_sync& r) {
	CHECK( l.name == r.name );
	CHECK( l.index == r.index );
	{
		INFO(".left");
		cmp_mirror(l.left, r.left);
	}
	{
		INFO(".right");
		cmp_mirror(l.right, r.right);
	}
}

void cmp_syncs(const std::vector<config_sync>& l, const std::vector<config_sync>& r) {
	REQUIRE( l.size() == r.size() );
	for (size_t i = 0; i < l.size(); i++) {
		INFO("syncs[" << i << "]");
		cmp_sync(l[i], r[i]);
	}
}

TEST_CASE("load", "[config]") {
	
	config cfg;

	const auto config_path = leafsync::test_dir / "config" / "config.txt";

	leafsync::error_handler err(config_path.string());
	std::ifstream f(config_path);
	cfg.load(err, f);

	const std::vector<config_sync>& actual_syncs = cfg.syncs();
	const std::vector<config_sync> expected_syncs = {
		config_sync {
			.name = "home", 
			.index = "/home/user/.config/leafsync/leafsync.index",
			.left = config_mirror { .mode = config_mode::WRITE, .path = "/home/user" },
			.right = config_mirror { .mode = config_mode::WRITE, .path = "/mnt/sync" }
		},
		config_sync {
			.name = "pic", 
			.index = "/home/user/.config/leafsync/pic.index",
			.left = config_mirror { .mode = config_mode::READ, .path = "/home/user/Pictures" },
			.right = config_mirror { .mode = config_mode::WRITE, .path = "/mnt/images" }
		},
		config_sync {
			.name = "rus", 
			.index = "/home/user/тест/индекс.index",
			.left = config_mirror { .mode = config_mode::WRITE, .path = "/home/user/тест" },
			.right = config_mirror { .mode = config_mode::WRITE, .path = "/mnt/Проверка" }
		},
		config_sync {
			.name = "greek", 
			.index = "/home/user/δοκιμή/δείκτης.index",
			.left = config_mirror { .mode = config_mode::WRITE, .path = "/home/user/δοκιμή" },
			.right = config_mirror { .mode = config_mode::WRITE, .path = "/mnt/δοκιμή" }
		},
		config_sync {
			.name = "trim", 
			.index = "/home/user/test.index",
			.left = config_mirror { .mode = config_mode::WRITE, .path = "/home/user/test" },
			.right = config_mirror { .mode = config_mode::READ, .path = "/mnt/test" }
		}
	};

	cmp_syncs(expected_syncs, actual_syncs);

	REQUIRE( cfg.find("abc") == nullptr );
	REQUIRE( cfg.find("pic") == &actual_syncs[1] );

}

bool test_has_errors(std::string text) {
	test_error_handler err;

	const std::string p1 = (test_dir / "a").string();
	const std::string p2 = (test_dir / "b").string();
	const std::string formatted = fmt::format(fmt::runtime(text), p1, p2);

	std::stringstream ss(formatted);
	config().load(err, ss);
	
	UNSCOPED_INFO("text: " << formatted);
	for (const auto& m: err.messages) {
		UNSCOPED_INFO("error: " << m);
	}

	return err.has_errors;
}

TEST_CASE("errors", "[config]") {

	// valid case 
	CHECK_FALSE( test_has_errors("a:b//r:{}//w:{}") );
	
	// empty component
	CHECK( test_has_errors(":b//r:{}//w:{}") );
	CHECK( test_has_errors("a://r:{}//w:{}") );
	CHECK( test_has_errors("a:b//:{}//w:{}") );
	CHECK( test_has_errors("a:b//r://w:{}") );
	CHECK( test_has_errors("a:b//r:{}//:{}") );
	CHECK( test_has_errors("a:b//r:{}//w:") );
	
	// illegal mode
	CHECK( test_has_errors("a:b//x:{}//w:{}") );
	CHECK( test_has_errors("a:b//r:{}//z:{}") );

	// non absolute paths
	CHECK( test_has_errors("a:b//r:c//w:{}") );
	CHECK( test_has_errors("a:b//r:{}//w:d") );

}
