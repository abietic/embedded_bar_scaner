#include "dialog.h"
#include "shoppingcart.h"
#include "infomanipulate.h"
#include "dbsqlite.h"
#include "ttythread.h"

#define CREATE_INFO_TABLE QString("create table if not exists products(barcode varchar(200) primary key, name varchar(200) not null, price float not null default 0.0)")


QMutex Dialog::mutex;

Dialog::Dialog(QWidget *parent)
    : QDialog(parent),
    tabWidget(new QTabWidget(this)),
    db(new DBSqlite("products.db")),
    _ttyThread(new TTYThread),
    curIndex(0)
{
    this->initDB();
    this->_shoppingCart = new ShoppingCart(db,Dialog::mutex,this);
    this->_infoManipulate = new InfoManipulate(db,Dialog::mutex,this);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);
    mainLayout->addWidget(this->tabWidget);
    this->tabWidget->addTab(this->_shoppingCart,"Shopping Cart");
    this->tabWidget->addTab(this->_infoManipulate,"Product Info Manipulate");
    connect(this->_ttyThread,SIGNAL(getABarcode(QString)),this->_shoppingCart,SLOT(addProductViaTTY(QString)));
    connect(this->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(switchFunction(int)));



    this->_ttyThread->start();
}

Dialog::~Dialog()
{
    delete this->db;
}

void Dialog::switchFunction(int index)
{
#ifdef DEBUG
    QMessageBox::warning(this,"Page changed","Current Page Number : "+QString::number(index));
#endif
    if (index != this->curIndex)
    {
        if (index == 0)
        {
            //disconnect another
            disconnect(this->_ttyThread,SIGNAL(getABarcode(QString)),this->_infoManipulate,SLOT(readBarcodeViaTTY(QString)));
            connect(this->_ttyThread,SIGNAL(getABarcode(QString)),this->_shoppingCart,SLOT(addProductViaTTY(QString)));
        }
        else
        {
            disconnect(this->_ttyThread,SIGNAL(getABarcode(QString)),this->_shoppingCart,SLOT(addProductViaTTY(QString)));
            connect(this->_ttyThread,SIGNAL(getABarcode(QString)),this->_infoManipulate,SLOT(readBarcodeViaTTY(QString)));
            //connect another
        }
        this->curIndex = index;
    }
}

void Dialog::initDB()
{
    this->db->exec(CREATE_INFO_TABLE);
    //this->db->exec("insert into products (barcode, name,price) values ('114514','ibm',19.6)");
    //this->db->exec("insert into products (barcode, name,price) values ('1919810','sun',80.1)");
}
