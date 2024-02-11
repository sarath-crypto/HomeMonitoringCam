g++ secam.cpp motiondetector.o scanner.o `pkg-config --cflags --libs opencv4` -llccv  -lmysqlcppconn -Wno-psabi -o secam
