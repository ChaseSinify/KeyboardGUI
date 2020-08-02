#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "explorer.h"
#include "keyboard.h"
#include "key.h"
#include "drawgrid.h"
#include "pixel.h"
#include "selection.h"
#include "brush.h"
#include "windows.h"

#define EXTENDED_KEY_MASK   0x01000000 // read up more on these
#define LSHIFT_MASK         0x2a
#define RSHIFT_MASK         0x36 // was 0X36
#define PRTSCN_MASK         0x37 // Unused for now

static QMap<int, QString> keymap = {
    {1, "A"},
    {2, "B"},
    {3, "C"},
    {4, "D"},
    {5, "E"},
    {6, "F"},
    {7, "G"},
    {8, "H"},
    {9, "I"},
    {10, "J"},
    {11, "K"},
    {12, "L"},
    {13, "M"},
    {14, "N"},
    {15, "O"},
    {16, "P"},
    {17, "Q"},
    {18, "R"},
    {19, "S"},
    {20, "T"},
    {21, "U"},
    {22, "V"},
    {23, "W"},
    {24, "X"},
    {25, "Y"},
    {26, "Z"},
};
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set window mode
    this->set_window_mode();

    // hide devtools
    this->hide_devtools();
    ui->keyboard_outline->hide();

    // HIDING OUTDATED MODULES
    ui->main_keyboard->hide();
    ui->main_kb_toggle_line->hide();
    ui->toggle_kb->hide();
    ui->clear_binds->hide();
    ui->lock_KB->hide();
    ui->unlock_KB->hide();

    // wrap menubar & toolbar
    this->wrap_menubar();

    // install file explorer & dialogs
    this->fileExplorer = new Explorer(this);

    // file explorer signals -- these get passed to drawgrid
    connect(this->fileExplorer, SIGNAL(sendFilePath(int, QString, bool)), this, SLOT(loadImportLayout(int, QString, bool)));
    connect(this->fileExplorer, SIGNAL(sendSaveFilePath(int, QString, QString, bool)), this, SLOT(saveAsHelper(int, QString, QString, bool)));

    // install keyboard drop zone
    this->keyboard = new Keyboard(this);
    this->keyboard->resize(1000,360); // divisible by base key size each pls
    this->keyboard->move(415, 410); // this->keyboard->move(170,315);

    // this->keyboard->setStyleSheet("");
    this->keyboard->setFocusPolicy(Qt::NoFocus); // this is so our focus css doesnt target keyboard... idk a better method

    // install drawing grid UWU
    this->drawGrid = new DrawGrid(this);
    this->drawGrid->resize(1000, 360); // divisible by base key size each pls
    this->drawGrid->move(415, 40);

    // populate key selection box from keymap
    this->map_keys_for_selection();

    // lock special keys
    this->lock_special_keys();

    // install event filter
    ui->main_keyboard->installEventFilter(this); // consider adding global filter
}
// destructor
MainWindow::~MainWindow()
{
    delete ui;
}
// set window mode
void MainWindow::set_window_mode()
{
    // IMPORTANT: borderless window -- make transparent title bar for moving window | see: https://forum.qt.io/topic/34354/frameless-window-dragging-issue
    Qt::WindowFlags flags = this->windowFlags(); this->setWindowFlags(flags|Qt::FramelessWindowHint);
}
// dev tools
void MainWindow::hide_devtools()
{
    // hiding dev tools (remove these fully for final release)
    // left tools
}
// menubar & toolbar wrapper
void MainWindow::wrap_menubar()
{
    QMenuBar* menubar = ui->menuBar;
    QToolBar* toolbar = ui->toolBar;

    QWidget* wrapper = new QWidget();
    wrapper->setLayout(new QHBoxLayout());
    wrapper->layout()->setContentsMargins(0,0,0,0);
    wrapper->layout()->addWidget(menubar);
    wrapper->layout()->addWidget(toolbar);

    this->layout()->setMenuBar(wrapper);
}
// populate keys based on OS detected, or universally
void MainWindow::map_keys_for_selection()
{
    QMap<int, QString>::iterator it;
    for (it = keymap.begin(); it != keymap.end(); it++ )
    {
        ui->key_selection_CB->addItem(it.value());
    }
}
// lock special keys
void MainWindow::lock_special_keys()
{
    ui->_print_screen->setText("PS");
    ui->_print_screen->setDisabled(true);
    ui->_function->setText("Fn");
    ui->_function->setDisabled(true);
}
// event filter
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    // If the event was from main_keyboard and it was a keypress -- this also implies we have main keyboard focus
    if (object == ui->main_keyboard && event->type() == QEvent::KeyPress)
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
                    ui->_right_control->setText("rCtrl");
                else
                    ui->_left_control->setText("lCtrl");
                break;
            case VK_MENU:
                // VK_MENU = ALT virtual key
                if(extended)
                    ui->_right_alternate->setText("rAlt");
                else
                    ui->_left_alternate->setText("lAlt");
                break;
            case VK_SHIFT:
                if ((scode ^ LSHIFT_MASK) == 0) {
                    //vk = VK_LSHIFT;
                    ui->_left_shift->setText("lShift");
                } else if ((scode ^ RSHIFT_MASK) == 0) {
                    //vk = VK_RSHIFT;
                    ui->_right_shift->setText("rShift");
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
        /*else*/ if (keyEvent->key() == Qt::Key_Meta)
        {
            ui->_super->setText("Meta"); // Should apply to windows key
            return true;
        }
        // SPACE
        else if (keyEvent->key() == Qt::Key_Space)
        {
            ui->_spacebar->setText("Space");
            return true;
        }
        else // not a special case -- but still a key event [switches]
        {
            switch(keyEvent->key()) // Qt::Key Enum -- Need to expand this fully... try to find this done online
            {
                case Qt::Key_Escape:
                    ui->_escape->setText("Esc");
                    break;
                case Qt::Key_F1:
                    ui->_f1->setText("F1");
                    break;
                case Qt::Key_F2:
                    ui->_f2->setText("F2");
                    break;
                case Qt::Key_F3:
                    ui->_f3->setText("F3");
                    break;
                case Qt::Key_F4:
                    ui->_f4->setText("F4");
                    break;
                case Qt::Key_F5:
                    ui->_f5->setText("F5");
                    break;
                case Qt::Key_F6:
                    ui->_f6->setText("F6");
                    break;
                case Qt::Key_F7:
                    ui->_f7->setText("F7");
                    break;
                case Qt::Key_F8:
                    ui->_f8->setText("F8");
                    break;
                case Qt::Key_F9:
                    ui->_f9->setText("F9");
                    break;
                case Qt::Key_F10:
                    ui->_f10->setText("F10");
                    break;
                case Qt::Key_F11:
                    ui->_f11->setText("F11");
                    break;
                case Qt::Key_F12:
                    ui->_f12->setText("F12");
                    break;
                case Qt::Key_Print: // THIS DOESNT WORK ATM
                    ui->_print_screen->setText("Print Screen");
                    break;
                case Qt::Key_ScrollLock:
                    ui->_scroll_lock->setText("Scroll Lock");
                    break;
                case Qt::Key_Pause:
                    ui->_pause_break->setText("Pause Break");
                    break;
                case Qt::Key_QuoteLeft: // or maybe Qt::Key_Dead_Grave | GRAVE ACCENT WINDOWS
                    ui->_grave_accent->setText("`");
                    break;
                case Qt::Key_1:
                    ui->_1->setText("1");
                    break;
                case Qt::Key_2:
                    ui->_2->setText("2");
                    break;
                case Qt::Key_3:
                    ui->_3->setText("3");
                    break;
                case Qt::Key_4:
                    ui->_4->setText("4");
                    break;
                case Qt::Key_5:
                    ui->_5->setText("5");
                    break;
                case Qt::Key_6:
                    ui->_6->setText("6");
                    break;
                case Qt::Key_7:
                    ui->_7->setText("7");
                    break;
                case Qt::Key_8:
                    ui->_8->setText("8");
                    break;
                case Qt::Key_9:
                    ui->_9->setText("9");
                    break;
                case Qt::Key_0:
                    ui->_0->setText("0");
                    break;
                case Qt::Key_Minus:
                    ui->_minus->setText("-");
                    break;
                case Qt::Key_Equal:
                    ui->_equal->setText("=");
                    break;
                case Qt::Key_Backspace:
                    ui->_backspace->setText("Backspace");
                    break;
                case Qt::Key_Insert:
                    ui->_insert->setText("Insert");
                    break;
                case Qt::Key_Home:
                    ui->_home->setText("Home");
                    break;
                case Qt::Key_PageUp:
                    ui->_page_up->setText("Page Up");
                    break;
                case Qt::Key_Tab:
                    ui->_tab->setText("Tab");
                    break;
                case Qt::Key_Q:
                    ui->_q->setText("Q");
                    break;
                case Qt::Key_W:
                    ui->_w->setText("W");
                    break;
                case Qt::Key_E:
                    ui->_e->setText("E");
                    break;
                case Qt::Key_R:
                    ui->_r->setText("R");
                    break;
                case Qt::Key_T:
                    ui->_t->setText("T");
                    break;
                case Qt::Key_Y:
                    ui->_y->setText("Y");
                    break;
                case Qt::Key_U:
                    ui->_u->setText("U");
                    break;
                case Qt::Key_I:
                    ui->_i->setText("I");
                    break;
                case Qt::Key_O:
                    ui->_o->setText("O");
                    break;
                case Qt::Key_P:
                    ui->_p->setText("P");
                    break;
                case Qt::Key_BracketLeft:
                    ui->_left_bracket->setText("[");
                    break;
                case Qt::Key_BracketRight:
                    ui->_right_bracket->setText("]");
                    break;
                case Qt::Key_Slash: // this is forward slash
                    ui->_forward_slash->setText("/"); // escape zzz -- find literal for non-ambigious inputs
                    break;
                case Qt::Key_Delete:
                    ui->_delete->setText("Delete");
                    break;
                case Qt::Key_End:
                    ui->_end->setText("End");
                    break;
                case Qt::Key_PageDown:
                    ui->_page_down->setText("Page Down");
                    break;
                case Qt::Key_CapsLock:
                    ui->_caps_lock->setText("Caps Lock");
                    break;
                case Qt::Key_A:
                    ui->_a->setText("A");
                    break;
                case Qt::Key_S:
                    ui->_s->setText("S");
                    break;
                case Qt::Key_D:
                    ui->_d->setText("D");
                    break;
                case Qt::Key_F:
                    ui->_f->setText("F");
                    break;
                case Qt::Key_G:
                    ui->_g->setText("G");
                    break;
                case Qt::Key_H:
                    ui->_h->setText("H");
                    break;
                case Qt::Key_J:
                    ui->_j->setText("J");
                    break;
                case Qt::Key_K:
                    ui->_k->setText("K");
                    break;
                case Qt::Key_L:
                    ui->_l->setText("L");
                    break;
                case Qt::Key_Semicolon:
                    ui->_semicolon->setText(";");
                    break;
                case Qt::Key_Apostrophe:
                    ui->_apostrophe->setText("'");
                    break;
                case Qt::Key_Return:
                    ui->_return->setText("Return"); // return is different than enter -- see docs
                    break;
                case Qt::Key_Z:
                    ui->_z->setText("Z");
                    break;
                case Qt::Key_X:
                    ui->_x->setText("X");
                    break;
                case Qt::Key_C:
                    ui->_c->setText("C");
                    break;
                case Qt::Key_V:
                    ui->_v->setText("V");
                    break;
                case Qt::Key_B:
                    ui->_b->setText("B");
                    break;
                case Qt::Key_N:
                    ui->_n->setText("N");
                    break;
                case Qt::Key_M:
                    ui->_m->setText("M");
                    break;
                case Qt::Key_Comma:
                    ui->_comma->setText(",");
                    break;
                case Qt::Key_Period:
                    ui->_period->setText(".");
                    break;
                case Qt::Key_Backslash:
                    ui->_backslash->setText("\\");
                    break;
                case Qt::Key_Super_L: // this might need to check WIN for windows ...
                    ui->_super->setText("Super");
                    break;
                case Qt::Key_Menu:
                    ui->_menu->setText("Menu");
                    break;
                case Qt::Key_Up:
                    ui->_up->setText("^");
                    break;
                case Qt::Key_Left:
                    ui->_left->setText("<");
                    break;
                case Qt::Key_Down:
                    ui->_down->setText("v");
                    break;
                case Qt::Key_Right:
                    ui->_right->setText(">");
                    break;
            }
            return true;
        }
    }
    return false; // NOT A KEYEVENT... if (object == ui->main_keyboard && event->type() == QEvent::KeyPress)
}
// mouse press event --> MainWindow
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mouseStart = mapFromGlobal(QCursor::pos()); // get the local position of the event

    // testing click locations
    if(event->button() == Qt::LeftButton && ui->toolBar->geometry().contains(mouseStart)) // if the event was inside the toolbar IMPORTANT[accepted positions of x: X: 342 > end of toolbar]
        isMouseDownInToolbarSpace = true;
}
// mouse release --> MainWindow
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && isMouseDownInToolbarSpace == true) // if we had an LMB release within the toolbar space
        isMouseDownInToolbarSpace = false;
}
// mouse move --> MainWindow
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(isMouseDownInToolbarSpace == true)
    {
        globalMouse = QCursor::pos();
        move(globalMouse-mouseStart); // move to the current global position - the starting local position within the window
    }
}
// lock_key_checkbox statechange monitoring
void MainWindow::on_lock_key_checkbox_stateChanged(int arg1)
{
    if(arg1 == 0) // The item is unchecked.
    {
        // unlock the menubar now
        ui->menuBar->setEnabled(true);
        ui->main_keyboard->clearFocus();
    }
    else if(arg1 ==2) // The item is checked.
    {
        // lock the menu to prevent alt focus passing
        ui->menuBar->setDisabled(true);
        ui->main_keyboard->setFocus();
    }
}
// clear all binds
void MainWindow::on_clear_binds_clicked()
{
    // clear all the keys to no text -- see: https://stackoverflow.com/questions/32109156/how-to-iterate-over-all-widgets-in-a-window
    QList<QPushButton *> keys = ui->main_keyboard->findChildren<QPushButton *>();
    for(QList<QPushButton *>::iterator it = keys.begin(); it != keys.end(); ++it)
    {
        // ignore our static keys for now
        if ((*it)->objectName() == "_function" || (*it)->objectName() == "_print_screen")
            continue;
        else
            (*it)->setText("");
    }
    ui->lock_key_checkbox->setChecked(false);

    // if we want to instantly be typing.. from user perspective, probs makes sense to clear > uncheck lock
    //ui->main_keyboard->setFocus();
}
// unlock kb --> focus
void MainWindow::on_unlock_KB_clicked()
{
    ui->lock_key_checkbox->setChecked(false);
}
// lock kb --> focus
void MainWindow::on_lock_KB_clicked()
{
    ui->lock_key_checkbox->setChecked(true);
}
// toggle tools
void MainWindow::on_toggle_tools_clicked()
{
    // tool kit 1
    if(ui->tool_kit->isVisible() == true)
        ui->tool_kit->setVisible(false);
    else if(ui->tool_kit->isVisible() == false)
        ui->tool_kit->setVisible(true);

    // tool kit 2
    if(ui->tool_kit_2->isVisible() == true)
        ui->tool_kit_2->setVisible(false);
    else if(ui->tool_kit_2->isVisible() == false)
        ui->tool_kit_2->setVisible(true);
}
// basic quit
void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}
// spawning keys
void MainWindow::on_key_selection_CB_currentTextChanged(const QString &arg1)
{
    if(spawnKey == true) // cheap hack ...
    {
        this->keyboard->modelKey(QRect(0,0,40,40), arg1);
    }
    else
    {
        spawnKey = true;
    }
}
// clearing all key drag widgets -- IMPORTANT: THIS WORKS BUT MIGHT NOT BE SAFE POINTERS -- NEED MORE RESEARCH
// this is like 500ms full clear... acceptable but not blazing fast... work on it
void MainWindow::on_clear_layout_PB_clicked()
{
    this->drawGrid->clear();
}
// this change sets the current key_color key for the brush in layout editor
void MainWindow::on_layout_color_CB_currentIndexChanged(int index)
{
    switch(index){
        case 0: // border -- black
        {
            ui->key_color_brush->setText("BORDER");
            this->drawGrid->setBrushColor("");
            break;
        }
        case 1: // a -- red
        {
            ui->key_color_brush->setText("A");
            this->drawGrid->setBrushColor("#ff0000");
            break;
        }
        case 2: // b -- green
        {
            ui->key_color_brush->setText("B");
            this->drawGrid->setBrushColor("#00ff00");
            break;
        }
        case 3: // c -- blue
        {
            ui->key_color_brush->setText("C");
            this->drawGrid->setBrushColor("#0000ff");
            break;
        }
    }
}
void MainWindow::on_selection_checkbox_stateChanged(int arg1)
{
    this->drawGrid->toggleSelectionTool();
    this->keyboard->toggleSelectionTool();
}
void MainWindow::on_eraser_checkbox_stateChanged(int arg1)
{
    this->drawGrid->toggleEraserTool();
    this->keyboard->toggleEraserTool();
}
// save
void MainWindow::on_actionSave_triggered()
{
    this->fileExplorer->show();
}
// save as
void MainWindow::on_actionSave_As_triggered()
{
    this->fileExplorer->show();
}
// import
void MainWindow::on_actionImport_triggered()
{
    this->fileExplorer->show();
}
// export
void MainWindow::on_actionExport_triggered()
{
    this->fileExplorer->show();
}
// importing layouts
void MainWindow::loadImportLayout(int eventType, QString filePath, bool saveToMyLayouts)
{
    this->drawGrid->renderImportedLayout(eventType, filePath, saveToMyLayouts); // testing grid output
}
// dynamically add to menubar with users saved layouts
void MainWindow::addToLayoutMenu(QAction *action)
{
    QVariant fileNameQVariant = action->data();
    QString fileNameQString = fileNameQVariant.toString();
    qDebug() << "IMPORTANT: "<< this->userLayoutsMap.value(fileNameQString);
    this->loadImportLayout(1, this->userLayoutsMap.value(fileNameQString), false);
    qDebug() << "QV: " << fileNameQVariant << "QS: " << fileNameQString; // output: QVariant(QString, "rehghjtykjtk.sock")
}
// save layout as
void MainWindow::saveAsHelper(int eventType, QString filePath, QString fileName, bool saveToMyLayouts)
{
    if(saveToMyLayouts == true)
    {
        // update user's layout map so we can load these later when they start the program
        this->userLayoutsMap.insert(fileName, (filePath + fileName));

        QAction *importAction = new QAction(fileName, this);
        importAction->setCheckable(false);
        importAction->setChecked(false);
        importAction->setData(fileName);
        ui->menuMy_Layouts->addAction(importAction);

        connect( ui->menuMy_Layouts, SIGNAL( triggered( QAction * ) ), this, SLOT( addToLayoutMenu( QAction * ) ) );

//        if ( parentItem->hasChildren() )  // create sub menus for each child
//        {
//            // pseudocode
//            for each child Item in parentItem
//            {
//                QMenu * childMenu = parentMenu->addMenu( childItem->productName() );
//                buildProductMenu( childMenu, childItem );
//            }
//        }

    }
    qDebug() << eventType << filePath << fileName << saveToMyLayouts;
    QString serializedLayout = this->drawGrid->exportLayout();
    qDebug() << serializedLayout;

    QString savePath = filePath;
    QDir dir(savePath);
    if (!dir.exists(savePath + fileName))
    {
        QFile layoutFile(savePath + fileName);
        if (layoutFile.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            qDebug()<< "file now exists";
            QTextStream stream(&layoutFile);
            stream << serializedLayout;
            layoutFile.close();
        }
        else
            qDebug() << "error making file";
    }
    else
        qDebug() << "overwrite???";
}
void MainWindow::on_actionConfigure_Home_Path_triggered()
{
    // this should open explorer, let user choose dir, then load in all .sock files, ideally just the name and not extensions, but w/e
}
void MainWindow::on_toggle_drawing_board_clicked()
{
    if(this->drawGrid->isVisible())
    {
        this->drawGrid->hide();
        ui->drawgrid_outline->hide();
    }
    else
    {
        this->drawGrid->show();
        ui->drawgrid_outline->show();
    }
}
void MainWindow::on_toggle_layout_board_clicked()
{
    if(this->keyboard->isVisible())
    {
        this->keyboard->hide();
    }
    else
    {
        this->keyboard->show();
    }
}
/*
 * this should call QList<QRect> DrawGrid::getKeyForModeling()
 * and a modified version of void Keyboard::addKeyToGrid(DragWidget* key)
 * which would simply do what Keyboard::dropEvent does and call the
 * static CreateDragLabel (or a modified version) to simply create and move
 * a key to the QRect position given by QList<QRect> DrawGrid::getKeyForModeling()
 */
