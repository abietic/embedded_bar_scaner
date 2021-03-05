#ifndef INFOMANIPULATE_H
#define INFOMANIPULATE_H

#include <QtGui>
#include <QMutex>

class DBSqlite;
class VirtualKeyBoard;
class Dialog;
class Button;
class InfoManipulate : public QWidget
{
    Q_OBJECT
public:
    InfoManipulate(DBSqlite *aDb,QMutex& lock,QWidget *parent = 0);
    void setSourceModel(QAbstractItemModel *model);
public slots:
    void startAdd();
    void startDelete();
    void readName();
    void readBarcodeViaVKBD();
    void readBarcodeViaTTY(QString barcode);
    void readPrice();
    void giveup();
private:
    void initButtons();
    void initModel();
    void addRow(const QString& name,const float price, const QString& barcode);
    enum {initState = 0,
          insertName = 1,
          insertBarcode = 2,
          insertPrice = 3,
          delState = 4} curState;
    Dialog *father;
    QSortFilterProxyModel *proxyModel;
    QStandardItemModel *model;
    QTreeView *proxyView;
    VirtualKeyBoard *vkbd;
    QMutex& mutex;
    DBSqlite *db;
    QLabel *instructLabel;
    Button *startAddButton,
    *startDeleteButton,
    *readNameButton,
    *readBarcodeButton,
    *readPriceButton,
    *giveupButton;
};

#endif // INFOMANIPULATE_H
