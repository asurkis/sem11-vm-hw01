run: main.exe
	./main.exe

main.exe: main.cpp
	$(CXX) -O0 main.cpp -o main.exe
