#include "drawgrid.h"
#include "pixel.h"
#include "selection.h"

#include <QtWidgets>
#include <QRegularExpression>

// IMPORTANT: make shortcuts for activating tools
// Also, pressing escape or delete should delete the tool instance from your hand

// constructor
DrawGrid::DrawGrid(QWidget *parent)
    : QWidget(parent)
{
    // layout basics
    this->pixelGrid = new QGridLayout;
    this->setLayout(this->pixelGrid); // https://doc.qt.io/qt-5/qgridlayout.html
    this->pixelGrid->setAlignment(Qt::AlignLeft);
    this->pixelGrid->setRowStretch(0,0);
    this->pixelGrid->setColumnStretch(0,0);
    this->pixelGrid->setContentsMargins(0,0,0,0);
    this->pixelGrid->setHorizontalSpacing(0);
    this->pixelGrid->setVerticalSpacing(0);
    this->pixelGrid->setRowMinimumHeight(0, 10);
    this->pixelGrid->setColumnMinimumWidth(0, 10); // IF YOU WANT MORE PIXELS, HAVE TO SET THIS FOR EVERY ROW OR THEY COMPRESS TO 0

    generateGrid();
    setStyleSheet("background: transparent; border: 1px solid rgba(255, 255, 255, .3);");
    setAcceptDrops(true);
}
// get keys for modeling
QList<QRect> DrawGrid::getKeysForModeling()
{
    QList<QRect> modelKeys; // holds all of the rectangle geometries we will be returning to caller
    // regex for finding colors of stylesheets
    QRegularExpression colorRegex("Background: #[0-9A-F]{6};", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch colorMatch;
    QString colorToSearchGridFor; // holds the current color to search 16px 4x4 grid minimum for same color
    bool differentStyleSheet = false; // this is just a flag that says if the 12px border we checked has a different stylesheet i.e. we dont have a key

    // IMPORTANT: these checks assume we only check down and to the right 4x4, may need changing for arbitrary key shapes and sizes...
    for(int row = 0; row < pixelGrid->rowCount() - 3; row++) // BOUNDARY CHECK: CANT START A NEW KEY < 4 KEYS AWAY FROM BOTTOM
    {
        for(int col = 0; col < pixelGrid->columnCount() - 3; col++) // BOUNDARY CHECK: CANT START A NEW KEY < 4 KEYS AWAY FROM RIGHT WALL
        {
            QLayoutItem *item = this->pixelGrid->itemAtPosition(row, col);
            if(item != nullptr)
            {
                // if the pixel has color information
                if(item->widget()->styleSheet() != "" && item->widget()->styleSheet() != "background: transparent;") // make sure its not empty or set to transparent
                {
                    // check the minimum 16px 4x4 grid starting from the first occurance as top left
                    // to see if all 16 are same color. Then compare this color to the reference map to
                    // see if we have a key name to generate... if not, just make a default key with
                    // an elipses on it by default. If the 16px 4x4 doesnt match, ignore and continue

                    /*
                     * GRID MATH: IMPORTANT --> WE CAN JUST CHECK THE BORDER POSITIONS SO WE CAN ALSO TAKE CENTRAL COLOR CODES LATER -- THIS IS ALSO FAST FOR USER TO DRAW
                     * itemAtPostion(row,col) --> this is starting top left
                     * check all of the following: (row,col) = 0,0 for example...
                     * TOP:-(this)0,0; 0,1; 0,2; 0,3; &
                     * ROW2:------1,0; .... .... 1,3; &
                     * ROW3:------2,0; .... .... 2,3; &
                     * BOTTOM:----3,0; 3,1; 3,2; 3,3; &
                     */

                    // get the color
                    QString styleSheet = item->widget()->styleSheet();
                    colorMatch = colorRegex.match(styleSheet);
                    if(colorMatch.hasMatch())
                    {
                        colorToSearchGridFor = colorMatch.captured(0).mid(12, 7) + ",";
                        // compare colors based only on stylesheet value
                        // TOP
                        if(styleSheet != this->pixelGrid->itemAtPosition(row, col + 1)->widget()->styleSheet()){differentStyleSheet = true;}
                        if(styleSheet != this->pixelGrid->itemAtPosition(row, col + 2)->widget()->styleSheet()){differentStyleSheet = true;}
                        if(styleSheet != this->pixelGrid->itemAtPosition(row, col + 3)->widget()->styleSheet()){differentStyleSheet = true;}
                        // ROW2
                        if(styleSheet != this->pixelGrid->itemAtPosition(row + 1, col)->widget()->styleSheet()){differentStyleSheet = true;}
                        if(styleSheet != this->pixelGrid->itemAtPosition(row + 1, col + 3)->widget()->styleSheet()){differentStyleSheet = true;}
                        // ROW3
                        if(styleSheet != this->pixelGrid->itemAtPosition(row + 2, col)->widget()->styleSheet()){differentStyleSheet = true;}
                        if(styleSheet != this->pixelGrid->itemAtPosition(row + 2, col + 3)->widget()->styleSheet()){differentStyleSheet = true;}
                        // BOTTOM
                        if(styleSheet != this->pixelGrid->itemAtPosition(row + 3, col)->widget()->styleSheet()){differentStyleSheet = true;}
                        if(styleSheet != this->pixelGrid->itemAtPosition(row + 3, col + 1)->widget()->styleSheet()){differentStyleSheet = true;}
                        if(styleSheet != this->pixelGrid->itemAtPosition(row + 3, col + 2)->widget()->styleSheet()){differentStyleSheet = true;}
                        if(styleSheet != this->pixelGrid->itemAtPosition(row + 3, col + 3)->widget()->styleSheet()){differentStyleSheet = true;}

                        // if none of the above failed set differentStyleSheet to false... add this items position information to the QRect list of model keys
                        if(differentStyleSheet == false)
                        {
                            modelKeys.append(QRect(item->widget()->pos().x(), item->widget()->pos().y(), 40, 40));
                        }
                        else
                        {
                            differentStyleSheet = false;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
            }
        }
    }
    return modelKeys;
}
// generate grid
void DrawGrid::generateGrid()
{
    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            Pixel *pixel = new Pixel(this);
            this->pixelGrid->addWidget(pixel, row, col, 1, 1);
        }
    }
}
// clear grid
void DrawGrid::clear()
{
    QWidget *pixel;
    for(int row = 0; row < this->rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            pixel = this->pixelGrid->itemAtPosition(row, col)->widget();
            if(pixel != nullptr)
            {
                pixel->setStyleSheet("background: transparent;");
            }
        }
    }
}
// render imported layout
bool DrawGrid::renderImportedLayout(int eventType, QString filePath, bool addToMyLayouts)
{
    if(!filePath.endsWith(".sock"))
    {
        qDebug() << "Not a .sock file!";
        return 1;
    }

    QFile layoutFile(filePath);
    if (!layoutFile.open(QIODevice::ReadOnly))
    {
        qDebug() << layoutFile.errorString();
        return 1;
    }
    QStringList colors;
    while(!layoutFile.atEnd())
    {
        QString fileData = layoutFile.readLine();
        colors.append(fileData.split(','));
    }
    for(int gridIndex = 0; gridIndex < colors.length() - 1; gridIndex++)
    {
        QStringList color = colors[gridIndex].split(',');
        if(colors[gridIndex] == "") continue; // if empty stylesheet
        else this->pixelGrid->itemAt(gridIndex)->widget()->setStyleSheet(
                    "Background: " + colors[gridIndex] + ";");
    }
    return 0;
}
// basic export testing
QString DrawGrid::exportLayout()
{
    QRegularExpression colorRegex("Background: #[0-9A-F]{6};", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch colorMatch;
    QString exportData;
    for(int row = 0; row < pixelGrid->rowCount(); row++)
    {
        for(int col = 0; col < pixelGrid->columnCount(); col++)
        {
            QLayoutItem *item = this->pixelGrid->itemAtPosition(row, col);
            if(item != nullptr)
            {
                QString styleSheet = item->widget()->styleSheet();
                colorMatch = colorRegex.match(styleSheet);
                if(colorMatch.hasMatch())
                {
                    exportData += colorMatch.captured(0).mid(12, 7) + ",";
                }
                else
                {
                    exportData += ",";
                }
            }
        }
    }
    return exportData;
}
// selection tool
void DrawGrid::toggleSelectionTool()
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
            this->setCursor(this->defaultCursor);
        }
    }
}
// eraser tool
void DrawGrid::toggleEraserTool()
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
            this->setCursor(this->defaultCursor);
        }
    }
}
// set brush color
void DrawGrid::setBrushColor(QColor color)
{
    this->currentBrushColor = color;
}
// set cursor color
void DrawGrid::setCursorBrush(QCursor cursorBrush)
{
    this->cursorBrush = cursorBrush;
}
// mouse press event
void DrawGrid::mousePressEvent(QMouseEvent *event){
    if(event->buttons() == Qt::LeftButton)
    {
        // if we have an active selection, ignore additional mouse presses to avoid overriding the selection event
        if(selectionActive == true)
        {
            event->ignore();
        }
        else if(this->selection == true) // no active selection, selection checked, and mouse event --> make new selection
        {
            // set active selection flag
            this->selectionActive = true;

            // get starting point for selection
            this->pressPosition = event->pos();

            // create selection object
            this->currentSelection = new Selection(QRubberBand::Rectangle, this, currentBrushColor);
            this->currentSelection->setGeometry(QRect(this->pressPosition, QSize()));
            this->currentSelection->show();
        }
        else if(this->eraser == true)
        {
            this->childAt(event->pos())->setStyleSheet("");
        }
        else
        {
            this->childAt(event->pos())->setStyleSheet("background: " + this->currentBrushColor.name() + ";");
        }
    }
    else
    {
        event->ignore();
    }
}
// mouse move event
void DrawGrid::mouseMoveEvent(QMouseEvent *event)
{
    // make sure we are in the grid
    if(this->rect().contains(event->pos()))
    {
        // helper for calculations
        this->moveEventSnap = event->pos();

        // starting point snapping
        if (pressPosition.x() % 10 >= 5)
        {
            pressPosition.setX(pressPosition.x() + (10 - (pressPosition.x() % 10)));
        }
        else
        {
            pressPosition.setX((pressPosition.x() - (pressPosition.x() % 10)));
        }
        if (pressPosition.y() % 10 >= 5)
        {
            pressPosition.setY(pressPosition.y() + (10 - (pressPosition.y() % 10)));
        }
        else
        {
            pressPosition.setY((pressPosition.y() - (pressPosition.y() % 10)));
        }

        // incremental snapping
        if (moveEventSnap.x() % 10 >= 5)
        {
            moveEventSnap.setX(moveEventSnap.x() + (10 - (moveEventSnap.x() % 10)));
        }
        else
        {
            moveEventSnap.setX((moveEventSnap.x() - (moveEventSnap.x() % 10)));
        }
        if (moveEventSnap.y() % 10 >= 5)
        {
            moveEventSnap.setY(moveEventSnap.y() + (10 - (moveEventSnap.y() % 10)));
        }
        else
        {
            moveEventSnap.setY((moveEventSnap.y() - (moveEventSnap.y() % 10)));
        }

        // selection
        if(this->selection == true)
        {
            this->currentSelection->setGeometry(QRect(this->pressPosition, moveEventSnap).normalized());//.normalized()); // was current position
        }

        // eraser -- cant use snapping, only makes it worse, only concept we could try would be lower mouse sensitiviy
        else if(this->eraser == true)
        {
            this->childAt(event->pos())->setStyleSheet("");
        }

        // regular drawing -- cant use snapping, only makes it worse, only concept we could try would be lower mouse sensitiviy
        else
        {
            this->childAt(event->pos())->setStyleSheet("background: " + this->currentBrushColor.name() + ";");
        }
    }
    else
    {
        event->ignore();
    }
}
// mouse release event
void DrawGrid::mouseReleaseEvent(QMouseEvent *event)
{
    // get and set the release position
    this->releasePosition = this->moveEventSnap;//event->pos();

    // confirm we have a selection
    if(this->currentSelection && this->selection == true)
    {
        QWidget *pixel;
        // if we were in a selection, set the flag back to false
        this->selectionActive = false;

        // correct the position
        this->correctedSelectionPosition = {this->pressPosition.x(), this->pressPosition.y(),
                                            this->releasePosition.x() - this->pressPosition.x(), this->releasePosition.y() - this->pressPosition.y()};
        // iterate through the grid
        for(int row = 0; row < pixelGrid->rowCount(); row++)
        {
            for(int col = 0; col < pixelGrid->columnCount(); col++)
            {
                pixel = this->pixelGrid->itemAtPosition(row, col)->widget();
                if(pixel != nullptr)
                {
                    if(this->correctedSelectionPosition.contains(pixel->geometry()))
                    {
                        // eraser selection tool
                        if(this->eraser == true)
                        {
                           pixel->setStyleSheet("");
                        }
                        // basic selection tool
                        else
                        {
                            pixel->setStyleSheet("background: " + this->currentBrushColor.name() + ";");
                        }
                    }
                }
            }
        }
        currentSelection->hide();
    }
}
