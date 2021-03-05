#include "infomanipulate.h"
#include "virtualkeyboard.h"
#include "dialog.h"
#include "dbsqlite.h"
#include "button.h"

InfoManipulate::InfoManipulate(DBSqlite *aDb,QMutex& lock,QWidget *parent):
        QWidget(parent),
        father(dynamic_cast<Dialog*>(parent)),
        proxyModel(new QSortFilterProxyModel),
        proxyView(new QTreeView),
        model(new QStandardItemModel(0,3,this)),
        db(aDb),
        mutex(lock),
        vkbd(new VirtualKeyBoard(this)),
        curState(initState),
        instructLabel(new QLabel(this))
{

    this->proxyModel->setDynamicSortFilter(true);

    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setModel(this->proxyModel);
    proxyView->setSortingEnabled(true);
    this->initModel();
    //QMessageBox::warning(this,"",QString::number(this->model->columnCount()));
    for (int i = 0; i < this->model->columnCount(); ++i)
    {
        this->proxyView->setColumnWidth(i,60);
    }

    this->initButtons();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QVBoxLayout *leftSideLayout = new QVBoxLayout;
    QVBoxLayout *rightSideLayout = new QVBoxLayout;

    buttonLayout->addWidget(this->startAddButton);
    buttonLayout->addWidget(this->readNameButton);
    buttonLayout->addWidget(this->readBarcodeButton);
    buttonLayout->addWidget(this->readPriceButton);
    buttonLayout->addWidget(this->startDeleteButton);
    buttonLayout->addWidget(this->giveupButton);
    buttonLayout->setSizeConstraint(QLayout::SetMinimumSize);

    leftSideLayout->addWidget(this->proxyView);
    leftSideLayout->addLayout(buttonLayout);


    rightSideLayout->addWidget(this->instructLabel);
    rightSideLayout->addWidget(this->vkbd);

    mainLayout->addLayout(leftSideLayout);
    mainLayout->addLayout(rightSideLayout);

}

void InfoManipulate::initButtons()
{
    QColor buttonColor(150, 205, 205);
    this->startAddButton = new Button("add",buttonColor,this);
    this->startDeleteButton = new Button("Del",buttonColor,this);
    this->readNameButton = new Button("Name",buttonColor,this);
    this->readBarcodeButton = new Button("Barcode",buttonColor,this);
    this->readPriceButton = new Button("Price",buttonColor,this);
    this->giveupButton = new Button("GvUp",buttonColor,this);
    this->readNameButton->setEnabled(false);
    this->readBarcodeButton->setEnabled(false);
    this->readPriceButton->setEnabled(false);
    this->giveupButton->setEnabled(false);

    connect(this->startAddButton,SIGNAL(clicked()),this,SLOT(startAdd()));
    connect(this->startDeleteButton,SIGNAL(clicked()),this,SLOT(startDelete()));
    connect(this->readNameButton,SIGNAL(clicked()),this,SLOT(readName()));
    connect(this->readBarcodeButton,SIGNAL(clicked()),this,SLOT(readBarcodeViaVKBD()));
    connect(this->readPriceButton,SIGNAL(clicked()),this,SLOT(readPrice()));
    connect(this->giveupButton,SIGNAL(clicked()),this,SLOT(giveup()));
}


void InfoManipulate::initModel()
{
    this->model->setHeaderData(0,Qt::Horizontal,QObject::tr("Name"));
    this->model->setHeaderData(1,Qt::Horizontal,QObject::tr("Price"));
    this->model->setHeaderData(2,Qt::Horizontal,QObject::tr("Barcode"));
    this->setSourceModel(this->model);
    DBSqliteStmt query(this->db->prepare("select * from products"));
    while (query.next())
    {
        this->addRow(query.getString(1),query.getDouble(2),query.getString(0));
    }
    query.close();
}