/*
 * Additionally, we need to have dropzone return a map of all current positions so we don't
 * overwrite any that already exist when compiling add adding -- perhaps just ignore those
 * that already exist and add the others
 */
// load model keys
void MainWindow::on_load_model_keys_clicked() // SPEED TESTS HERE @@@
{
    QList<QRect> modelKeyRectangles = this->drawGrid->getKeysForModeling(); // this accurately stores n modeled keys from the drawgrid as QRect positions/size values --> pass to keyboard
    // create the key based on the drawing geometry
    if(modelKeyRectangles.length() > 0)
    {
        for(int i = 0; i < modelKeyRectangles.length(); i++)
        {
            // if the keyboard is at max capacity
            if (this->keyboard->isFull())
            {
                ui->load_model_keys->setDisabled(true); // we will need a condition to re-enable this, as well as possibly a tracker for how many spaces are left
                break; // need to pop the modelkeys once we generate one so we dont overlap with one that we already made
            }
            else
            {
                this->keyboard->modelKey(modelKeyRectangles[i]);
            }
        }
        modelKeyRectangles.clear(); // erase after we are done modeling
    }

    this->drawGrid->clear(); // OPTIONAL: once we are done mapping, clear the drawing grid
    if(this->keyboard->isFull())
    {
        ui->load_model_keys->setDisabled(true);
        ui->load_model_keys->setStyleSheet("Background: grey; color: white; border: 1px solid white;");
        ui->load_model_keys->setText("Model is full!");
        return;
    }
}

//TESTING keyboard layout export format
void MainWindow::on_scan_kb_pb_clicked()
{
    QString data = this->keyboard->exportKeyboard();
    qDebug() << data;
}
