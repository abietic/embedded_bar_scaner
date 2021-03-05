#include "shoppingcart.h"
#include <QtGui>
#include "virtualkeyboard.h"
#include "dialog.h"
#include "dbsqlite.h"
#include "button.h"
ShoppingCart::ShoppingCart(DBSqlite *aDb,QMutex& lock, QWidget *parent):
        QWidget(parent),
        father(dynamic_cast<Dialog*>(parent)),
        proxyModel(new QSortFilterProxyModel),
        proxyView(new QTreeView),
        model(new QStandardItemModel(0,4,this)),
        db(aDb),
        mutex(lock),
        clearVKBDButton(new Button("ClearVKBD",QColor(150, 205, 205),this)),
        addProductButton(new Button("AddProduct",QColor(150, 205, 205),this)),
        settlementButton(new Button("Settlement",QColor(150, 205, 205),this)),
        vkbd(new VirtualKeyBoard(this))
{


    this->proxyModel->setDynamicSortFilter(true);

    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setModel(this->proxyModel);
    proxyView->setSortingEnabled(true);
    this->initModel();
    for (int i = 0; i < this->model->columnCount(); ++i)
    {
        this->proxyView->setColumnWidth(i,60);
    }

    this->connect(this->addProductButton,SIGNAL(clicked()),this,SLOT(addProductViaVKBD()));
    this->connect(this->clearVKBDButton,SIGNAL(clicked()),this,SLOT(clearVKBD()));
    this->connect(this->settlementButton,SIGNAL(clicked()),this,SLOT(doSettlement()));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QVBoxLayout *leftSideLayout = new QVBoxLayout;
    QVBoxLayout *rightSideLayout = new QVBoxLayout;
    buttonLayout->addWidget(this->clearVKBDButton);
    buttonLayout->addWidget(this->settlementButton);
    buttonLayout->addWidget(this->addProductButton);
    leftSideLayout->addWidget(this->proxyView);
    leftSideLayout->addLayout(buttonLayout);
    rightSideLayout->addWidget(this->vkbd);
    mainLayout->addLayout(leftSideLayout);
    mainLayout->addLayout(rightSideLayout);
    this->setLayout(mainLayout);
}

ShoppingCart::~ShoppingCart()
{
}

void ShoppingCart::initModel()
{
    this->model->setHeaderData(0,Qt::Horizontal,QObject::tr("Name"));
    this->model->setHeaderData(1,Qt::Horizontal,QObject::tr("Count"));
    this->model->setHeaderData(2,Qt::Horizontal,QObject::tr("Total Price"));
    this->model->setHeaderData(3,Qt::Horizontal,QObject::tr("Barcode"));
    this->setSourceModel(this->model);
//    DBSqliteStmt query(this->db->prepare("select * from products"));
//    while (query.next())
//    {
//        this->addRow(query.getString(1),1,query.getDouble(2),query.getString(0));
//    }
//    query.close();
}


void ShoppingCart::addRow(const QString& name, const int count, const float price, const QString& barcode)
{

    if (this->model->findItems(barcode,Qt::MatchExactly,3).empty())
    {
        this->model->insertRow(0);
        this->model->setData(this->model->index(0, 0), name);
        this->model->setData(this->model->index(0, 1), count);
        this->model->setData(this->model->index(0, 2), price);
        this->model->setData(this->model->index(0, 3), barcode);
    }
    else
    {
#ifdef DEBUG
        int aRow = this->model->findItems(barcode,Qt::MatchExactly,3).front()->row();
        fprintf(stderr,"The row number is : %d\n", aRow);
#endif
    }
}

 void ShoppingCart::setSourceModel(QAbstractItemModel *model)
 {
     proxyModel->setSourceModel(model);
 }

 void ShoppingCart::addProductViaVKBD()
 {
     this->mutex.lock();
     QString barcode(this->vkbd->getText());
     this->addProduct(barcode);
//     DBSqliteStmt query(this->db->prepare("select * from products where barcode = ?"));
//     query.bindString(0,barcode);
//     if (!query.exec())
//     {
//         QMessageBox::warning(this,"Op Error","Op Select By Barcode Failed");
//     }
//     else
//     {
//         if (!query.next())
//         {
//             QMessageBox::warning(this,"Row Not Found","Barcode "+barcode+" doesn't exist in db");
//         }
//         else
//         {
//             ModelRow r = this->findRow(barcode);
//             if (r.count == 0)//hasn't existed in the model
//             {
//                 this->addRow(query.getString(1),1,query.getDouble(2),query.getString(0));
//             }
//             else             //already existed in the model
//             {
//                 this->model->removeRow(r.row);
//                 this->addRow(r.name,r.count + 1, r.price + query.getDouble(2),r.barcode);
//             }
//         }
//     }
//     query.close();
     this->mutex.unlock();
 }

 ModelRow ShoppingCart::findRow(const QString& barcode)
 {
     ModelRow res;
     QList<QStandardItem*> items = this->model->findItems(barcode,Qt::MatchExactly,3);
     if (items.isEmpty())
     {
         res.count = 0;
     }
     else
     {
         int row = items.front()->row();
         res.count = this->model->itemData(this->model->index(row,1))[0].toInt();
         res.name = this->model->itemData(this->model->index(row,0))[0].toString();
         res.price = this->model->itemData(this->model->index(row,2))[0].toDouble();
         res.barcode = this->model->itemData(this->model->index(row,3))[0].toString();
         res.row = row;
     }
     return res;
 }

 void ShoppingCart::clearVKBD()
 {
     this->vkbd->clearText();
 }

 void ShoppingCart::doSettlement()
 {
     this->mutex.lock();
     QString inMoney = this->vkbd->getText();
     int rows = this->model->rowCount();
     double total = 0;
     for (int i = 0; i < rows; ++i)
     {
         total += this->model->itemData(this->model->index(i, 2))[0].toDouble();
     }
     QMessageBox::warning(this,"Total Price", "The total price is : "+QString::number(total,'f',1));
     double customerPays = inMoney.toDouble();
     if (customerPays < total)
     {
         QMessageBox::warning(this,"Not Enough Money","The customer pays : "+inMoney+"\n"+"The total cost is : "+QString::number(total,'f',1)+"\n");
     }
     else
     {
         QMessageBox::warning(this, "Settlement Success","You should Change : "+QString::number(customerPays - total,'f',1));
         //this->vkbd->displayText(QString::number(customerPays - total,'f',1));
         this->model->removeRows(0,rows);
     }
     this->mutex.unlock();
 }

 void ShoppingCart::addProductViaTTY(QString barcode)
 {
     this->mutex.lock();
     this->addProduct(barcode);
     this->mutex.unlock();
 }

 void ShoppingCart::addProduct(const QString& barcode)
 {
     DBSqliteStmt query(this->db->prepare("select * from products where barcode = ?"));
     query.bindString(0,barcode);
     if (!query.exec())
     {
         QMessageBox::warning(this,"Op Error","Op Select By Barcode Failed");
     }
     else
     {
         if (!query.next())
         {
             QMessageBox::warning(this,"Row Not Found","Barcode "+barcode+" doesn't exist in db");
         }
         else
         {
             ModelRow r = this->findRow(barcode);
             if (r.count == 0)//hasn't existed in the model
             {
                 this->addRow(query.getString(1),1,query.getDouble(2),query.getString(0));
             }
             else             //already existed in the model
             {
                 this->model->removeRow(r.row);
                 this->addRow(r.name,r.count + 1, r.price + query.getDouble(2),r.barcode);
             }
         }
     }
     query.close();
 }
