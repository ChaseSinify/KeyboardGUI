#include "key.h"
#include <QtWidgets>
#include <QWidget>

#define EXTENDED_KEY_MASK   0x01000000 // read up more on these
#define LSHIFT_MASK         0x2a
#define RSHIFT_MASK         0x36 // was 0X36
#define PRTSCN_MASK         0x37 // Unused for now

/*
 * https://doc.qt.io/archives/qt-4.8/qfocusevent.html IMPORTANT FOCUS EVENT INFO
 */

static QString hotSpotMimeDataKey() { return QStringLiteral("application/x-hotspot"); }

Key::Key(QWidget *parent, QString keyText)
    : QLabel(parent)
{
    // set the key text
    this->setText(keyText);
    // focus policy (works)
    this->setFocusPolicy(Qt::StrongFocus);
    // event filter (works)
    this->installEventFilter(this);
    // basics
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAcceptDrops(false);
    this->setMinimumSize(40, 40);
    this->setBaseSize(40, 40);
    this->resize(40, 40);
    // text wrapping
    this->setWordWrap(true);
}

//TESTING
void Key::toggleSelected()
{
    if(this->selected == true)
    {
        this->selected = false;
    }
    else
    {
        this->selected = true;
    }
}
void Key::focusInEvent(QFocusEvent *event)
{
    this->setStyleSheet("background: red; color: white; border: 1px solid white;");
};
void Key::focusOutEvent(QFocusEvent *event)
{
    this->setStyleSheet("background: black; color: white; border: 1px solid white;");
};
// mouse press event override
void Key::mousePressEvent(QMouseEvent *event)
{
    // attempt to avoid clicking causing drop events ...
//    if (event->button() == Qt::LeftButton)
//    {
//        this->
//    }

//    this->setFocus();
    emit clicked();

}
// mouse move event
void Key::mouseMoveEvent(QMouseEvent *event)
{
    // attempt to avoid clicking causing drop events ... WORKS
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QPoint hotSpot = event->pos() - mapFromParent(this->pos()); // correctly gets hotspot -- different than mainwindow due to parent hiearchy present here

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(this->text());
    mimeData->setData(hotSpotMimeDataKey(),
                      QByteArray::number(hotSpot.x()) + ' ' + QByteArray::number(hotSpot.y()) + ' ' // HOTSPOT
                      + QByteArray::number(this->width()) + ' ' + QByteArray::number(this->height())); // SIZE

    qreal dpr = window()->windowHandle()->devicePixelRatio();
    QPixmap pixmap(this->size() * dpr);
    pixmap.setDevicePixelRatio(dpr);
    this->render(&pixmap);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(hotSpot);

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);//, Qt::CopyAction); // | Qt::MoveAction, Qt::CopyAction); // was using just copyaction personally... testing move for now

    if (dropAction == Qt::MoveAction)
        this->close();
}
// event filter
bool Key::eventFilter(QObject *object, QEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    // If the event was from main_keyboard and it was a keypress -- this also implies we have main keyboard focus
    if (object == this && event->type() == QEvent::KeyPress)
    {
        // LEFT & RIGHT WITH VK_KEYS | SEE: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
        quint32 vk = keyEvent->nativeVirtualKey();
        quint32 mods = keyEvent->nativeModifiers();
        quint32 scode = keyEvent->nativeScanCode();
        bool extended = (bool(mods & EXTENDED_KEY_MASK)); // ???

        switch (vk) // FN KEY HAS NO SCAN CODE -- USED FOR EMULATING KEYS AT OS LEVEL https://stackoverflow.com/questions/24423724/finding-the-scan-code-of-fn-key
        {
            case VK_CONTROL:
                if(extended)
                    this->setText("RCtrl");
                else
                    this->setText("LCtrl");
                break;
            case VK_MENU:
                // VK_MENU = ALT virtual key
                if(extended)
                    this->setText("RAlt");
                else
                    this->setText("LAlt");
                break;
            case VK_SHIFT:
                if ((scode ^ LSHIFT_MASK) == 0) {
                    //vk = VK_LSHIFT;
                    this->setText("LShift");
                } else if ((scode ^ RSHIFT_MASK) == 0) {
                    //vk = VK_RSHIFT;
                    this->setText("RShift");
                }
                break;
            default: // ???
                break;
        }
        // PRINT SCREEN (os captures this before code, see link)
        //if (keyEvent->key() == Qt::Key_SysReq || keyEvent->key() == Qt::Key_Print || keyEvent->key() == Qt::Key_Printer) // IMPORTANT: https://stackoverflow.com/questions/27002328/qt-printscreen-key-in-keypressevent
        //{
            //ui->_print_screen->setText("PrtScn");
            //return true;
        //}
        // SUPER (os captures this before code, see link) // COMPLICATED
/*else*/if (keyEvent->key() == Qt::Key_Meta)
        {
            this->setText("Meta"); // Should apply to windows key
            return true;
        }
        // SPACE
        else if (keyEvent->key() == Qt::Key_Space)
        {
            this->setText("Space");
            return true;
        }
        else // not a special case -- but still a key event [switches]
        {
            switch(keyEvent->key()) // Qt::Key Enum -- Need to expand this fully... try to find this done online
            {
                case Qt::Key_Escape:
                    this->setText("Esc");
                    break;
                //TESTING -- VERTICAL IS INVERTED HERE... -10 MOVES KEY UP SCREEN, +10 DOWN
                // boundry checks to prevent overflow on key movements
                // UP ARROW
                case Qt::Key_Up:
                    if(keyEvent->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier)) // control + shift + arrow key = inverse resize
                    {
                        if(this->height() >= 50)
                        {
                            this->resize(this->width(), this->height() - 10);
                        }
                    }
                    else if(keyEvent->modifiers() == Qt::ControlModifier) // control + arrow key = resize
                    {
                        if(this->pos().y() >= 10)
                        {
                            this->resize(this->width(), this->height() + 10);
                            this->move(this->pos().x(), this->pos().y() - 10);
                        }
                    }
                    else if(keyEvent->modifiers() == Qt::ShiftModifier) // shift + arrow key = set text
                    {
                        this->setText("UP");
                    }
                    else if(this->pos().y() >= 10) // arrow key = move
                    {
                        this->move(this->pos().x(), this->pos().y() - 10);
                    }

                    break;
                // LEFT ARROW
                case Qt::Key_Left:
                    if(keyEvent->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier)) // control + shift + arrow key = inverse resize
                    {
                        if(this->width() >= 50)
                        {
                            this->resize(this->width() - 10, this->height());
                        }
                    }
                    else if(keyEvent->modifiers() == Qt::ControlModifier)
                    {
                        if(this->pos().x() >= 10)
                        {
                            this->resize(this->width() + 10, this->height());
                            this->move(this->pos().x() - 10, this->pos().y());
                        }
                    }
                    else if(keyEvent->modifiers() == Qt::ShiftModifier)
                    {
                        this->setText("LEFT");
                    }

                    else if(this->pos().x() >= 10)
                    {
                        this->move(this->pos().x() - 10, this->pos().y());
                    }
                    break;
                // DOWN ARROW
                case Qt::Key_Down:
                    if(keyEvent->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier)) // control + shift + arrow key = inverse resize
                    {
                        if(this->height() >= 50)
                        {
                            this->resize(this->width(), this->height() - 10);
                            this->move(this->pos().x(), this->pos().y() + 10);
                        }
                    }
                    else if(keyEvent->modifiers() & Qt::ControlModifier)
                    {
                        if(this->pos().y() + this->height() <= 350)
                        {
                            this->resize(this->width(), this->height() + 10);
                        }
                    }
                    else if(keyEvent->modifiers() & Qt::ShiftModifier)
                    {
                        this->setText("DOWN");
                    }
                    else if(this->pos().y() + this->height() <= 350)
                    {
                        this->move(this->pos().x(), this->pos().y() + 10);
                    }
                    break;
                // RIGHT ARROW
                case Qt::Key_Right:
                    if(keyEvent->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier)) // control + shift + arrow key = inverse resize
                    {
                        if(this->width() >= 50)
                        {
                            this->resize(this->width() - 10, this->height());
                            this->move(this->pos().x() + 10, this->pos().y());
                        }
                    }
                    else if(keyEvent->modifiers() & Qt::ControlModifier)
                    {
                        if(this->pos().x() + this->width() <= 990)
                        {
                            this->resize(this->width() + 10, this->height());
                        }
                    }
                    else if(keyEvent->modifiers() & Qt::ShiftModifier)
                    {
                        this->setText("RIGHT");
                    }
                    else if(this->pos().x() + this->width() <= 990)
                    {
                        this->move(this->pos().x() + 10, this->pos().y());
                    }
                    break;
                case Qt::Key_F1:
                    this->setText("F1");
                    break;
                case Qt::Key_F2:
                    this->setText("F2");
                    break;
                case Qt::Key_F3:
                    this->setText("F3");
                    break;
                case Qt::Key_F4:
                    this->setText("F4");
                    break;
                case Qt::Key_F5:
                    this->setText("F5");
                    break;
                case Qt::Key_F6:
                    this->setText("F6");
                    break;
                case Qt::Key_F7:
                    this->setText("F7");
                    break;
                case Qt::Key_F8:
                    this->setText("F8");
                    break;
                case Qt::Key_F9:
                    this->setText("F9");
                    break;
                case Qt::Key_F10:
                    this->setText("F10");
                    break;
                case Qt::Key_F11:
                    this->setText("F11");
                    break;
                case Qt::Key_F12:
                    this->setText("F12");
                    break;
                case Qt::Key_ScrollLock:
                    this->setText("Scroll Lock");
                    break;
                case Qt::Key_Pause:
                    this->setText("Pause Break");
                    break;
                case Qt::Key_QuoteLeft: // or maybe Qt::Key_Dead_Grave | GRAVE ACCENT WINDOWS
                    this->setText("`");
                    break;
                case Qt::Key_1:
                    this->setText("1");
                    break;
                case Qt::Key_2:
                    this->setText("2");
                    break;
                case Qt::Key_3:
                    this->setText("3");
                    break;
                case Qt::Key_4:
                    this->setText("4");
                    break;
                case Qt::Key_5:
                    this->setText("5");
                    break;
                case Qt::Key_6:
                    this->setText("6");
                    break;
                case Qt::Key_7:
                    this->setText("7");
                    break;
                case Qt::Key_8:
                    this->setText("8");
                    break;
                case Qt::Key_9:
                    this->setText("9");
                    break;
                case Qt::Key_0:
                    this->setText("0");
                    break;
                case Qt::Key_Minus:
                    this->setText("-");
                    break;
                case Qt::Key_Equal:
                    this->setText("=");
                    break;
                case Qt::Key_Backspace:
                    this->setText("Backspace");
                    break;
                case Qt::Key_Insert:
                    this->setText("Insert");
                    break;
                case Qt::Key_Home:
                    this->setText("Home");
                    break;
                case Qt::Key_PageUp:
                    this->setText("Page Up");
                    break;
                case Qt::Key_Tab:
                    this->setText("Tab");
                    break;
                case Qt::Key_Q:
                    this->setText("Q");
                    break;
                case Qt::Key_W:
                    this->setText("W");
                    break;
                case Qt::Key_E:
                    this->setText("E");
                    break;
                case Qt::Key_R:
                    this->setText("R");
                    break;
                case Qt::Key_T:
                    this->setText("T");
                    break;
                case Qt::Key_Y:
                    this->setText("Y");
                    break;
                case Qt::Key_U:
                    this->setText("U");
                    break;
                case Qt::Key_I:
                    this->setText("I");
                    break;
                case Qt::Key_O:
                    this->setText("O");
                    break;
                case Qt::Key_P:
                    this->setText("P");
                    break;
                case Qt::Key_BracketLeft:
                    this->setText("[");
                    break;
                case Qt::Key_BracketRight:
                    this->setText("]");
                    break;
                case Qt::Key_Slash: // this is forward slash
                    this->setText("/");
                    break;
                // case testing for delete: shift delete sets text, regular delete deletes the key
                case Qt::Key_Delete:
                    if(keyEvent->modifiers() & Qt::ShiftModifier)
                    {
                        this->setText("Delete");
                    }
                    else
                    {
                        this->deleteLater();
                    }
                    break;
                case Qt::Key_End:
                    this->setText("End");
                    break;
                case Qt::Key_PageDown:
                    this->setText("Page Down");
                    break;
                case Qt::Key_CapsLock:
                    this->setText("Caps Lock");
                    break;
                case Qt::Key_A:
                    this->setText("A");
                    break;
                case Qt::Key_S:
                    this->setText("S");
                    break;
                case Qt::Key_D:
                    this->setText("D");
                    break;
                case Qt::Key_F:
                    this->setText("F");
                    break;
                case Qt::Key_G:
                    this->setText("G");
                    break;
                case Qt::Key_H:
                    this->setText("H");
                    break;
                case Qt::Key_J:
                    this->setText("J");
                    break;
                case Qt::Key_K:
                    this->setText("K");
                    break;
                case Qt::Key_L:
                    this->setText("L");
                    break;
                case Qt::Key_Semicolon:
                    this->setText(";");
                    break;
                case Qt::Key_Apostrophe:
                    this->setText("'");
                    break;
                case Qt::Key_Return:
                    this->setText("Return"); // return is different than enter -- see docs
                    break;
                case Qt::Key_Z:
                    this->setText("Z");
                    break;
                case Qt::Key_X:
                    this->setText("X");
                    break;
                case Qt::Key_C:
                    this->setText("C");
                    break;
                case Qt::Key_V:
                    this->setText("V");
                    break;
                case Qt::Key_B:
                    this->setText("B");
                    break;
                case Qt::Key_N:
                    this->setText("N");
                    break;
                case Qt::Key_M:
                    this->setText("M");
                    break;
                case Qt::Key_Comma:
                    this->setText(",");
                    break;
                case Qt::Key_Period:
                    this->setText(".");
                    break;
                case Qt::Key_Backslash:
                    this->setText("\\");
                    break;
            }
            return true;
        }
    }
    // if object was key_button and we have focus on button and event type is key press
//    else if(testCheck == true && event->type() == QEvent::KeyPress)
//    {
//        //ui->key->setText(keyEvent->text());
//    }
    return false; // NOT A KEYEVENT... if (object == ui->main_keyboard && event->type() == QEvent::KeyPress)
}
