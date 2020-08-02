#include "trashcan.h"
#include "dragwidget.h"
#include "mainwindow.h"
#include "keyboarddropzone.h"

#include <QDebug>
#include <QTextStream>

#include <QtWidgets>

/*
 * USE THIS TO DRAG AND DELETE ITEMS FROM THE KEYBOARD
 */

TrashCan::TrashCan(QWidget *parent)
    : QWidget(parent)
{
    // layout
    QLayout *trash = new QGridLayout();
    trash->setAlignment(Qt::AlignTop);
    setLayout(trash); // https://doc.qt.io/qt-5/qgridlayout.html

    setAcceptDrops(true);
    setMinimumSize(100, 100);
    setMaximumSize(100, 100);
}

// drag enter override
void TrashCan::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}


// big bad trashcan baby
void TrashCan::dropEvent(QDropEvent *event)
{
    // event mime data
    const QMimeData *mime = event->mimeData();
    // debug
    QTextStream out(stdout);
    out << event->mimeData()->objectName() << "\n";

    // remove the key from main kb based on object named dropped on trash
    //MainWindow::deleteKeyWithId(mime->objectName().toInt());


}
