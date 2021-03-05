#include "virtualkeyboard.h"
#include "button.h"

VirtualKeyBoard::VirtualKeyBoard(QWidget *parent):
        QWidget(parent),
        display(new QLineEdit(this)),
        mainLayout(new QVBoxLayout(this)),
        pages(new QTabWidget(this)),
        isCapital(false)
{
    this->display->setReadOnly(true);
    this->mainLayout->addWidget(this->display);
    this->initButtons();
    this->mainLayout->addWidget(this->backspaceButton);
    this->mainLayout->addWidget(this->pages);

}

void VirtualKeyBoard::initButtons()
{
    QColor digitColor(150, 205, 205);
    QWidget *digitPage = new QWidget(this->pages);
    QGridLayout *digitGrid = new QGridLayout(digitPage);
    for (int i = 0; i < NumDigitButtons; ++i)
    {
         digitButtons[i] = createButton(QString::number(i), digitColor,
                                        SLOT(digitClicked()));
    }
    for (int i = 1; i < NumDigitButtons; ++i)
    {
         int row = ((9 - i) / 3);
         int column = ((i - 1) % 3);
         digitGrid->addWidget(digitButtons[i], row, column);
     }

    this->barButton = createButton("-",digitColor,
                                   SLOT(normalbuttonClicked()));
    this->dotButton = createButton(".",digitColor,
                                   SLOT(normalbuttonClicked()));
    digitGrid->addWidget(digitButtons[0],3,0);
    digitGrid->addWidget(this->dotButton,3,1);
    digitGrid->addWidget(this->barButton,3,2);
    digitPage->setLayout(digitGrid);
    this->pages->addTab(digitPage,"Numbers");
    QWidget *letterPage = new QWidget(this->pages);
    QGridLayout *letterGrid = new QGridLayout(letterPage);
    this->spaceButton = createButton(" ",digitColor,
                                   SLOT(normalbuttonClicked()));
    this->capslockButton = createButton("Caps",digitColor,
                                        SLOT(capslockClicked()));
    for (char i = 'a'; i <= 'z'; ++i)
    {
        int row = (i-'a')/6;
        int column=(i-'a')%6;
        this->letterButtons[i-'a'] = createButton(QString::fromAscii(&i,1),digitColor,
                                              SLOT(letterClicked()));
        if (i != 'z')
            letterGrid->addWidget(this->letterButtons[i-'a'],row,column);
        else
        {
            letterGrid->addWidget(this->letterButtons[i-'a'],row,column);
            letterGrid->addWidget(this->spaceButton,row,2,1,2);
            letterGrid->addWidget(this->capslockButton,row,4,1,2);
        }
    }
    letterPage->setLayout(letterGrid);
    this->pages->addTab(letterPage,"Letters");
    this->backspaceButton = createButton("<-Backspace",digitColor,
                                         SLOT(backspaceClicked()));

}

void VirtualKeyBoard::normalbuttonClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    this->display->setText(this->display->text() + clickedButton->text());
}

void VirtualKeyBoard::backspaceClicked()
{
   QString s = this->display->text();
   if (!s.isEmpty())
   {
       s.chop(1);
       this->display->setText(s);
   }
}

void VirtualKeyBoard::letterClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString appendLetter = clickedButton->text();
    if (this->isCapital)
    {
        appendLetter = appendLetter.toUpper();
    }
    else
    {
        appendLetter = appendLetter.toLower();
    }
     display->setText(display->text() + appendLetter);
}

Button *VirtualKeyBoard::createButton(const QString &text, const QColor &color,
                                  const char *member)
 {
     Button *button = new Button(text, color);
     connect(button, SIGNAL(clicked()), this, member);
     return button;
 }

 void VirtualKeyBoard::digitClicked()
 {
     Button *clickedButton = qobject_cast<Button *>(sender());
     int digitValue = clickedButton->text().toInt();
     display->setText(display->text() + QString::number(digitValue));
 }

 void VirtualKeyBoard::capslockClicked()
 {
     this->isCapital = (!this->isCapital);
     for (int i = 0; i < LetterButtons; ++i)
     {
         if (this->isCapital)
         {
             this->letterButtons[i]->setText(this->letterButtons[i]->text().toUpper());
         }
         else
         {
             this->letterButtons[i]->setText(this->letterButtons[i]->text().toLower());
         }
     }
 }

 QString VirtualKeyBoard::getText()
 {
     return this->display->text();
 }
 void VirtualKeyBoard::clearText()
 {
     this->display->setText("");
 }
void VirtualKeyBoard::displayText(const QString& str2Display)
{
    this->display->setText(str2Display);
}
