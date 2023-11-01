#ifndef XDATEEDIT_H
#define XDATEEDIT_H

#include <QDateEdit>

/*****************************************************************/

class XDateEdit : public QDateEdit
{
    Q_OBJECT

public:
    explicit XDateEdit(QWidget *parent = nullptr);
    ~XDateEdit() override {}

    QDateTime dateTime() const;
    QDate date() const;
    QTime time() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    bool focusNextPrevChild(bool next) override;
    QValidator::State validate(QString &input, int &pos) const override;
    QDateTime dateTimeFromText(const QString &text) const override;

public Q_SLOTS:
    void clear() override;
    void setDateTime(const QDateTime &dateTime);
    void setDate(const QDate &date);
    void setTime(const QTime &time);

private:
    void setupUI();

private:
    bool f_DateIsNull;
};

/*****************************************************************/

#endif // XDATEEDIT_H
