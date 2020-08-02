#include <QtWidgets>
#include "dragwidget.h"
#include "keyboard.h"
#include "mainwindow.h"

/*
 * CREATE A BETTER AUTO-LAYOUT FOR WHEN A NEW KEY IS CREATED
 * ADD FOCUS ON KEYS AND ALLOW ARROW KEY MOVING AND ADJUSTING
 *
 * TAKE IN AN INT REPRESENTING OBJECT NAME FOR EACH KEY MADE
 * SO WE CAN IDENTIFY AND DELETE SPECIFIC KEYS
 *
 * WE ARE USING MATH TO POSITION BASED OFF OF TOP LEFT CORNER
 * AS IS, SO WE MIGHT AS WELL POPULATE QRect 's BASED ON THOSE
 * COORDINATES AND DELETE BASED ON QRect POSITIONS
 */

//extern int keyId;
//extern void incrementKeyId();

// creates the actual key that gets displayed
static QLabel *createDragLabel(QWidget *parent, const QString &keyText, int styleId, QPoint position, QSize keySize, QString styleSheet)
{
    // create the base label
    QLabel *label = new QLabel(keyText, parent);
    label->setObjectName(QString::number(styleId)); // how to refer to key simply

    // style
    label->setAutoFillBackground(true);
    label->setFrameShape(QFrame::Panel);
    label->setFrameShadow(QFrame::Raised);
    label->setStyleSheet(styleSheet);

    // sizing
    label->resize(keySize);

    return label;
}

static QString hotSpotMimeDataKey() { return QStringLiteral("application/x-hotspot"); }

DragWidget::DragWidget(QWidget *parent, QString keyText, int styleId, QPoint position, QSize keySize, QString styleSheet)
    : QWidget(parent)
{
    // initialize internal members -- might need parent ... ?
    this->keyText = keyText;
    this->styleId = styleId;
    this->position = position;
    this->keySize = keySize;
    this->styleSheet = "background: black; color: white; font: 14px 'arial black'; border: 1px solid white; border-style: inset;"; //border-style: outset;"; // styleSheet;

    // create label
    QLabel *key = createDragLabel(this, keyText.toUpper(), styleId, position, keySize, this->styleSheet);

    // update values
     // -- might not need this ..
    key->show();
    key->setAttribute(Qt::WA_DeleteOnClose);

    // basic and likely to change info
    setAcceptDrops(false); // changed from true
    setMinimumSize(40, 40);
    setBaseSize(40, 40);

    // add key to keysMap
//    MainWindow::addStyleToStyledKeys(styleId, this);

//    // increment our style id
//    MainWindow::incrementStyleId();

    //TESTING
    this->setFocusPolicy(Qt::StrongFocus);
}

void DragWidget::dragEnterEvent(QDragEnterEvent *event)
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

// dont need this for now since we are not accepting drops
//void DragWidget::dropEvent(QDropEvent *event)
//{
//    if (event->mimeData()->hasText()) {
//        const QMimeData *mime = event->mimeData();
//        QStringList pieces = mime->text().split(QRegularExpression(QStringLiteral("\\s+")),
//                             QString::SkipEmptyParts);
//        QPoint position = event->pos();
//        // grid snap --> % 10 aka .25 key size
////        position.setX((position.x() - (position.x() % 10) ? (position.x() % 10 <= 5) : (position.x() - (position.x() % 10 + 10))));
////        position.setY((position.y() - (position.y() % 10) ? (position.y() % 10 <= 5) : (position.y() - (position.y() % 10 + 10))));
//        QPoint hotSpot;

//        QByteArrayList hotSpotPos = mime->data(hotSpotMimeDataKey()).split(' ');
//        if (hotSpotPos.size() == 2) {
//            hotSpot.setX(hotSpotPos.first().toInt());
//            hotSpot.setY(hotSpotPos.last().toInt());
//        }

//        for (const QString &piece : pieces) {
//            QLabel *newLabel = createDragLabel(this, piece, 1, position, {40,40}, "border: 1px solid white;");
//            newLabel->move((position - hotSpot)); // newLabel->move(position - hotSpot);
//            newLabel->show();
//            newLabel->setAttribute(Qt::WA_DeleteOnClose);

