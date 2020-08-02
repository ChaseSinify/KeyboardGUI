#include "explorer.h"
#include "ui_explorer.h"

Explorer::Explorer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Explorer)
{
    ui->setupUi(this);

    // LOW-ISH PRIORITY: LET THE USER SELECT THEIR OWN BASE DIRECTORY AND WHERE THEY SAVE
    QStringList filters;
    filters << "*.sock";
    QString sPath = "C:/";

    // dirmodel
    this->dirModel  = new QFileSystemModel(this);
    QModelIndex index = dirModel->index(sPath, 0);

    // dirmodel config
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirModel->setRootPath(sPath);

    ui->explorer_directoryViewer->setRootIndex(index);
    ui->explorer_directoryViewer->setModel(dirModel);

    ui->explorer_directoryViewer->setColumnWidth(0, 300);
    ui->explorer_directoryViewer->hideColumn(1);
    ui->explorer_directoryViewer->hideColumn(3);

    // file model
    this->fileModel = new QFileSystemModel(this);

    // file model config
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    fileModel->setRootPath(sPath);
    fileModel->setNameFilters(filters);
    fileModel->setNameFilterDisables(false);

    ui->explorer_fileViewer->setModel(fileModel);
    index = fileModel->index(sPath, 0);
    ui->explorer_fileViewer->setRootIndex(index);

}

Explorer::~Explorer()
{
    delete ui;
}


void Explorer::on_explorer_directoryViewer_clicked(const QModelIndex &index)
{
    singleClickedDirectoryPath = dirModel->fileInfo(index).absoluteFilePath();
    ui->explorer_fileViewer->setRootIndex(fileModel->setRootPath(singleClickedDirectoryPath));
}

// double click import -- int eventType = 1
void Explorer::on_explorer_fileViewer_doubleClicked(const QModelIndex &index)
{
    // if file ends with .sock
    filePath = fileModel->fileInfo(index).absoluteFilePath();
    if(filePath.endsWith(".sock"))
    {
        emit sendFilePath(1, filePath, saveToMyLayouts); // there should be a check box for this, also an option on mainwindow ui to add this later
    }
}

// single click selection for file viewer to use targetted click file with other buttons
void Explorer::on_explorer_fileViewer_clicked(const QModelIndex &index)
{
    // highlight, let user choose what to do
    this->singleClickedFilePath = fileModel->fileInfo(index).absoluteFilePath();
}

// import clicked
void Explorer::on_explorer_import_button_clicked()
{
    // get the file that was highlighted by single click in list view
}

// export clicked
void Explorer::on_explorer_export_button_clicked()
{
    // export the file -- same as save as??
}

// cancel clicked
void Explorer::on_explorer_cancel_button_clicked()
{
    this->close();
}

// save clicked
void Explorer::on_explorer_save_button_clicked()
{
    // if we are working with a file that already has a save directory
        // check the hash
        // if same do nothing
        // else overwrite
    // save with deafult name, which is shown in text box, or whatever user entered
    // generate hash of save file for comparisons later
}

// save as clicked
void Explorer::on_explorer_saveas_button_clicked()
{
    savePath = singleClickedDirectoryPath + "/";
    fileName = ui->explorer_saveas_lineedit->text() + ".sock";
    qDebug() << "Save Path: " + savePath;

    emit sendSaveFilePath(2, savePath, fileName, saveToMyLayouts);
    // pass focus to name box, on enter, or this clicked, mkdir in location named whatever was entered
    // prompt to confirm overwrite as needed
}

void Explorer::on_add_to_my_layouts_checkbox_stateChanged(int arg1)
{
    if(arg1 == 2)
        this->saveToMyLayouts = true;
    else
        this->saveToMyLayouts = false;
    qDebug() << this->saveToMyLayouts;
}

void Explorer::on_Explorer_finished(int result)
{
    if (ui->add_to_my_layouts_checkbox->checkState() == 2)
        ui->add_to_my_layouts_checkbox->toggle();
}
