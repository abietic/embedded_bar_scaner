#include "ttythread.h"
#include "tty.h"
#include <stdio.h>
#define BUF_SZ 40

TTYThread::TTYThread():
        _tty(TTY::tty)
{
}

void TTYThread::run()
{
    int count = 0;
    char buf[BUF_SZ]={0};
    forever
    {
        count = this->_tty.tty_read(buf,BUF_SZ);
        if (count <= 0)
        {
#ifdef k
            fprintf(stderr,"Read tty Failed\n");
#endif
            this->_tty.tty_flush();
            continue;
        }
        this->_tty.tty_flush();
        buf[count] = '\0';
        if (buf[count - 1] == '\n')
            buf[count - 1] = '\0';
        emit getABarcode(QString::fromAscii(buf,count));
    }
}
