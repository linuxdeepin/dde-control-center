#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QLabel>

#include "searchlist.h"

class ToolTip : public QLabel, public SearchItem
{
    Q_OBJECT
public:
    explicit ToolTip(QWidget *parent = 0);

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;

private:
    QWidget *m_widget;
};

#endif // TOOLTIP_H
