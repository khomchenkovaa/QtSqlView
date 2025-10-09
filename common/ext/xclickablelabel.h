#ifndef X_CLICABLELABEL_H
#define X_CLICABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

class XClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit XClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags())
        : QLabel(parent, f) {}
    explicit XClickableLabel(const QString &text, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags())
        : QLabel(text, parent, f) {}

Q_SIGNALS:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
        QLabel::mousePressEvent(event);
    }
};

#endif // X_CLICABLELABEL_H
