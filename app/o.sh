g++ -c motiondetector.cpp `pkg-config --cflags --libs opencv4` -llccv  -lmysqlcppconn -lsoundio  -Wno-psabi  -ldl -lm -lpthread -latomic
g++ -c scanner.cpp        `pkg-config --cflags --libs opencv4` -llccv  -lmysqlcppconn -lsoundio  -Wno-psabi  -ldl -lm -lpthread -latomic
g++ -c miniaudio.c 	  `pkg-config --cflags --libs opencv4` -llccv  -lmysqlcppconn -lsoundio  -Wno-psabi  -ldl -lm -lpthread -latomic
