#include <QtGui>

 #include "button.h"

 Button::Button(const QString &text, const QColor &color, QWidget *parent)
     : QToolButton(parent)
 {
     setMinimumSize(20,20);
     setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
     setText(text);

     QPalette newPalette = palette();
     newPalette.setColor(QPalette::Button, color);
     setPalette(newPalette);
 }

 QSize Button::sizeHint() const
 {
     QSize size = QToolButton::sizeHint();
     size.rheight() += 20;
     size.rwidth() = qMax(size.width(), size.height());
     return size;
 }
