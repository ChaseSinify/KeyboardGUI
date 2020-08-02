#ifndef EXPLORER_H
#define EXPLORER_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>

namespace Ui {
class Explorer;
}

class Explorer : public QDialog
{
    Q_OBJECT

public:
    explicit Explorer(QWidget *parent = nullptr);
    ~Explorer();

signals:
    void sendFilePath(int eventType, QString filePath, bool saveToMyLayouts = false);
    void sendSaveFilePath(int eventType, QString filePath, QString fileName, bool saveToMyLayouts = false);

private slots:

    void on_explorer_directoryViewer_clicked(const QModelIndex &index);

    void on_explorer_fileViewer_doubleClicked(const QModelIndex &index);

    void on_explorer_fileViewer_clicked(const QModelIndex &index);

    void on_explorer_import_button_clicked();

    void on_explorer_export_button_clicked();

    void on_explorer_cancel_button_clicked();

    void on_explorer_save_button_clicked();

    void on_explorer_saveas_button_clicked();

    void on_add_to_my_layouts_checkbox_stateChanged(int arg1);

    void on_Explorer_finished(int result);

private:
    Ui::Explorer *ui;

    QFileSystemModel *dirModel;
    QFileSystemModel *fileModel;

    QString singleClickedDirectoryPath;
    QString singleClickedFilePath;
    QString filePath;
    QString savePath;
    QString fileName;

    bool saveToMyLayouts = false;
};

#endif // EXPLORER_H
