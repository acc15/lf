#include <catch2/catch_test_macros.hpp>

#include <string>
#include <sstream>

#include <config/config.hpp>

#include "../test_util.hpp"

using namespace lf;

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
	
	const std::string content = 
		"\n"
        "    # Comments allowed, blank lines are ignored   \n"
    	"    home:" + test_pstr("home/user/.config/leafsync/leafsync.index") + "//w:" + test_pstr("home/user") + "//w:" + test_pstr("mnt/sync") + "   \n"
		"\n"
		"pic:" + test_pstr("home/user/.config/leafsync/pic.index") + "//r:" + test_pstr("home/user/Pictures") + "//w:" + test_pstr("mnt/images") + " \n"
		"rus:" + test_pstr("home/user/тест/индекс.index") + "//w:" + test_pstr("home/user/тест") + "//w:" + test_pstr("mnt/Проверка") + "  \n"
  		"greek:" + test_pstr("home/user/δοκιμή/δείκτης.index") + "//w:" + test_pstr("home/user/δοκιμή") + "//w:" + test_pstr("mnt/δοκιμή") + "\n"
		"\n"
		"# This should be trimmed    \n"
    	"trim:" + test_pstr("home/user/test.index") + "//w:" + test_pstr("home/user/test") + "//r:" + test_pstr("mnt/test") + "      ";

	config cfg;

	const auto config_path = test_dir / "config" / "config.txt";

	errors err(data_location {.source = "config.txt", .line = 0 });
	std::stringstream f;
	REQUIRE( f << content );

	cfg.load(err, f);

	const std::vector<config_sync>& actual_syncs = cfg.syncs();
	const std::vector<config_sync> expected_syncs = {
		config_sync {
			.name = "home", 
			.index = test_path("home/user/.config/leafsync/leafsync.index"),
			.left = config_mirror { .mode = config_mode::WRITE, .path = test_path("home/user") },
			.right = config_mirror { .mode = config_mode::WRITE, .path = test_path("mnt/sync") }
		},
		config_sync {
			.name = "pic", 
			.index = test_path("home/user/.config/leafsync/pic.index"),
			.left = config_mirror { .mode = config_mode::READ, .path = test_path("home/user/Pictures") },
			.right = config_mirror { .mode = config_mode::WRITE, .path = test_path("mnt/images") }
		},
		config_sync {
			.name = "rus", 
			.index = test_path("home/user/тест/индекс.index"),
			.left = config_mirror { .mode = config_mode::WRITE, .path = test_path("home/user/тест") },
			.right = config_mirror { .mode = config_mode::WRITE, .path = test_path("mnt/Проверка") }
		},
		config_sync {
			.name = "greek", 
			.index = test_path("home/user/δοκιμή/δείκτης.index"),
			.left = config_mirror { .mode = config_mode::WRITE, .path = test_path("home/user/δοκιμή") },
			.right = config_mirror { .mode = config_mode::WRITE, .path = test_path("mnt/δοκιμή") }
		},
		config_sync {
			.name = "trim", 
			.index = test_path("home/user/test.index"),
			.left = config_mirror { .mode = config_mode::WRITE, .path = test_path("home/user/test") },
			.right = config_mirror { .mode = config_mode::READ, .path = test_path("mnt/test") }
		}
	};

	cmp_syncs(expected_syncs, actual_syncs);

	REQUIRE( cfg.find("abc") == nullptr );
	REQUIRE( cfg.find("pic") == &actual_syncs[1] );

}

bool test_has_errors(const std::string& text) {
	test_errors err({ .source = "test" });

	std::stringstream ss(text);
	config().load(err, ss);
	
	UNSCOPED_INFO("text: " << text);
	for (const auto& m: err.messages) {
		UNSCOPED_INFO("error: " << m);
	}

	return err.has_errors();
}


TEST_CASE("errors", "[config]") {
	
	const std::string p1 = (test_dir / "a").string();
	const std::string p2 = (test_dir / "b").string();

	// valid case 
	CHECK_FALSE( test_has_errors("a:b//r:" + p1 + "//w:" + p2) );
	
	// empty component
	CHECK( test_has_errors(":b//r:" + p1 + "//w:" + p2) );
	CHECK( test_has_errors("a://r:" + p1 + "//w:" + p2) );
	CHECK( test_has_errors("a:b//:" + p1 + "//w:" + p2) );
	CHECK( test_has_errors("a:b//r://w:" + p2) );
	CHECK( test_has_errors("a:b//r:" + p1 + "//:" + p2) );
	CHECK( test_has_errors("a:b//r:" + p1 + "//w:") );
	
	// illegal mode
	CHECK( test_has_errors("a:b//x:" + p1 + "//w:" + p2) );
	CHECK( test_has_errors("a:b//r:" + p1 + "//z:" + p2) );

	// non absolute paths
	CHECK( test_has_errors("a:b//r:c//w:" + p2) );
	CHECK( test_has_errors("a:b//r:" + p1 + "//w:d") );

}
