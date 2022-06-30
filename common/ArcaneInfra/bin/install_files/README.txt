For tesing your application with this installation
1- in CMakeLists.txt check if PROJECT_BIN defines your main executable with its options (default is the automatic Shell script wrapper)
   WARNING: for Windows executable you must use .exe extension (if not, you will obtain a silent error which do nothing)
   WARNING: for Windows environment, use / not \ as directory separator if used with test_architecture() default configuration
   ex: set(PROJECT_BIN C:/Users/havep/Desktop/ArcTem/install/ArcTemLauncher.exe --mpirun-options=--delegate)
2- in CMakeLists.txt you can bypass default test_architecture() call to define your own BUILD_TEST_COMMAND
   NB: default BUILD_TEST_COMMAND defined in test_architecture() uses .sh script and requires cygwin for Windows environment.
3- copy or link into 'data' sub-directory your test directory (usually a copy of the one at your application root)
   ex: cp APPLI_ROOT/test install/testing/data
4- go into 'test' sub-directory. This directory will contain all generated configuration and test results. 
   You can destroy it to clean up.
5- configure your test process with
     ../bin/cmake ..
6- run your tests with 
     ../bin/ctest
