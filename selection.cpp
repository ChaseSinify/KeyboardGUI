#include "selection.h"
#include <QPainter>
#include <QPalette>

Selection::Selection(QRubberBand::Shape shape, QWidget *parent, QColor selectionColor) :
    QRubberBand(shape, parent)
{
    this->shape = shape;
    this->selectionColor = selectionColor;
}

void Selection::paintEvent(QPaintEvent *event)
{
    QRubberBand::paintEvent(event);

    // paint -- theres a way to futher customize selections with paint override here;
    // however, not really important
    QPainter paint(this);
    paint.setPen(QPen(Qt::black,2));

    // pallete
    QPalette pallete;
    pallete.setBrush(QPalette::Highlight, QBrush(this->selectionColor)); // pretty sure this is OS specific, where some have default overrides (doesnt crash or anything, though)

    this->setPalette(pallete);

    // minimum selection size
    if(size().width() >10 && size().height() >10) // smallest possible selection on grid
    {
        paint.drawText(20,20,QString("%1,%2").arg(size().width() / 10).arg(size().height() / 10));
    }
}
