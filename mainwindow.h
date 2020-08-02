#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "explorer.h"
#include "keyboard.h"
#include "drawgrid.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:

private slots:
    void set_window_mode();
    void hide_devtools();
    void wrap_menubar();
    void map_keys_for_selection();
    void lock_special_keys();
    bool eventFilter(QObject *object, QEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void on_lock_key_checkbox_stateChanged(int arg1);
    void on_clear_binds_clicked();
    void on_unlock_KB_clicked();
    void on_lock_KB_clicked();
    void on_toggle_tools_clicked();
    void on_actionQuit_triggered();
    void on_key_selection_CB_currentTextChanged(const QString &arg1);
    void on_clear_layout_PB_clicked();
    void on_layout_color_CB_currentIndexChanged(int index);
    void on_selection_checkbox_stateChanged(int arg1);
    void on_eraser_checkbox_stateChanged(int arg1);
    void on_actionSave_As_triggered();
    void on_actionSave_triggered();
    void on_actionImport_triggered();
    void on_actionExport_triggered();
    // Explorer signals   
    void loadImportLayout(int eventType, QString filePath, bool saveToMyLayouts = false);
    void saveAsHelper(int eventType, QString savePath, QString fileName, bool saveToMyLayouts = false);
    void addToLayoutMenu(QAction *action);
    void on_actionConfigure_Home_Path_triggered();
    void on_toggle_drawing_board_clicked();
    void on_toggle_layout_board_clicked();
    void on_load_model_keys_clicked();
    //void on_toggle_kb_clicked();

    void on_scan_kb_pb_clicked();

private:
    Ui::MainWindow *ui;
    Explorer *fileExplorer;
    Keyboard *keyboard;
    DrawGrid *drawGrid;

    bool isMouseDownInToolbarSpace = false;
    bool spawnKey = false;
    QPoint globalMouse;
    QPoint mouseStart;
    QMap<QString, QString> userLayoutsMap; // fileName, filePath
    int runningKeyId = 0;
};
#endif // MAINWINDOW_H
