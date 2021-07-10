//
//  main.cpp
//  fart-json-test
//
//  Created by Kristian Trenskow on 16/01/2021.
//

#include "fart/fart.hpp"

using namespace fart::io::fs;
using namespace fart::system;
using namespace fart::serialization;

size_t skip = 0;
size_t ran = 0;

Array<String> fullPaths(const String& path, const Array<String>& filenames) {
	return filenames.map<String>([&path](const String& filename, const size_t idx) {
		return path.mapCString<String>([&filename](const char* path) {
			return filename.mapCString<String>([&path](const char* filename) {
				return String::format("%s%s", path, filename);
			});
		});
	});
}

void fail(bool bail = true) {
	printf("Failed\n");
	if (bail) exit(1);
}

void pass() {
	printf("Passed\n");
}

enum class Expectation {
	pass = 0,
	fail,
	indifferent
};

void decide(Expectation expectation, const Exception* result) {
	switch (expectation) {
		case Expectation::pass:
			if (result == nullptr) pass(); else fail();
			break;
		case Expectation::fail:
			if (result != nullptr) pass(); else fail();
			break;
		case Expectation::indifferent:
			if (result == nullptr) pass(); else fail(false);
			break;
	}
}

void test(const String& path, const Array<String>& filenames, Expectation expectation) {
	filenames.forEach([&path,&expectation](const String& filename) {

		ran++;

		filename.substring(path.length()).withCString([](const char* filename) {
			printf("%zu) Testing %s... ", ran, filename);
		});

		if (ran <= skip) {
			printf("Skipped\n");
			return;
		}

		try {
			JSON::parse(String(File::open(filename, File::Mode::asRead)->readToEnd()));
		} catch (DecoderException exception) {
			decide(expectation, &exception);
			return;
		} catch (JSONMalformedException exception) {
			decide(expectation, &exception);
			return;
		}

		decide(expectation, nullptr);

	});
}

int main(int argc, const char * argv[]) {

	if (argc == 1 || String(argv[1]) == "--help") {
		printf("Usage: fart-json-test [test-suite-directory]\n");
		exit(1);
	}

	String path = argv[1];
	if (path.length() > 0 && path[path.length() - 1] != '/') path.append("/");

	auto filenames = File::directoryContent(path);

	auto shouldPass = fullPaths(path, filenames.filter([](const String& filename, const size_t idx) {
		return filename[0] == 'y';
	}));

	auto shouldFail = fullPaths(path, filenames.filter([](const String& filename, const size_t idx) {
		return filename[0] == 'n';
	}));

	auto shouldBeIndifferent = fullPaths(path, filenames.filter([](const String& filename, const size_t idx) {
		return filename[0] == 'i';
	}));

	test(path, shouldPass, Expectation::pass);
	test(path, shouldFail, Expectation::fail);
	test(path, shouldBeIndifferent, Expectation::indifferent);

	printf("All done.\n");

	return 0;
}
