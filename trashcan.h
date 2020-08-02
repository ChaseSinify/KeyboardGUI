#ifndef TRASHCAN_H
#define TRASHCAN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class TrashCan : public QWidget
{
public:
    explicit TrashCan(QWidget *parent = nullptr);

protected:

    // event overrides
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
};

#endif // TRASHCAN_H
