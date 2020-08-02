#ifndef BRUSH_H
#define BRUSH_H

#include <QRubberBand>
#include <QPaintEvent>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class Brush : public QRubberBand
{
    Q_OBJECT
public:
    explicit Brush(Shape brushShape, QWidget *parent, const QString &brushName, const QString &brushKey = "", QSize brushSize = {40, 40}, QColor brushColor = "ffffff",
                 const QString &brushStyleSheet = "");

signals:

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:

private:
    // get and set these sometime
    Shape brushShape;
    QString brushName;
    QString brushKey;
    QSize brushSize;
    QColor brushColor;
    QString brushStyleSheet;
};

#endif // BRUSH_H
