#include "xdatetimeedit.h"

#include <QAction>
#include <QCalendarWidget>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QStyle>

#include <QDebug>

/*****************************************************************/

XDateTimeEdit::XDateTimeEdit(QWidget *parent)
    : QDateTimeEdit (parent),
      f_DateIsNull(true)
{
    setupUI();
}

/*****************************************************************/

QDateTime XDateTimeEdit::dateTime() const
{
    if (f_DateIsNull) {
        return QDateTime();
    }
    return QDateTimeEdit::dateTime();
}

/*****************************************************************/

QDate XDateTimeEdit::date() const
{
    if (f_DateIsNull) {
        return QDate();
    }
    return QDateTimeEdit::date();
}

/*****************************************************************/

QTime XDateTimeEdit::time() const
{
    if (f_DateIsNull) {
        return QTime();
    }
    return QDateTimeEdit::time();
}

/*****************************************************************/

QSize XDateTimeEdit::sizeHint() const
{
    const QSize sz = QDateTimeEdit::sizeHint();
    // добавляем ширины, чтобы поместилась иконка
    return QSize(sz.width() + sz.height(), sz.height());
}

/*****************************************************************/

QSize XDateTimeEdit::minimumSizeHint() const
{
    const QSize sz = QDateTimeEdit::minimumSizeHint();
    // добавляем ширины, чтобы поместилась иконка
    return QSize(sz.width() + sz.height(), sz.height());
}

/*****************************************************************/

void XDateTimeEdit::paintEvent(QPaintEvent *event)
{
    if (f_DateIsNull) {
        lineEdit()->clear();
    }
    // Отрисовываем оставшийся DateEdit
    QDateTimeEdit::paintEvent(event);
}

/*****************************************************************/

void XDateTimeEdit::keyPressEvent(QKeyEvent *event)
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

    QDateTimeEdit::keyPressEvent(event);
}

/*****************************************************************/

void XDateTimeEdit::mousePressEvent(QMouseEvent *event)
{
    QDateTimeEdit::mousePressEvent(event);
    if (f_DateIsNull && calendarWidget()->isVisible()) {
        setDate(QDate::currentDate());
    }
}

/*****************************************************************/

bool XDateTimeEdit::focusNextPrevChild(bool next)
{
    if (f_DateIsNull) {
        return QAbstractSpinBox::focusNextPrevChild(next);
    } else {
        return QDateTimeEdit::focusNextPrevChild(next);
    }
}

/*****************************************************************/

QValidator::State XDateTimeEdit::validate(QString &input, int &pos) const
{
    if (f_DateIsNull) {
        return QValidator::Acceptable;
    }
    return QDateTimeEdit::validate(input, pos);
}

/*****************************************************************/

QDateTime XDateTimeEdit::dateTimeFromText(const QString &text) const
{
    if (text.isEmpty()) { // date is null
        return QDateTime();
    }
    return QDateTimeEdit::dateTimeFromText(text);
}

/*****************************************************************/

void XDateTimeEdit::clear()
{
    setDate(QDate());
}

/*****************************************************************/

void XDateTimeEdit::setDateTime(const QDateTime &dateTime)
{
    QDateTimeEdit::setDateTime(dateTime);
    f_DateIsNull = dateTime.isNull();
}

/*****************************************************************/

void XDateTimeEdit::setDate(const QDate &date)
{
    QDateTimeEdit::setDate(date);
    f_DateIsNull = date.isNull();
}

/*****************************************************************/

void XDateTimeEdit::setTime(const QTime &time)
{
    QDateTimeEdit::setTime(time);
    if (!f_DateIsNull) {
        f_DateIsNull = time.isNull();
    }
}

/*****************************************************************/

void XDateTimeEdit::setupUI()
{
    setCalendarPopup(true);

    QLineEdit *edit = lineEdit();

    QAction *clearAction = edit->addAction(style()->standardIcon(QStyle::SP_LineEditClearButton), QLineEdit::TrailingPosition);
    clearAction->setToolTip(tr("Clear"));

    connect(edit, &QLineEdit::textChanged, this, [this, clearAction](const QString &text) {
        f_DateIsNull = text.isEmpty();
        clearAction->setDisabled(f_DateIsNull);
    });
    connect(this, &QDateTimeEdit::dateTimeChanged, this, [this, clearAction](const QDateTime &dt) {
        f_DateIsNull = dt.isNull();
        clearAction->setDisabled(f_DateIsNull);
    });
    connect(clearAction, &QAction::triggered, this, [this]() {
        setDateTime(QDateTime());
    });
}

/*****************************************************************/
