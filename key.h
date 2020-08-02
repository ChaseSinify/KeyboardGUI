#ifndef KEY_H
#define KEY_H

#include <QLabel>
#include <QWidget>

class Key : public QLabel
{
    Q_OBJECT

public:
    explicit Key(QWidget *parent = nullptr, QString keyText = "...");
    void toggleSelected();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    //TESTING
    QPoint dragStartPosition;
    bool selected = false;
};

#endif // KEY_H
