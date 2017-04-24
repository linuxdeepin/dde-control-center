#ifndef KEYBOARDLAYOUTWIDGET_H
#define KEYBOARDLAYOUTWIDGET_H

#include "contentwidget.h"
#include "indexmodel.h"
#include "indexview.h"
#include "indexframe.h"
#include "indexdelegate.h"
#include "searchinput.h"

#include <DGraphicsClipEffect>

DWIDGET_USE_NAMESPACE

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

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

signals:
    void layoutSelected(const QString &value);

public slots:
    void onSearch(const QString& text);
    void onItemClicked(const QModelIndex &index);

private:
    int textLength;
    SearchInput* m_search;
    IndexDelegate* m_delegate;
    IndexView* m_view;
    IndexModel* m_model;
    IndexModel* m_searchModel;
    IndexFrame* m_indexframe;
    TranslucentFrame *m_mainWidget;
    DGraphicsClipEffect *m_clipEffectWidget;
};
}
}
#endif // KEYBOARDLAYOUTWIDGET_H
