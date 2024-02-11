# HomeMonitoringCam
A simple software to monitor home using Pi3@bookworm and Pi3 camera
The solution needed mysql (mariaDB), c++ connector (https://dev.mysql.com/downloads/connector/cpp/) , apache2 , php >= 8.2.7 , opencv4, lccv(https://github.com/kbarni/LCCV),  motion detection software (https://github.com/bezbahen0/MotionDetection) 
command for complilation is -> g++ secam.cpp motiondetector.o scanner.o `pkg-config --cflags --libs opencv4` -llccv  -lmysqlcppconn -Wno-psabi -o secam
You need to compile motion detecion software seperatley to create .o files (objective files)

cat /etc/os-release
PRETTY_NAME="Raspbian GNU/Linux 12 (bookworm)"
NAME="Raspbian GNU/Linux"
VERSION_ID="12"
VERSION="12 (bookworm)"
VERSION_CODENAME=bookworm
ID=raspbian
ID_LIKE=debian
HOME_URL="http://www.raspbian.org/"
SUPPORT_URL="http://www.raspbian.org/RaspbianForums"
BUG_REPORT_URL="http://www.raspbian.org/RaspbianBugs"

