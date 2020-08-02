#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QWidget>
#include <QRect>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class DragWidget : public QWidget
{
public:
    explicit DragWidget(QWidget *parent = nullptr, QString keyText = "", int styleId = 0, QPoint position = {0, 0}, QSize keySize = {40, 40}, QString styleSheet = "");

    // get and set these sometime
    QString keyText;

    int styleId;
    QPoint position;
    QSize keySize;
    QString styleSheet;

    //TESTING
    QString getKeyText(){return keyText;}
    void setKeyText(QString text);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    //void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    //TESTING
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    // for when we implement getters and setters
    QPoint mouseDownPosition; // holds when we clicked -> used to determine if we dragged or if we want to change text
    QPoint mouseReleasePosition; // holds where we realeased -> used to determine if we dragged or if we want to change text
};

#endif // DRAGWIDGET_H
