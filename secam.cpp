#include <iostream>
#include <lccv.hpp>
#include <opencv2/opencv.hpp>
#include <syslog.h>
#include <queue>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <sys/types.h>
#include <set>
#include <iomanip>
#include <bits/stdc++.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "motiondetector.hpp"

#define DIR_MAX_NUM	14
#define FRAME_SZ	0x8000
#define MAX_FERR	16

namespace fs = std::filesystem;
using namespace std;
using namespace cv;

bool exit_main = false;
bool exit_proc = false;

typedef struct frames{
	bool wr;
	unsigned char data[FRAME_SZ];
	unsigned short len;
}frames;

queue <frames> fq;

void  sighandler(int num){
	exit_proc = true;
	exit_main = true;
}

void sort(map<unsigned int,string>& M){ 
	multimap<string,unsigned int> MM;
    	for (auto& it : M) {
        	MM.insert({ it.second, it.first });
    	}
} 

void gettimestamp(string &fn){
	time_t t;
	struct tm *ptm;
	time(&t);
	ptm = localtime(&t);
	char ts[24];
	strftime(ts,24,"%y%m%d%H%M%S",ptm);

	struct timeval tv;
	gettimeofday(&tv,NULL);
	unsigned short ms = tv.tv_usec/1000;
	
	fn.append(ts,4,2);
	fn += "/";
	fn = fn.append(ts,strlen(ts));
	fn = fn+ "m" + to_string(ms) +".";	
}

void *proc(void *p){
  	sql::Driver *driver;
  	sql::Connection *con;
	sql::PreparedStatement *prep_stmt;
	sql::Statement *stmt;
	sql::ResultSet  *res;


 	driver = get_driver_instance();
  	con = driver->connect("tcp://127.0.0.1:3306", "usersecam", "secam123");
  	con->setSchema("secam");

	stmt = con->createStatement();
	res = stmt->executeQuery("select ts from img");
	string tsd;
	if(res->next())tsd = string(res->getString("ts"));
	delete stmt;
	delete res;

	string fn = "/var/www/html/data/";
	gettimestamp(fn);
	fn = fn+"tmr";	

	fs::path fp = fn;	
	if(!fs::is_directory(fp.parent_path()))fs::create_directory(fp.parent_path());
	int fd = open (fn.c_str(),O_CREAT|O_WRONLY,0006);
	write(fd,tsd.data(),tsd.length());
	close(fd);

	syslog(LOG_INFO,"secapp proc started");
	while(!exit_proc){
		if(!fq.empty()){
			frames f = fq.front();

			prep_stmt = con->prepareStatement("update img set data=?");
			struct membuf : std::streambuf {
				membuf(char* base, std::size_t n) {
					this->setg(base, base, base + n);
				}
			};
			membuf mbuf((char*)f.data,f.len);
			std::istream blob(&mbuf);
			prep_stmt->setBlob(1,&blob);
			prep_stmt->executeUpdate();
			delete prep_stmt;

			if(f.wr){
				fn = "/var/www/html/data/";
				map<unsigned int,string>sname;
				for (const auto & p : fs::directory_iterator(fn)){
					struct stat attrib;  
					stat(p.path().string().c_str(), &attrib); 
					unsigned int ts = mktime(gmtime(&attrib.st_mtime));
				        string s = p.path().string();
					sname[ts] = s;
				}
				if(sname.size() > DIR_MAX_NUM){
					sort(sname);
					fs::remove_all(sname.begin()->second);
				}
				sname.clear();
			
				gettimestamp(fn);
				fn = fn+"jpg";	

				fs::path fp = fn;	
				if(!fs::is_directory(fp.parent_path()))fs::create_directory(fp.parent_path());
				int fd = open (fn.c_str(),O_CREAT|O_WRONLY,0006);
				write(fd,&f.data,f.len);
				close(fd);
			}
			fq.pop();	

		}
	}
	delete con;
	syslog(LOG_INFO,"secapp proc stopped");
	return NULL;
}

int main(){
	pid_t process_id = 0;
        pid_t sid = 0;
        int pid = 0;

        process_id = fork();
        if(process_id < 0)exit(1);
        if (process_id > 0)exit(0);
        pid = getpid();

        umask(0);
        sid = setsid();
        if(sid < 0)exit(1);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

	signal(SIGINT,sighandler);

        openlog("secapp",LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
        syslog (LOG_NOTICE, "secapp started by uid %d pid %d", getuid (),pid);

	Mat frame;
	lccv::PiCamera cam;
	cam.options->video_width = 640;
	cam.options->video_height = 480;
	cam.options->framerate = 2;
	cam.options->verbose = false;
	cam.startVideo();
	unsigned char ferror = 0;

	pthread_t th_id;
	pthread_create(&th_id,NULL,proc,NULL);

	MotionDetector detector(1,0.2,20,0.1,5,10,2);

	syslog(LOG_INFO,"secapp started");
	while(!exit_main){
		if(!cam.getVideoFrame(frame,1000)){
			syslog(LOG_INFO,"secapp ferror");
			ferror++;
			continue;
		}else{
			ferror = 0;
			std::list<cv::Rect2d>boxes;
      			boxes = detector.detect(frame);
        		for(auto i = boxes.begin(); i != boxes.end(); ++i)rectangle(frame,*i,Scalar(0,0,255));

			time_t t;
			struct tm *ptm;
			time(&t);
			ptm = localtime(&t);
			char ts[16];
			strftime(ts,16,"%H%M%S",ptm);

			Point tp(0,24);
   			int fs = 1;
   			Scalar fc(255,0,0);
   			int fw = 1;
   			putText(frame,ts,tp,FONT_HERSHEY_COMPLEX,fs,fc,fw);

			unsigned char q = 80;
			vector<unsigned char>buf;
			vector<int>param(2);
			param[0] = IMWRITE_JPEG_QUALITY;
			do{
				buf.clear();
				param[1] = q;
				imencode(".jpg",frame,buf,param);
				q--;
			}while(buf.size() > FRAME_SZ);
			frames f;
			memcpy(f.data,buf.data(),buf.size());
			f.len = buf.size();
			if(boxes.size())f.wr = true;
			else f.wr = false;
			fq.push(f);	
		}
		if(ferror >= MAX_FERR){
			exit_proc = true;
			exit_main = true;
		}
	}
	cam.stopVideo();
	pthread_join(th_id,NULL);
	syslog(LOG_INFO,"secapp stopped");
	closelog();
	return 0;
}
