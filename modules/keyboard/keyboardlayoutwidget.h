#ifndef KEYBOARDLAYOUTWIDGET_H
#define KEYBOARDLAYOUTWIDGET_H

#include "contentwidget.h"
#include "indexmodel.h"
#include "indexview.h"
#include "indexframe.h"
#include "indexdelegate.h"
#include "searchinput.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{

class QLineEdit;
class KeyboardLayoutWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit KeyboardLayoutWidget(QWidget *parent = 0);
    ~KeyboardLayoutWidget();

    void setMetaData(const QList<MetaData>& datas);
    void setLetters(QList<QString> letters);
    QList<MetaData> selectData() const;

public slots:
    void onSearch(const QString& text);

private:
    int textLength;
    SearchInput* m_search;
    IndexDelegate* m_delegate;
    IndexView* m_view;
    IndexModel* m_model;
    IndexModel* m_searchModel;
    IndexFrame* m_indexframe;
};
}
}
#endif // KEYBOARDLAYOUTWIDGET_H
