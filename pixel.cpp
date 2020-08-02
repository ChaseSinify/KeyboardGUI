#include "pixel.h"
#include "drawgrid.h"
#include <QWidget>

Pixel::Pixel(QWidget* parent) // take in a brush here from global or drawgrid
    : QWidget(parent)
{
    // basic
    setAcceptDrops(false); // changed from true
    setMinimumSize(10, 10);
}
