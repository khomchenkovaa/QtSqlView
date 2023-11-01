#ifndef XCOLORSELECTOR_H
#define XCOLORSELECTOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
QT_END_NAMESPACE

class XColorSelector : public QWidget
{
    Q_OBJECT
public:
    explicit XColorSelector(QWidget *parent = nullptr);

    void setCaption(const QString& text);
    void setColor(QColor color);
    QColor color() const;

private:
    void setupUI();

private:
    QPushButton *btnSelect;
    QLabel *lblColor;
};

#endif // XCOLORSELECTOR_H
