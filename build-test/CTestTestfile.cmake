# CMake generated Testfile for 
# Source directory: /home/macally/cryptofile
# Build directory: /home/macally/cryptofile/build-test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[core]=] "/home/macally/cryptofile/build-test/test_core")
set_tests_properties([=[core]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/macally/cryptofile/CMakeLists.txt;29;add_test;/home/macally/cryptofile/CMakeLists.txt;0;")
add_test([=[cli_version]=] "/home/macally/cryptofile/build-test/cryptofile" "--version")
set_tests_properties([=[cli_version]=] PROPERTIES  PASS_REGULAR_EXPRESSION "0.1.0" _BACKTRACE_TRIPLES "/home/macally/cryptofile/CMakeLists.txt;32;add_test;/home/macally/cryptofile/CMakeLists.txt;0;")
add_test([=[cli_bad_args]=] "/home/macally/cryptofile/build-test/cryptofile" "--no-such-option")
set_tests_properties([=[cli_bad_args]=] PROPERTIES  WILL_FAIL "TRUE" _BACKTRACE_TRIPLES "/home/macally/cryptofile/CMakeLists.txt;36;add_test;/home/macally/cryptofile/CMakeLists.txt;0;")
