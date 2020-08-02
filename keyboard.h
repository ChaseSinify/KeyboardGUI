#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "key.h"
#include "selection.h"

#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class Keyboard : public QWidget
{

public:
    explicit Keyboard(QWidget *parent = nullptr);
    QString exportKeyboard();
    void modelKey(QRect keyLocation, QString keyText = "...");
    bool isFull(){return fullCheck;}

    void toggleSelectionTool();
    void toggleEraserTool();
    void clear();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //void dragLeaveEvent(QDragLeaveEvent *event) override;

private:
    int MAX_KEYS_LIMIT = 3200;
    int keyCount = 0;
    bool fullCheck = false;

    QMap<int, QLabel *> keysMap;

    QGridLayout* kblayout;
    QRect gridSize;
    QPoint spawnLocation = {0,0};

    int rows = 36;
    int cols = 100;

    QLabel* currentKey;

    QList<QString> keysToDelete;
    QString currentChildClickedObjectName;  

    // bool tools
    bool selection = false;
    bool eraser = false;
    bool selectionActive = false;
    bool mouseButtonActive = false;

    // selection
    QPoint pressPosition;
    QPoint releasePosition;
    Selection *currentSelection;
    QRect correctedSelectionPosition;
    Qt::MouseButton mouseStartButton;
    Qt::MouseButton mouseEndButton;

    // selection snapping helpers
    QPoint moveEventSnap;
};

#endif // KEYBOARDDROPZONE_H
