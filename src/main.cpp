//
//  main.cpp
//  fart-json-test
//
//  Created by Kristian Trenskow on 16/01/2021.
//

#include "../foundation/src/foundation.hpp"

using namespace foundation::system;
using namespace foundation::serialization;
using namespace foundation::io::fs;
using namespace foundation::exceptions::serialization;

const size_t skip = 0;
const bool bail = false;
size_t ran = 0;

Array<String> fullPaths(const String& path, const Array<String>& filenames) {
	return filenames.map<String>([&path](const String& filename) {
		return path.mapCString<String>([&filename](const char* path) {
			return filename.mapCString<String>([&path](const char* filename) {
				return String::format("%s%s", path, filename);
			});
		});
	});
}

bool fail(const String& filename) {
	filename.withCString([](const char* filename) {
		printf("Failed: %s\n", filename);
	});
	if (bail) exit(1);
	return true;
}

enum class Expectation {
	pass = 0,
	fail,
	indifferent
};

bool decide(const String& filename, Expectation expectation, const Exception* result) {

	switch (expectation) {
		case Expectation::pass:
			if (result != nullptr) return fail(filename);
			break;
		case Expectation::fail:
			if (result == nullptr) return fail(filename);
			break;
		case Expectation::indifferent:
			break;
	}

	return false;

}

bool test(const String& path, const Array<String>& filenames, Expectation expectation) {

	return filenames.reduce<bool>(false, [&path,&expectation](bool result, const String& fullPath) {

		ran++;

		if (ran <= skip) {
			printf("Skipped\n");
			return result;
		}

		String filename = fullPath.substring(path.length());

		try {
			JSON().parse(String(File::open(fullPath, File::Mode::asRead)->readToEnd(), false));
		} catch (DecoderException exception) {
			return result || decide(filename, expectation, &exception);
		} catch (JSONMalformedException exception) {
			return result || decide(filename, expectation, &exception);
		}

		return result || decide(filename, expectation, nullptr);

	});

}

int main(int argc, const char * argv[]) {

	if (argc < 2 || String(argv[1]).equals("--help")) {
		printf("Usage: fart-json-test [test-suite-directory]\n");
		exit(1);
	}

	String path = argv[1];
	if (path.length() > 0 && path[path.length() - 1] != '/') path.append("/");

	Array<String> filenames;

	try {
		filenames = File::directoryContent(path);
	} catch (Exception& e) {
		printf("Test suites not found at directory.\n");
		exit(1);
	}

	auto shouldPass = fullPaths(path, filenames.filter([](const String& filename) {
		return filename[0] == 'y';
	}));

	auto shouldFail = fullPaths(path, filenames.filter([](const String& filename) {
		return filename[0] == 'n';
	}));

	auto shouldBeIndifferent = fullPaths(path, filenames.filter([](const String& filename) {
		return filename[0] == 'i';
	}));

	bool result = false;

	result = result || test(path, shouldPass, Expectation::pass);
	result = result || test(path, shouldFail, Expectation::fail);
	result = result || test(path, shouldBeIndifferent, Expectation::indifferent);

	return result ? 1 : 0;

}
