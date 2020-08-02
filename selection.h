#ifndef SELECTION_H
#define SELECTION_H

#include <QRubberBand>
#include <QPaintEvent>

class Selection : public QRubberBand
{
    Q_OBJECT

public:
    explicit Selection(Shape shape, QWidget *parent = nullptr, QColor selectionColor = "#03befc");

signals:

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:

private:
    QColor selectionColor;
    Shape shape;
};

#endif // SELECTION_H
