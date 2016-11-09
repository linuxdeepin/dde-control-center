#ifndef KEYBOARDLAYOUTWIDGET_H
#define KEYBOARDLAYOUTWIDGET_H

#include "contentwidget.h"
#include "indexmodel.h"
#include "indexview.h"
#include "indexframe.h"
#include "indexdelegate.h"

class KeyboardLayoutWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit KeyboardLayoutWidget(QWidget *parent = 0);
    void setMetaData(const QList<MetaData>& datas);
    void setLetters(QList<QString> letters);
    QList<MetaData> selectData() const;

private:
    IndexDelegate* m_delegate;
    IndexView* m_view;
    IndexModel* m_model;
    IndexFrame* m_indexframe;
};

#endif // KEYBOARDLAYOUTWIDGET_H
