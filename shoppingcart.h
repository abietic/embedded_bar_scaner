#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include <QtGui>
#include <QMutex>
#include "defdbg.h"

class Dialog;
class VirtualKeyBoard;
class DBSqlite;
class Button;
struct ModelRow
{
    QString name;
    int count;
    float price;
    QString barcode;
    int row;
};
class ShoppingCart : public QWidget
{
    Q_OBJECT
public:
    ShoppingCart(DBSqlite* aDb,QMutex& lock,QWidget *parent = 0);
    ~ShoppingCart();
    void setSourceModel(QAbstractItemModel *model);
public slots:
    void addProductViaVKBD();
    void clearVKBD();
    void doSettlement();
    void addProductViaTTY(QString barcode);
private:
    void addRow(const QString& name, const int count, const float price, const QString& barcode);
    ModelRow findRow(const QString& barcode);
    void addProduct(const QString& barcode);
    void initModel();
    Dialog *father;
    QSortFilterProxyModel *proxyModel;
    QStandardItemModel *model;
    QTreeView *proxyView;
    VirtualKeyBoard *vkbd;
    DBSqlite *db;
    QMutex &mutex;
    Button *addProductButton,*clearVKBDButton,*settlementButton;
};

#endif // SHOPPINGCART_H