//            position += QPoint(newLabel->width(), 0);
//        }

//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }
//    } else {
//        event->ignore();
//    }
//    for (QWidget *widget : findChildren<QWidget *>()) {
//        if (!widget->isVisible())
//            widget->deleteLater();
//    }
//}

void DragWidget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = qobject_cast<QLabel*>(childAt(event->pos()));
    if (!child)
        return;

    QPoint hotSpot = event->pos() - child->pos();

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(child->text());
    mimeData->setData(hotSpotMimeDataKey(),
                      QByteArray::number(hotSpot.x()) + ' ' + QByteArray::number(hotSpot.y()));

    qreal dpr = window()->windowHandle()->devicePixelRatio();
    QPixmap pixmap(child->size() * dpr);
    pixmap.setDevicePixelRatio(dpr);
    child->render(&pixmap);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(hotSpot);

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);//, Qt::CopyAction); // | Qt::MoveAction, Qt::CopyAction); // was using just copyaction personally... testing move for now

    if (dropAction == Qt::MoveAction)
        child->close();
}

//TESTING
void DragWidget::mouseReleaseEvent(QMouseEvent *event)
{
    this->setFocus(); // grabkeyboard? probs not but might test this
}

void DragWidget::setKeyText(QString text)
{
    this->keyText = text;
}

//TESTING
#define EXTENDED_KEY_MASK   0x01000000 // read up more on these
#define LSHIFT_MASK         0x2a
#define RSHIFT_MASK         0x36 // was 0X36
#define PRTSCN_MASK         0x37 // Unused for now
// event filter
bool DragWidget::eventFilter(QObject *object, QEvent *event)
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
                    this->keyText = "rCtrl"; // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                else
                    this->keyText = "lCtrl";
                break;
//            case VK_MENU:
//                // VK_MENU = ALT virtual key
//                if(extended)
//                    ui->_right_alternate->setText("rAlt");
//                else
//                    ui->_left_alternate->setText("lAlt");
//                break;
//            case VK_SHIFT:
//                if ((scode ^ LSHIFT_MASK) == 0) {
//                    //vk = VK_LSHIFT;
//                    ui->_left_shift->setText("lShift");
//                } else if ((scode ^ RSHIFT_MASK) == 0) {
//                    //vk = VK_RSHIFT;
//                    ui->_right_shift->setText("rShift");
//                }
//                break;
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
            this->keyText = "Meta"; // Should apply to windows key
            return true;
        }
