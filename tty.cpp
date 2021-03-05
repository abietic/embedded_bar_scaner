#include "tty.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#define COM "/dev/ttySe0"
#define COM "/dev/ttySAC1"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#if 0
#define close(FD) fclose((FILE *)FD)
#define read(FD,BUF,NBYTES) fread(BUF,sizeof(char),NBYTES,(FILE *)FD)
#define write(FD,BUF,NBYTES) fwrite(BUF,sizeof(char),NBYTES,(FILE *)FD)
#include <time.h>
#define usleep(ARG) struct timespec t = {0,ARG * 1000}; nanosleep(&t,NULL)
#endif

TTY TTY::tty;

extern int baud;
TTY::TTY()
{
    init();
#ifdef DEBUG
    printf("tty opend!\n");
#endif
}

TTY::~TTY()
{
    tcsetattr(fd,TCSANOW,&oldtio);	 	/* restore old modem setings */
//  	tcsetattr(0,TCSANOW,&oldstdtio); 	/* restore old tty setings */
        close(fd);
#ifdef DEBUG
        printf("tty closed!\n");
#endif
}

void TTY::tty_flush()
{
    tcflush(fd,TCIFLUSH);
}

int TTY::tty_read(char *buf, int nbytes)
{
    return read(fd,buf,nbytes);
}

int TTY::tty_write(char *buf, int nbytes)
{
    int i;
        for(i=0; i<nbytes; i++) {
                write(fd,&buf[i],1);
                usleep(100);
        }
        return tcdrain(fd);
}

int TTY::tty_writecmd(char *buf, int nbytes)
{
    int i;
        for(i=0; i<nbytes; i++) {
                write(fd,&buf[i],1);
                usleep(100);
        }
        write(fd,"\r",1);
        usleep(300000);
        return tcdrain(fd);
}

int TTY::init()
{
    fd = open(COM, O_RDWR ); //| O_NONBLOCK);//
        if (fd <0) {
                perror(COM);
                exit(-1);
        }

        tcgetattr(fd,&oldtio); /* save current modem settings */
        bzero(&newtio, sizeof(newtio));

        newtio.c_cflag = B57600 | /*CRTSCTS |*/ CS8 /*| CLOCAL | CREAD */;
        newtio.c_iflag = IGNPAR | ICRNL;
        newtio.c_oflag = 0;
        newtio.c_lflag = ICANON;

         newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
         newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
         newtio.c_cc[VERASE]   = 0;     /* del */
         newtio.c_cc[VKILL]    = 0;     /* @ */
         newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
         newtio.c_cc[VTIME]    = 0;     /* ʹ�ָ���ļ�� */
         newtio.c_cc[VMIN]     = 1;     /* �� 1 ��ǰ�ֹͣ */
         newtio.c_cc[VSWTC]    = 0;     /* '\0' */
         newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
         newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
         newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
         newtio.c_cc[VEOL]     = 0;     /* '\0' */
         newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
         newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
         newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
         newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
         newtio.c_cc[VEOL2]    = 0;     /* '\0' */


        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);/*set attrib	  */

/* now clean the modem line and activate the settings for modem */
// 	tcflush(fd, TCIFLUSH);
//	tcsetattr(fd,TCSANOW,&newtio);/*set attrib	  */
//	signal(SIGTERM,do_exit);
//	signal(SIGQUIT,do_exit);
//	signal(SIGINT,do_exit);
        return 0;
}
