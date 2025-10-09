#ifndef XCOMBOBOX_H
#define XCOMBOBOX_H

#include <QComboBox>

class XComboBox : public QComboBox
{
    Q_OBJECT

    bool skipNextHide;
public:
    explicit XComboBox(QWidget *parent);

    virtual bool eventFilter(QObject *object, QEvent *event) override;

    virtual void hidePopup() override;

    void setPlaceholderText(const QString &text);

    QModelIndex findIndexFromData(QVariant data,
                                  int column = 0,
                                  int role = Qt::UserRole + 1,
                                  Qt::MatchFlags flags = static_cast<Qt::MatchFlags>(Qt::MatchExactly|Qt::MatchCaseSensitive|Qt::MatchRecursive)) const;

    QModelIndex currentModelIndex() const;
    QVariant currentModelData() const;
    virtual QVariant modelDataByIdx(const QModelIndex& index) const;

public Q_SLOTS:
    void setCurrentModelIndex(const QModelIndex& index);
    void setNotSelected();
    void enableItemsRecoursive(const QModelIndex& index = QModelIndex(), bool enabled = true);
    void expand(const QModelIndex &index);
    void expandToRoot(const QModelIndex &index);

Q_SIGNALS:
    void currentDataChanged(QVariant data);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    void setupTreeView();

};

#endif // XCOMBOBOX_H
