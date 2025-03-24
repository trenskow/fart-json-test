# foundation-json-test

JSON test suite for [foundation](https://github.com/trenskow/foundation) – uses the [nst/JSONTestSuite](https://github.com/nst/JSONTestSuite) for it's tests.

## Run

````bash
cmake .
cmake --build .
./build/Release/[arch]/bin/foundation-json-test tests/test_parsing
echo $? # prints `1` if failed and `0` if succeeded.
````

> Replace `[arch]` with machine architecture (eg. `arm64` or `x86_64`).

## License

See LICENSE for license.
