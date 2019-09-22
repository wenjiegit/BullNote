#ifndef NOTEITEM_H
#define NOTEITEM_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class NoteItem;
}

class NoteItem : public QWidget
{
    Q_OBJECT

public:
    explicit NoteItem(QWidget *parent = 0);
    ~NoteItem();

    QJsonObject getData();
    void setData(const QJsonObject &obj);

protected:
    void paintEvent(QPaintEvent *e);
    bool event(QEvent *event);

private slots:
    void onTextChanged();
    void onRemoveClicked();

    void onColorChanged();

private:
    void initColorBtn();
    void addColorBtn(const QColor &clr);

signals:
    void saveRequired();
    void removeRequired();

private:
    Ui::NoteItem *ui;
    QColor m_bgColor;
};

#endif // NOTEITEM_H
