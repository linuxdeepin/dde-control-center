#ifndef LANGWIDGET_H
#define LANGWIDGET_H

#include "contentwidget.h"
#include "indexdelegate.h"
#include "indexview.h"
#include "indexmodel.h"
#include "searchinput.h"
#include "translucentframe.h"

#include <DGraphicsClipEffect>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{
class QLineEdit;
class KeyboardModel;
class LangWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit LangWidget(KeyboardModel *model, QWidget *parent = 0);

signals:
    void click(const QModelIndex& index);

public slots:
    void onSearch(const QString& text);
    void setCurLang(const QString& lang);
    void setModelData(const QList<MetaData>& datas);

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    SearchInput* m_search;
    IndexDelegate* m_delegate;
    IndexView* m_view;
    IndexModel* m_model;
    IndexModel* m_searchModel;
    KeyboardModel *m_keyboardModel;
    DGraphicsClipEffect *m_clipEffectWidget;
    TranslucentFrame* m_contentWidget;
    QList<MetaData> m_datas;
};
}
}
#endif // LANGWIDGET_H
