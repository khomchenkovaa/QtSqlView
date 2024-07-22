#include "xdateedit.h"

#include <QAction>
#include <QCalendarWidget>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QStyle>

#include <QDebug>

/*****************************************************************/

XDateEdit::XDateEdit(QWidget *parent)
    : QDateEdit (parent),
      f_DateIsNull(true)
{
    setupUI();
}

/*****************************************************************/

QDateTime XDateEdit::dateTime() const
{
    if (f_DateIsNull) {
        return QDateTime();
    }
    return QDateEdit::dateTime();
}

/*****************************************************************/

QDate XDateEdit::date() const
{
    if (f_DateIsNull) {
        return QDate();
    }
    return QDateEdit::date();
}

/*****************************************************************/

QTime XDateEdit::time() const
{
    if (f_DateIsNull) {
        return QTime();
    }
    return QDateEdit::time();
}

/*****************************************************************/

QSize XDateEdit::sizeHint() const
{
    const QSize sz = QDateEdit::sizeHint();
    // добавляем ширины, чтобы поместилась иконка
    return QSize(sz.width() + sz.height(), sz.height());
}

/*****************************************************************/

QSize XDateEdit::minimumSizeHint() const
{
    const QSize sz = QDateEdit::minimumSizeHint();
    // добавляем ширины, чтобы поместилась иконка
    return QSize(sz.width() + sz.height(), sz.height());
}

/*****************************************************************/

void XDateEdit::paintEvent(QPaintEvent *event)
{
    if (f_DateIsNull) {
        lineEdit()->clear();
    }
    // Отрисовываем оставшийся DateEdit
    QDateEdit::paintEvent(event);
}

/*****************************************************************/

void XDateEdit::keyPressEvent(QKeyEvent *event)
{
    if (f_DateIsNull) {
        if ((event->key() >= Qt::Key_0) && (event->key() <= Qt::Key_9)) {
            setDate(QDate::currentDate());
        }
        if (event->key() == Qt::Key_Tab) {
            QAbstractSpinBox::keyPressEvent(event); // clazy:exclude=skipped-base-method
            return;
        }
    } else {
        if (event->key() == Qt::Key_Backspace){
            QLineEdit *edit = lineEdit();
            if (edit->selectedText() == edit->text()) {
                setDate(QDate());
                event->accept();
                return;
            }
        }
    }

    QDateEdit::keyPressEvent(event);
}

/*****************************************************************/

void XDateEdit::mousePressEvent(QMouseEvent *event)
{
    QDateEdit::mousePressEvent(event);
    if (f_DateIsNull && calendarWidget()->isVisible()) {
        setDate(QDate::currentDate());
    }
}

/*****************************************************************/

bool XDateEdit::focusNextPrevChild(bool next)
{
    if (f_DateIsNull) {
        return QAbstractSpinBox::focusNextPrevChild(next);
    } else {
        return QDateEdit::focusNextPrevChild(next);
    }
}

/*****************************************************************/

QValidator::State XDateEdit::validate(QString &input, int &pos) const
{
    if (f_DateIsNull) {
        return QValidator::Acceptable;
    }
    return QDateEdit::validate(input, pos);
}

/*****************************************************************/

QDateTime XDateEdit::dateTimeFromText(const QString &text) const
{
    if (text.isEmpty()) { // date is null
        return QDateTime();
    }
    return QDateEdit::dateTimeFromText(text);
}

/*****************************************************************/

void XDateEdit::clear()
{
    setDate(QDate());
}

/*****************************************************************/

void XDateEdit::setDateTime(const QDateTime &dateTime)
{
    QDateEdit::setDateTime(dateTime);
    f_DateIsNull = dateTime.isNull();
}

/*****************************************************************/

void XDateEdit::setDate(const QDate &date)
{
    QDateEdit::setDate(date);
    f_DateIsNull = date.isNull();
}

/*****************************************************************/

void XDateEdit::setTime(const QTime &time)
{
    QDateEdit::setTime(time);
    if (!f_DateIsNull) {
        f_DateIsNull = time.isNull();
    }
}

/*****************************************************************/

void XDateEdit::setupUI()
{
    setCalendarPopup(true);

    QLineEdit *edit = lineEdit();

    QAction *clearAction = edit->addAction(style()->standardIcon(QStyle::SP_LineEditClearButton), QLineEdit::TrailingPosition);
    clearAction->setToolTip(tr("Clear"));

    connect(edit, &QLineEdit::textChanged, this, [this, clearAction](const QString &text) {
        f_DateIsNull = text.isEmpty();
        clearAction->setDisabled(f_DateIsNull);
    });
    connect(this, &QDateEdit::dateChanged, this, [this, clearAction](const QDate &dt) {
        f_DateIsNull = dt.isNull();
        clearAction->setDisabled(f_DateIsNull);
    });
    connect(clearAction, &QAction::triggered, this, [this](){
        setDate(QDate());
    });
}

/*****************************************************************/
