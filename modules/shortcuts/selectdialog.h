#ifndef SELECTDIALOG_H
#define SELECTDIALOG_H

#include <QMessageBox>

#include "searchlist.h"

class SelectDialog : public QMessageBox, public SearchItem
{
    Q_OBJECT

public:
    explicit SelectDialog(QWidget *parent = 0);

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;

private:
    QWidget *m_widget;
};

#endif // SELECTDIALOG_H
