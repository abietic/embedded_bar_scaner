#ifndef TTYTHREAD_H
#define TTYTHREAD_H

#include <QThread>
#include "defdbg.h"
class TTY;
class TTYThread : public QThread
{
    Q_OBJECT
public:
    signals:
    void getABarcode(QString barcode);
protected:
    void run();
public:
    TTYThread();
private:
    TTY& _tty;
};

#endif // TTYTHREAD_H
