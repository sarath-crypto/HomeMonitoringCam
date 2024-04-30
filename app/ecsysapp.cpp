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
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/wait.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <vector>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "motiondetector.hpp"
#include "miniaudio.h"

#define DIR_MAX_NUM     12
#define FRAME_SZ        0x8000
#define MAX_FERR        255
#define EVENT_DEVICE    "/dev/input/event1"
#define BEACON_ALIVE    10

//#define DEBUG         1

#define MINIAUDIO_IMPLEMENTATION

namespace fs = std::filesystem;
using namespace std;
using namespace cv;
using namespace std::chrono;

bool exit_main = false;
bool exit_imgproc = false;
bool exit_mouseproc = false;

typedef struct frames{
        bool wr;
        unsigned char data[FRAME_SZ];
        unsigned short len;
}frames;

queue <frames> fq;

enum    wav_type{BLIP =1,RING};

void  sighandler(int num){
        exit_imgproc = true;
        exit_mouseproc = true;
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

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount){
        ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
        if (pDecoder == NULL) {
                return;
        }
        ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
        (void)pInput;
}

void play_wav(unsigned char type){
        ma_result result;
        ma_decoder decoder;
        ma_device_config deviceConfig;
        ma_device device;
        system("amixer set PCM 100%");

        string fn;
        unsigned char dly = 1;
        switch(type){
                case(BLIP):{
                        fn = string("/home/ecsys/wav/blip.wav");
                        break;
                }
                case(RING):{
                        fn = string("/home/ecsys/wav/ring.wav");
                        dly = 10;
                        break;
                }
        }
        result = ma_decoder_init_file(fn.c_str(), NULL, &decoder);
        if (result != MA_SUCCESS) {
                syslog(LOG_INFO,"ecsysapp mouseproc unable to load wav file");
                return;
        }

        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format   = decoder.outputFormat;
        deviceConfig.playback.channels = decoder.outputChannels;
        deviceConfig.sampleRate        = decoder.outputSampleRate;
        deviceConfig.dataCallback      = data_callback;
        deviceConfig.pUserData         = &decoder;

        if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
                syslog(LOG_INFO,"ecsysapp mouseproc unable to open playback device");
                ma_decoder_uninit(&decoder);
                return;
        }
        if (ma_device_start(&device) != MA_SUCCESS) {
                syslog(LOG_INFO,"ecsysapp mouseproc unable to start playback device");
                return;
        }
        sleep(dly);
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
}


void *mouseproc(void *p){
        int fd = -1;
        struct input_event ev;
        char name[256] = "Unknown";
        fd_set readfds;
        fd = open(EVENT_DEVICE, O_RDONLY | O_NONBLOCK);
        if (fd == -1) {
                syslog(LOG_INFO,"ecsysapp proc bluetooth mouse %s is not a vaild device",EVENT_DEVICE);
                exit_imgproc = true;
                exit_mouseproc = true;
                exit_main = true;
        }
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        syslog(LOG_INFO,"ecsysapp mouseproc bluetooth mouse %s is %s",EVENT_DEVICE,name);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        unsigned int s = time(NULL);
        unsigned int e = s;
        unsigned int d = 0;


        while(!exit_mouseproc){
                e = time(NULL);
                if(s != e)d = e-s;
                if(d >= BEACON_ALIVE){
                        d = 0;
                        s = e;
                        play_wav(BLIP);
                }

                FD_ZERO(&readfds);
                FD_SET(fd,&readfds);
                const size_t ev_size = sizeof(struct input_event);

                int ret = select(fd + 1, &readfds, NULL, NULL, &tv);
                if(ret == -1){
                        syslog(LOG_INFO,"ecsysapp mouseproc bluetooth select failed");
                        close(fd);
                        exit_imgproc = true;
                        exit_mouseproc = true;
                        exit_main = true;
                }else if (ret == 0){
                        continue;
                }else if(read(fd, &ev, ev_size) < ev_size){
                        syslog(LOG_INFO,"ecsysapp mouseproc bluetooth size failed");
                        close(fd);
                        exit_mouseproc = true;
                        exit_imgproc = true;
                        exit_main = true;
                }else if((ev.type == 2) || (ev.type == 1)){
                        switch(ev.code){
                                case(8):
                                case(11):
                                case(272):
                                case(273):
                                case(274):{
                                        syslog(LOG_INFO,"ecsysapp mouseproc trigger");
                                        play_wav(RING);
                                        while(read(fd, &ev, ev_size) > 0);
                                        break;
                                }
                                default:{
                                        break;
                                }
                        }
                }
        }
        syslog(LOG_INFO,"ecsysapp mouseproc stopped");
        return NULL;
}


void *imgproc(void *p){
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement *prep_stmt;
        sql::Statement *stmt;
        sql::ResultSet  *res;

        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "userecsys", "ecsys123");
        con->setSchema("ecsys");

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

        syslog(LOG_INFO,"ecsysapp imgproc started");
        while(!exit_imgproc){
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
                sched_yield();
        }
        delete con;
        syslog(LOG_INFO,"ecsysapp imgproc stopped");
        return NULL;
}

int main(){
#ifndef DEBUG
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
#endif

        signal(SIGINT,sighandler);

        openlog("ecsysapp",LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
        syslog (LOG_NOTICE, "ecsysapp started by uid %d", getuid ());

        Mat frame;
        lccv::PiCamera cam;
        cam.options->video_width = 1024;
        cam.options->video_height = 768;
        cam.options->framerate = 2;
        cam.options->verbose = false;
        cam.startVideo();
        unsigned char ferror = 0;

        pthread_t th_imgproc_id;
        pthread_t th_mouseproc_id;
        pthread_create(&th_imgproc_id,NULL,imgproc,NULL);
        pthread_create(&th_mouseproc_id,NULL,mouseproc,NULL);

        MotionDetector detector(1,0.2,20,0.1,5,10,2);

        syslog(LOG_INFO,"ecsysapp started");
        while(!exit_main){
                if(!cam.getVideoFrame(frame,1000)){
                        syslog(LOG_INFO,"ecsysapp ferror");
                        ferror++;
                        continue;
                }else{
                        ferror = 0;
                        std::list<cv::Rect2d>boxes;
                        boxes = detector.detect(frame);
                        for(auto i = boxes.begin(); i != boxes.end(); ++i)rectangle(frame,*i,Scalar(0,0,255));
                        resize(frame,frame,Size(640,480),INTER_LINEAR);

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
                        exit_mouseproc = true;
                        exit_imgproc = true;
                        exit_main = true;
                }
        }
        cam.stopVideo();
        pthread_join(th_imgproc_id,NULL);
        pthread_join(th_mouseproc_id,NULL);
        syslog(LOG_INFO,"ecsysapp stopped");
        closelog();
        return 0;
}
