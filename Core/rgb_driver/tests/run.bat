cmake -B build -G "MinGW Makefiles"
make -C build
start /d build /B unit_test.exe
