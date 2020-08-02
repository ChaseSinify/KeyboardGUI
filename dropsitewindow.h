
#ifndef DROPSITEWINDOW_H
#define DROPSITEWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QMimeData;
class QPushButton;
class QTableWidget;
QT_END_NAMESPACE
class DropArea;

//! [DropSiteWindow header]
class DropSiteWindow : public QWidget
{
    Q_OBJECT

public:
    DropSiteWindow();

public slots:
    void updateFormatsTable(const QMimeData *mimeData);

private:
    DropArea *dropArea;
    QLabel *abstractLabel;
    QTableWidget *formatsTable;

    QPushButton *clearButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;
};
//! [DropSiteWindow header]

#endif // DROPSITEWINDOW_H
