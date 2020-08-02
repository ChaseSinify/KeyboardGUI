#include "keyboard.h"
#include "key.h"

#include <QtWidgets>

static QString hotSpotMimeDataKey() { return QStringLiteral("application/x-hotspot"); }

Keyboard::Keyboard(QWidget *parent)
    : QWidget(parent)
{
    // layout basics
    this->kblayout = new QGridLayout;
    this->kblayout->setAlignment(Qt::AlignLeft);
    this->kblayout->setRowStretch(0,0);
    this->kblayout->setColumnStretch(0,0);
    this->kblayout->setContentsMargins(0,0,0,0);
    this->kblayout->setHorizontalSpacing(0);
    this->kblayout->setVerticalSpacing(0);
    this->kblayout->setRowMinimumHeight(0, 10);
    this->kblayout->setColumnMinimumWidth(0, 10);

    this->setLayout(this->kblayout); // https://doc.qt.io/qt-5/qgridlayout.html
    this->setStyleSheet("background: transparent; border: 1px solid rgba(255, 255, 255, 1);");
    this->setAcceptDrops(true);
}
// model key from drawgrid onto keyboard
void Keyboard::modelKey(QRect keyLocation, QString keyText)
{
    // we should be tracking deletes as well to monitor current key counts, this class needs a function for inc/dec by n keys deleted
    if(this->keyCount <= this->MAX_KEYS_LIMIT && this->fullCheck == false) // <=
    {
        Key *key = new Key(this, keyText); // obviosuly adjust these values later

        key->move(keyLocation.left(), keyLocation.top());
        key->resize(keyLocation.width(), keyLocation.height());
        key->setStyleSheet("background: black; border: 1px solid white; color: white;");

        key->setFocusPolicy(Qt::StrongFocus);
        key->setAcceptDrops(false);
        key->setEnabled(true);

        key->show();
    }

    if(this->keyCount == MAX_KEYS_LIMIT)
    {
        this->fullCheck = true;
        return;
    }
    else
    {
        this->keyCount++;
    }
}
// this basically tells us everything we would need for mapping layouts from the drawing grid to the keyboard model
QString Keyboard::exportKeyboard()
{
    QString exportData;
    QString posx;
    QString posy;
    QString width;
    QString height;
    QList<QWidget *> children = this->findChildren<QWidget *>();
    if(!children.isEmpty())
    {
        for(auto child : children)
        {
            posx = QString::number(child->geometry().x());
            posy = QString::number(child->geometry().y());
            width = QString::number(child->geometry().width());
            height = QString::number(child->geometry().height());
            exportData += "[" + posx + "," + posy + "," + width + "," + height + "],";
        }
    }
    return exportData;
}
// clear
void Keyboard::clear()
{
    QList<QWidget *> children = this->findChildren<QWidget *>("", Qt::FindDirectChildrenOnly);
    if(!children.isEmpty())
    {
        for(auto child : children)
        {
            delete child;
        }
    }
}
// drag enter -- not really needed
void Keyboard::dragEnterEvent(QDragEnterEvent *event)
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
// drop event
void Keyboard::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        const QMimeData *mime = event->mimeData();

        QPoint position = event->pos(); // x,y cursor position
        QPoint hotSpot; // x,y point relative to drag object
        QPoint snapPoint;
        QSize keySize;
        // hotspot is cursor relative to topleft corner of widget grabbed
        QByteArrayList hotSpotPos = mime->data(hotSpotMimeDataKey()).split(' ');

        if(hotSpotPos.size() == 4)
        {
            hotSpot.setX(hotSpotPos[0].toInt());
            hotSpot.setY(hotSpotPos[1].toInt());
            keySize.setWidth(hotSpotPos[2].toInt());
            keySize.setHeight(hotSpotPos[3].toInt());
        }

        int horizontal = position.x() - hotSpot.x(); // refers to left border of drag object
        int vertical = position.y() - hotSpot.y(); // refers to top border of drag object

        // within keysize away -- else delete on drop
        // left x
        if(horizontal < 0 && horizontal > - keySize.width()) // left wall
            horizontal = 0;
        // right x
        if(horizontal > 1000 - keySize.width() && horizontal < 1000) // right wall - key width
            horizontal = 1000 - keySize.width();
        // top y
        if(vertical < 0 && vertical > - keySize.height()) // top wall
            vertical = 0;
        // bottom y
        if(vertical > 360 - keySize.height() && vertical < 360) // bottom wall - key height
            vertical = 360 - keySize.height();

        // grid snapping based on quarter keys i.e. 0.25 --> 40/10

        // correct x
        if(horizontal % 10 >= 5) // i.e. 116LCtrl
            snapPoint.setX(horizontal + (10 - (horizontal % 10))); // set to 116 + 4
        else // i.e. 113
            snapPoint.setX((horizontal - (horizontal % 10))); // set to 113 - 3

        // correct yb
        if(vertical % 10 >= 5) // i.e. 116
            snapPoint.setY(vertical + (10 - (vertical % 10))); // set to 116 + 4
        else // i.e. 113
            snapPoint.setY((vertical - (vertical % 10))); // set to 113 - 3

        //TESTING
        Key* key = new Key(this, mime->text());
        key->resize(keySize);

        key->setStyleSheet("background: black; border: 1px solid white; color: white;");
        // could compare mime->objectname (which is key id) to see if it is in keysmap
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        // if there was a move event and we were NOT the source, a key was added to the board, increment our id
        // IMPORTANT: NEED VALUE CHECKING ON BOTH OF THESE TO AVOID HAVING DUPLICATES OR FALSE INCREMENTS
        else
        {
            event->acceptProposedAction();
        }
        key->move(snapPoint);
        if(!key->isVisible())
        {
            key->show();
        }
        else
        {
            //qDebug() << "15: was visible";
        }
        key->setFocus(); // after a drag event, leave focus here until we manually click another key or widget
    }
    else
    {
        //qDebug() << "###: mime didnt have text";
        event->ignore();
    }
}
// mouse press event
void Keyboard::mousePressEvent(QMouseEvent *event)
{
//
}
// mouse move event
void Keyboard::mouseMoveEvent(QMouseEvent *event)
{
//
}
// mouse release event
void Keyboard::mouseReleaseEvent(QMouseEvent *event)
{
//
}
// selection tool
void Keyboard::toggleSelectionTool()
{
    if(this->selection == false)
    {
        this->selection = true;
        if (this->eraser == true)
        {
            this->setCursor(Qt::SplitHCursor);
        }
        else
        {
            this->setCursor(Qt::CrossCursor);
        }
    }
    else
    {
        this->selection = false;
        if (this->eraser == true)
        {
            this->setCursor(Qt::OpenHandCursor);
        }
        else
        {
            this->setCursor(Qt::ArrowCursor);
        }
    }
}
// eraser tool
void Keyboard::toggleEraserTool()
{
    if(this->eraser == false)
    {
        this->eraser = true;
        if(this->selection == true)
        {
            this->setCursor(Qt::SplitHCursor);
        }
        else
        {
            this->setCursor(Qt::OpenHandCursor);
        }
    }
    else
    {
        this->eraser = false;
        if(this->selection == true)
        {
            this->setCursor(Qt::CrossCursor);
        }
        {
            this->setCursor(Qt::ArrowCursor);
        }
    }
}
