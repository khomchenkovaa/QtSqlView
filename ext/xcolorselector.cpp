#include "xcolorselector.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QColorDialog>

/******************************************************************/

XColorSelector::XColorSelector(QWidget *parent)
    : QWidget(parent)
    , btnSelect(new QPushButton(this))
    , lblColor(new QLabel(this))
{
    setupUI();
}

/******************************************************************/

void XColorSelector::setCaption(const QString &text)
{
    btnSelect->setText(text);
}

/******************************************************************/

void XColorSelector::setColor(QColor color)
{
    QPalette palette = lblColor->palette();
    palette.setColor(QPalette::Window, color);
    lblColor->setPalette(palette);
}

/******************************************************************/

QColor XColorSelector::color() const
{
    const QPalette palette = lblColor->palette();
    return palette.color(QPalette::Window);
}

/******************************************************************/

void XColorSelector::setupUI()
{
    lblColor->setFrameStyle(QFrame::Sunken | QFrame::Panel);
    lblColor->setFixedWidth(32);
    lblColor->setAutoFillBackground(true);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(btnSelect);
    mainLayout->addWidget(lblColor);

    QObject::connect(btnSelect, &QPushButton::clicked, this, [this](){
        const QColor bgColor = color();
        const QColor color = QColorDialog::getColor(bgColor, this, "Select Color");
        if (color.isValid()) {
            setColor(color);
        }
    });
}

/******************************************************************/