void InfoManipulate::addRow(const QString& name, const float price, const QString& barcode)
{
    if (this->model->findItems(barcode,Qt::MatchExactly,3).empty())
    {
        this->model->insertRow(0);
        this->model->setData(this->model->index(0, 0), name);
        this->model->setData(this->model->index(0, 1), price);
        this->model->setData(this->model->index(0, 2), barcode);
    }
    else
    {
#ifdef DEBUG
        int aRow = this->model->findItems(barcode,Qt::MatchExactly,3).front()->row();
        QMessageBox::warning(this,"Product Already Exist","The Product Exists at row : "+QString::number(aRow));
#endif
    }
}


 void InfoManipulate::setSourceModel(QAbstractItemModel *model)
 {
     proxyModel->setSourceModel(model);
 }

 void InfoManipulate::startAdd()
 {
     this->mutex.lock();
     if (this->curState == initState)
     {
         this->model->insertRow(0);
         this->curState = insertName;
         this->giveupButton->setEnabled(true);
         this->readNameButton->setEnabled(true);
         this->startAddButton->setEnabled(false);
         this->startDeleteButton->setEnabled(false);
     }
     else
     {
         QMessageBox::warning(this,"Illegal Op","Start Add at State : "+QString::number(this->curState));
     }
     this->mutex.unlock();
 }

 void InfoManipulate::readName()
 {
     this->mutex.lock();
     QString name(this->vkbd->getText());
     if (this->curState == insertName)
     {
         //save name
         this->model->setData(this->model->index(0,0),name);
         this->curState = insertBarcode;
         this->readNameButton->setEnabled(false);
         this->readBarcodeButton->setEnabled(true);
     }
     else if (this->curState == delState)
     {
         //delete via name
         QList<QStandardItem*> finds = this->model->findItems(name,Qt::MatchExactly,0);
         if (finds.empty())
         {
             QMessageBox::warning(this,"No Such Product","No Product with name : "+name);
         }
         else
         {
             DBSqliteStmt  stmt(this->db->prepare("delete from products where name = ?"));
             stmt.bindString(0,name);
             if (stmt.exec())
             {
                 for (int i = 0; i < finds.size(); ++i)
                 {
                     int aRow = finds[i]->row();
                     this->model->removeRow(aRow);
                 }
             }
             else
             {
                 QMessageBox::warning(this,"Sql Failed", "Failed to delete product with name : "+name);
             }
             stmt.close();
         }
         this->curState = initState;
         this->giveupButton->setEnabled(false);
         this->startAddButton->setEnabled(true);
         this->readNameButton->setEnabled(false);
         this->readBarcodeButton->setEnabled(false);
         this->startDeleteButton->setEnabled(true);
     }
     else
     {
         QMessageBox::warning(this,"Illegal Op","Read name at State : "+QString::number(this->curState));
     }
     this->mutex.unlock();
 }

 void InfoManipulate::readBarcodeViaVKBD()
 {
     this->mutex.lock();
     QString barcode(this->vkbd->getText());
     if (this->curState == insertBarcode)
     {
         //save barcode
         this->model->setData(this->model->index(0,2),barcode);
         this->curState = insertPrice;
         this->readBarcodeButton->setEnabled(false);
         this->readPriceButton->setEnabled(true);
     }
     else if (this->curState == delState)
     {
         //delete via barcode
         QList<QStandardItem*> finds = this->model->findItems(barcode,Qt::MatchExactly,2);
         if (finds.empty())
         {
             QMessageBox::warning(this,"No Such Product","No Product with barcode : "+barcode);
         }
         else
         {
             DBSqliteStmt  stmt(this->db->prepare("delete from products where barcode = ?"));
             stmt.bindString(0,barcode);
             if (stmt.exec())
             {
                 for (int i = 0; i < finds.size(); ++i)
                 {
                     int aRow = finds[i]->row();
                     this->model->removeRow(aRow);
                 }
             }
             else
             {
                 QMessageBox::warning(this,"Sql Failed", "Failed to delete product with barcode : "+barcode);
             }
             stmt.close();
         }
         this->curState = initState;
         this->giveupButton->setEnabled(false);
         this->startAddButton->setEnabled(true);
         this->readBarcodeButton->setEnabled(false);
         this->readNameButton->setEnabled(false);
         this->startDeleteButton->setEnabled(true);
     }
     else
     {
         QMessageBox::warning(this,"Illegal Op","Read barcode at State : "+QString::number(this->curState));
     }
     this->mutex.unlock();
 }
 void InfoManipulate::readBarcodeViaTTY(QString barcode)
 {
     this->mutex.lock();
     if (this->curState == insertBarcode)
     {
         //save barcode
         this->model->setData(this->model->index(0,2),barcode);
         this->curState = insertPrice;
         this->readBarcodeButton->setEnabled(false);
         this->readPriceButton->setEnabled(true);
     }
     else if (this->curState == delState)
     {
         //delete via barcode
         QList<QStandardItem*> finds = this->model->findItems(barcode,Qt::MatchExactly,2);
         if (finds.empty())
         {
             QMessageBox::warning(this,"No Such Product","No Product with barcode : "+barcode);
         }
         else
         {
             DBSqliteStmt  stmt(this->db->prepare("delete from products where barcode = ?"));
             stmt.bindString(0,barcode);
             if (stmt.exec())
             {
                 for (int i = 0; i < finds.size(); ++i)
                 {
                     int aRow = finds[i]->row();
                     this->model->removeRow(aRow);
                 }
             }
             else
             {
                 QMessageBox::warning(this,"Sql Failed", "Failed to delete product with barcode : "+barcode);
             }
             stmt.close();
         }
         this->curState = initState;
         this->giveupButton->setEnabled(false);
         this->readBarcodeButton->setEnabled(false);
         this->readNameButton->setEnabled(false);
         this->startAddButton->setEnabled(true);
         this->startDeleteButton->setEnabled(true);
     }
     else
     {
         QMessageBox::warning(this,"Illegal Op","Read barcode at State : "+QString::number(this->curState));
     }
     this->mutex.unlock();
 }
 void InfoManipulate::readPrice()
 {
     this->mutex.lock();
     double price = this->vkbd->getText().toDouble();
     if (price <= 0)
     {
         QMessageBox::warning(this,"Invalid Price","Invalid Price : "+this->vkbd->getText());
     }
     else
     {
         if (this->curState == insertPrice)
         {
             //save price

             QString name(this->model->data(this->model->index(0,0)).toString());
             QString barcode(this->model->data(this->model->index(0,2)).toString());
             DBSqliteStmt stmt(this->db->prepare("insert into products (barcode,name,price) values(?,?,?)"));
             stmt.bindString(0,barcode);
             stmt.bindString(1,name);
             stmt.bindDouble(2,price);
             if (stmt.exec())
             {
                 this->model->setData(this->model->index(0,1),price);
             }
             else
             {
                 QMessageBox::warning(this,"Sql Failed","Failed to do insert barcode : "+barcode+"\nname : "+name+"\nprice : "+QString::number(price,'f',1));
                 this->model->removeRow(0);
             }
             stmt.close();
             this->curState = initState;
             this->readPriceButton->setEnabled(false);
             this->giveupButton->setEnabled(false);
             this->startAddButton->setEnabled(true);
             this->startDeleteButton->setEnabled(true);
         }
         else
         {
             QMessageBox::warning(this,"Illegal Op","Read Price in State : "+QString::number(this->curState));
         }
     }
     this->mutex.unlock();
 }
 void InfoManipulate::startDelete()
 {
     this->mutex.lock();
     if (this->curState == initState)
     {
         this->curState = delState;
         this->startDeleteButton->setEnabled(false);
         this->readBarcodeButton->setEnabled(true);
         this->readNameButton->setEnabled(true);
         this->giveupButton->setEnabled(true);
         this->startAddButton->setEnabled(false);
     }
     else
     {
         QMessageBox::warning(this,"Illegal Op","Start delete at State : "+QString::number(this->curState));
     }
     this->mutex.unlock();
 }
 void InfoManipulate::giveup()
 {
     this->mutex.lock();
     //do give up
     if (this->curState != delState)
     {
         this->model->removeRow(0);
     }
     this->curState = initState;
     this->giveupButton->setEnabled(false);
     this->startAddButton->setEnabled(true);
     this->startDeleteButton->setEnabled(true);
     this->readBarcodeButton->setEnabled(false);
     this->readNameButton->setEnabled(false);
     this->readPriceButton->setEnabled(false);
     this->mutex.unlock();
 }
