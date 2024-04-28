g++ ecsysapp.cpp motiondetector.o scanner.o miniaudio.o `pkg-config --cflags --libs opencv4` -llccv  -lmysqlcppconn  -Wno-psabi  -ldl -lm -lpthread -latomic -o ecsysapp
