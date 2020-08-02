#ifndef DRAWGRID_H
#define DRAWGRID_H

#include <QWidget>
#include <QGridLayout>
#include <QColor>

#include "selection.h"

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QMouseEvent;
QT_END_NAMESPACE

class DrawGrid : public QWidget
{
public:
    explicit DrawGrid(QWidget *parent = nullptr);

    // basic size information
    int rows = 36;
    int cols = 100;

    // basic grid functions
    void generateGrid();
    void clear();

    // brush functions
    void setBrushColor(QColor color);
    void setCursorBrush(QCursor cursorBrush);

    // tool functions
    void toggleSelectionTool();
    void toggleEraserTool();

    // import and export layout functions
    bool renderImportedLayout(int eventType, QString filePath, bool addToMyLayouts = false);
    QString exportLayout();

    //TESTING SUBFUNCTION THAT WILL ULTIMATELY GO TO EXPORT MOST LIKELY
    QList<QRect> getKeysForModeling();

protected:
    // event overrides
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // pixel grid information template
    QGridLayout* pixelGrid;

    // brush information
    QColor currentBrushColor; // this can hold "#RRGGBB" color values and converts to arbg properly UWU

    // cursor/brush
    QCursor cursorBrush;
    QCursor defaultCursor = Qt::PointingHandCursor;

    // bool tools
    bool selection = false;
    bool eraser = false;
    bool selectionActive = false;

    // selection
    QPoint pressPosition;
    QPoint releasePosition;
    Selection *currentSelection;
    QRect correctedSelectionPosition;

    // selection snapping helpers
    QPoint moveEventSnap;
};
#endif // DRAWGRID_H
