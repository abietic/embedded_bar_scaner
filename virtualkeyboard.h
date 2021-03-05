#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QtGui>
class Button;
class VirtualKeyBoard : public QWidget
{
    Q_OBJECT
public:
    VirtualKeyBoard(QWidget *parent = 0);
    QString getText();
    void clearText();
    void displayText(const QString& str2Display);
private slots:
    void digitClicked();
    void capslockClicked();
    void letterClicked();
    void backspaceClicked();
    void normalbuttonClicked();
private:
    Button *createButton(const QString &text, const QColor &color,
                          const char *member);
    void initButtons();
    enum { NumDigitButtons = 10 , LetterButtons = 26};
    Button *digitButtons[NumDigitButtons];
    Button *letterButtons[LetterButtons];
    Button *backspaceButton;
    Button *capslockButton;
    Button *spaceButton;
    Button *dotButton;
    Button *barButton;
    QLineEdit *display;
    QTabWidget *pages;
    QVBoxLayout *mainLayout;
    bool isCapital;
};

#endif // VIRTUALKEYBOARD_H