//        // SPACE
//        else if (keyEvent->key() == Qt::Key_Space)
//        {
//            ui->_spacebar->setText("Space");
//            return true;
//        }
        else // not a special case -- but still a key event [switches]
        {
            switch(keyEvent->key()) // Qt::Key Enum -- Need to expand this fully... try to find this done online
            {
                case Qt::Key_Escape:
                    this->keyText = "Esc";
                    break;
//                case Qt::Key_F1:
//                    ui->_f1->setText("F1");
//                    break;
//                case Qt::Key_F2:
//                    ui->_f2->setText("F2");
//                    break;
//                case Qt::Key_F3:
//                    ui->_f3->setText("F3");
//                    break;
//                case Qt::Key_F4:
//                    ui->_f4->setText("F4");
//                    break;
//                case Qt::Key_F5:
//                    ui->_f5->setText("F5");
//                    break;
//                case Qt::Key_F6:
//                    ui->_f6->setText("F6");
//                    break;
//                case Qt::Key_F7:
//                    ui->_f7->setText("F7");
//                    break;
//                case Qt::Key_F8:
//                    ui->_f8->setText("F8");
//                    break;
//                case Qt::Key_F9:
//                    ui->_f9->setText("F9");
//                    break;
//                case Qt::Key_F10:
//                    ui->_f10->setText("F10");
//                    break;
//                case Qt::Key_F11:
//                    ui->_f11->setText("F11");
//                    break;
//                case Qt::Key_F12:
//                    ui->_f12->setText("F12");
//                    break;
//                case Qt::Key_Print: // THIS DOESNT WORK ATM
//                    ui->_print_screen->setText("Print Screen");
//                    break;
//                case Qt::Key_ScrollLock:
//                    ui->_scroll_lock->setText("Scroll Lock");
//                    break;
//                case Qt::Key_Pause:
//                    ui->_pause_break->setText("Pause Break");
//                    break;
//                case Qt::Key_QuoteLeft: // or maybe Qt::Key_Dead_Grave | GRAVE ACCENT WINDOWS
//                    ui->_grave_accent->setText("`");
//                    break;
//                case Qt::Key_1:
//                    ui->_1->setText("1");
//                    break;
//                case Qt::Key_2:
//                    ui->_2->setText("2");
//                    break;
//                case Qt::Key_3:
//                    ui->_3->setText("3");
//                    break;
//                case Qt::Key_4:
//                    ui->_4->setText("4");
//                    break;
//                case Qt::Key_5:
//                    ui->_5->setText("5");
//                    break;
//                case Qt::Key_6:
//                    ui->_6->setText("6");
//                    break;
//                case Qt::Key_7:
//                    ui->_7->setText("7");
//                    break;
//                case Qt::Key_8:
//                    ui->_8->setText("8");
//                    break;
//                case Qt::Key_9:
//                    ui->_9->setText("9");
//                    break;
//                case Qt::Key_0:
//                    ui->_0->setText("0");
//                    break;
//                case Qt::Key_Minus:
//                    ui->_minus->setText("-");
//                    break;
//                case Qt::Key_Equal:
//                    ui->_equal->setText("=");
//                    break;
//                case Qt::Key_Backspace:
//                    ui->_backspace->setText("Backspace");
//                    break;
//                case Qt::Key_Insert:
//                    ui->_insert->setText("Insert");
//                    break;
//                case Qt::Key_Home:
//                    ui->_home->setText("Home");
//                    break;
//                case Qt::Key_PageUp:
//                    ui->_page_up->setText("Page Up");
//                    break;
//                case Qt::Key_Tab:
//                    ui->_tab->setText("Tab");
//                    break;
//                case Qt::Key_Q:
//                    ui->_q->setText("Q");
//                    break;
//                case Qt::Key_W:
//                    ui->_w->setText("W");
//                    break;
//                case Qt::Key_E:
//                    ui->_e->setText("E");
//                    break;
//                case Qt::Key_R:
//                    ui->_r->setText("R");
//                    break;
//                case Qt::Key_T:
//                    ui->_t->setText("T");
//                    break;
//                case Qt::Key_Y:
//                    ui->_y->setText("Y");
//                    break;
//                case Qt::Key_U:
//                    ui->_u->setText("U");
//                    break;
//                case Qt::Key_I:
//                    ui->_i->setText("I");
//                    break;
//                case Qt::Key_O:
//                    ui->_o->setText("O");
//                    break;
//                case Qt::Key_P:
//                    ui->_p->setText("P");
//                    break;
//                case Qt::Key_BracketLeft:
//                    ui->_left_bracket->setText("[");
//                    break;
//                case Qt::Key_BracketRight:
//                    ui->_right_bracket->setText("]");
//                    break;
//                case Qt::Key_Slash: // this is forward slash
//                    ui->_forward_slash->setText("/"); // escape zzz -- find literal for non-ambigious inputs
//                    break;
//                case Qt::Key_Delete:
//                    ui->_delete->setText("Delete");
//                    break;
//                case Qt::Key_End:
//                    ui->_end->setText("End");
//                    break;
//                case Qt::Key_PageDown:
//                    ui->_page_down->setText("Page Down");
//                    break;
//                case Qt::Key_CapsLock:
//                    ui->_caps_lock->setText("Caps Lock");
//                    break;
//                case Qt::Key_A:
//                    ui->_a->setText("A");
//                    break;
//                case Qt::Key_S:
//                    ui->_s->setText("S");
//                    break;
//                case Qt::Key_D:
//                    ui->_d->setText("D");
//                    break;
//                case Qt::Key_F:
//                    ui->_f->setText("F");
//                    break;
//                case Qt::Key_G:
//                    ui->_g->setText("G");
//                    break;
//                case Qt::Key_H:
//                    ui->_h->setText("H");
//                    break;
//                case Qt::Key_J:
//                    ui->_j->setText("J");
//                    break;
//                case Qt::Key_K:
//                    ui->_k->setText("K");
//                    break;
//                case Qt::Key_L:
//                    ui->_l->setText("L");
//                    break;
//                case Qt::Key_Semicolon:
//                    ui->_semicolon->setText(";");
//                    break;
//                case Qt::Key_Apostrophe:
//                    ui->_apostrophe->setText("'");
//                    break;
//                case Qt::Key_Return:
//                    ui->_return->setText("Return"); // return is different than enter -- see docs
//                    break;
//                // BOTH SHIFTS MAP TO SAME KEY ??????????????????
//                //case Qt::Key_Shift: // see: https://stackoverflow.com/questions/44813630/detecting-the-right-shift-key-in-qt | is this Rshift, Lshift, Both?
//                    //ui->_left_shift->setText("Shift"); // testing left first, so make sure to only hit left shift @@@
//                    //break;
//                case Qt::Key_Z:
//                    ui->_z->setText("Z");
//                    break;
//                case Qt::Key_X:
//                    ui->_x->setText("X");
//                    break;
//                case Qt::Key_C:
//                    ui->_c->setText("C");
//                    break;
//                case Qt::Key_V:
//                    ui->_v->setText("V");
//                    break;
//                case Qt::Key_B:
//                    ui->_b->setText("B");
//                    break;
//                case Qt::Key_N:
//                    ui->_n->setText("N");
//                    break;
//                case Qt::Key_M:
//                    ui->_m->setText("M");
//                    break;
//                case Qt::Key_Comma:
//                    ui->_comma->setText(",");
//                    break;
//                case Qt::Key_Period:
//                    ui->_period->setText(".");
//                    break;
//                case Qt::Key_Backslash:
//                    ui->_backslash->setText("\\");
//                    break;
//                //case Qt::Key_Shift: see: https://stackoverflow.com/questions/44813630/detecting-the-right-shift-key-in-qt
//                    //ui->_escape->setText("Esc");
//                    //break;
//                case Qt::Key_Control: // see: https://stackoverflow.com/questions/44813630/detecting-the-right-shift-key-in-qt
//                    ui->_escape->setText("Esc"); // testing this as left control first, only hit left @@@
//                    break;
//                case Qt::Key_Super_L: // this might need to check WIN for windows ...
//                    ui->_super->setText("Super");
//                    break;
//                //BOTH ALTS MAP TO SAME KEY HERE ??????????????
//                //case Qt::Key_Alt: // see: https://stackoverflow.com/questions/44813630/detecting-the-right-shift-key-in-qt
//                    //ui->_left_alternate->setText("Alt"); // testing this as left alt first, only hit left @@@
//                    //break;
//                //case Qt::Key_Space:
//                    //ui->_spacebar->setText("Spacebar");
//                    //break;
//                //case Qt::Key_Alt: see: https://stackoverflow.com/questions/44813630/detecting-the-right-shift-key-in-qt
//                    //ui->_escape->setText("Esc");
//                    //break;
//                //case Qt::Key_Fn: // NEED THIS KEY NAME FOR FUNCTION
//                    //ui->_escape->setText("Esc");
//                    //break;
//                case Qt::Key_Menu:
//                    ui->_menu->setText("Menu");
//                    break;
//                //case Qt::Key_Control: see: https://stackoverflow.com/questions/44813630/detecting-the-right-shift-key-in-qt
//                    //ui->_escape->setText("Esc");
//                    //break;
//                case Qt::Key_Up:
//                    ui->_up->setText("^");
//                    break;
//                case Qt::Key_Left:
//                    ui->_left->setText("<");
//                    break;
//                case Qt::Key_Down:
//                    ui->_down->setText("v");
//                    break;
//                case Qt::Key_Right:
//                    ui->_right->setText(">");
//                    break;
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
