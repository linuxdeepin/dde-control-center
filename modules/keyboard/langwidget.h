#ifndef LANGWIDGET_H
#define LANGWIDGET_H

#include "contentwidget.h"
#include "indexdelegate.h"
#include "indexview.h"
#include "indexmodel.h"

class LangWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit LangWidget(QWidget *parent = 0);
    void setModelData(const QList<MetaData>& datas);

private:
    IndexDelegate* m_delegate;
    IndexView* m_view;
    IndexModel* m_model;
};

#endif // LANGWIDGET_H
