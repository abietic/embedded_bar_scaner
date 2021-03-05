#ifndef TTY_H
#define TTY_H


#include <termios.h>
#include "defdbg.h"


class TTY
{
public:
    static TTY tty;
    TTY();
    ~TTY();
    int tty_read(char *buf,int nbytes);
    int tty_write(char *buf,int nbytes);
    int tty_writecmd(char *buf,int nbytes);
    void tty_flush();
private:
    int init();
private:
    int fd;
    struct termios oldtio,newtio;
};

#endif // TTY_H
