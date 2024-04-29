# Elder Care Solution
A simple software for elder care using Pi3@bookworm. Hardware consists for  Pi3 , Pi3 camera, Rapoo Bluetooth mouse (Single AA battery), and audio bluetooth transmitter, a generic bluetooth speaker.
Solution consists of [Mouse(BT)]~~~~~~~~~~~~~~~~~~>Pi3[Application software][audio out jack]--->[BT audio transmitter]~~~~~~~~~~~~~~~~~~>[Bluetooth portable speaker]. An image (Pi camera) grabber based on motion detection grabs and store the images which can be viewed using web browser in near real time.
The solution needed mysql (mariaDB), c++ connector (https://dev.mysql.com/downloads/connector/cpp/) , apache2 , php >= 8.2.7 , opencv4, lccv(https://github.com/kbarni/LCCV),  motion detection software (https://github.com/bezbahen0/MotionDetection), jpgraph for php. Steps are shown below

sudo apt update
sudo apt upgrade
sudo nmcli con add con-name ethernet ifname eth0 type ethernet ip4 10.10.10.1/24 gw4 10.10.10.1
sudo nmcli con mod ethernet ipv4.dns 10.10.10.1,8.8.8.8
sudo nmcli con up ethernet ifname eth0
raspi-config timezone
sudo apt install libopencv-dev mariadb-server php cmake libcamera-dev libmysqlcppconn-dev vim git  php8.2-mysql evtest syslog-ng php8.2-gd  libasound2-dev

sudo reboot
cd /usr/include/libcamera
sudo cp libcamera/*.* . -r
sudo cp libcamera/ipa/ . -r
sudo cp libcamera/base/ . -r
git clone https://github.com/kbarni/LCCV
mkdir build
cd build
cmake ..
make -j4
sudo make install
git clone https://github.com/sarath-crypto/Elder-Care-Solution

sudo mysql -u root
create database ecsys;
use ecsys;
create table img(data BLOB,ts TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);
create user 'userecsys'@'localhost' identified by 'ecsys123';
grant all on ecsys.* TO 'userecsys'@'localhost';
insert into img(data) values(LOAD_FILE('/home/seccam/backup/dbase/tv640.jpg'));

https://jpgraph.net/download/download.php?p=57
copy /Elder-Care-Solution/jpgraph-4.4.2/src to var/www/html
sudo chown  www-data:www-data -R *
