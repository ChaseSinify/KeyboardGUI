#include "brush.h"

#include <QPainter>
#include <QPalette>

Brush::Brush(QRubberBand::Shape brushShape, QWidget *parent, const QString &brushName, const QString &brushKey, QSize brushSize,
             QColor brushColor, const QString &brushStyleSheet) :
    QRubberBand(brushShape, parent)
{
    this->brushShape = brushShape;
    this->brushName = brushName;
    this->brushKey = brushKey;
    this->brushSize = brushSize;
    this->brushColor= brushColor;
    this->brushStyleSheet = brushStyleSheet;
}

void Brush::paintEvent(QPaintEvent *event)
{
    QRubberBand::paintEvent(event);

    // paint -- theres a way to futher customize selections with paint override here;
    // however, not really important
    QPainter paint(this);
    paint.setPen(QPen(Qt::black,2));

    // pallete
    QPalette pallete;
    pallete.setBrush(QPalette::Highlight, QBrush(this->brushColor)); // pretty sure this is OS specific, where some have default overrides (doesnt crash or anything, though)

    this->setPalette(pallete);

    // minimum selection size
    if(size().width() >10 && size().height() >10) // smallest possible selection on grid
    {
        paint.drawText(20,20,QString("%1,%2").arg(size().width()).arg(size().height()));
    }
}
