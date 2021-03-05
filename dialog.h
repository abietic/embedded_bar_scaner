#ifndef DIALOG_H
#define DIALOG_H

#include <QtGui>
//#include "keyboard.h"
#include <QMutex>

class TTYThread;
class DBSqlite;
class ShoppingCart;
class InfoManipulate;
class Dialog : public QDialog
{
    Q_OBJECT

public:
    static QMutex mutex;
    Dialog(QWidget *parent = 0);
    ~Dialog();
private slots:
    void switchFunction(int index);
private:
    void initDB();
    QTabWidget *tabWidget;
    ShoppingCart *_shoppingCart;
    InfoManipulate *_infoManipulate;
    DBSqlite *db;
    TTYThread *_ttyThread;
    int curIndex;
};

#endif // DIALOG_H
