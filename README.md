# Elder Care Solution
A simple software for elder care using Pi3@bookworm.Hardware consists for  Pi3 , Pi3 camera, Rapoo Bluetooth mouse (AA battery), A generic bluetooth speaker.
Solution consists of Mouse(BT)----->Pi3-------->BT Speaker. Setting up of bluetooth speaker with Pi3 is mentioned in https://www.okdo.com/project/set-up-a-bluetooth-speaker-with-a-raspberry-pi/.

The solution needed mysql (mariaDB), c++ connector (https://dev.mysql.com/downloads/connector/cpp/) , apache2 , php >= 8.2.7 , opencv4.8, lccv(https://github.com/kbarni/LCCV),  motion detection software (https://github.com/bezbahen0/MotionDetection), jpgraph for php
command for complilation is -> g++ secam.cpp motiondetector.o scanner.o `pkg-config --cflags --libs opencv4` -llccv  -lmysqlcppconn -Wno-psabi -o secam
