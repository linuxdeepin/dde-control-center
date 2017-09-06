#ifndef CHAINSTYPEPAGE_H
#define CHAINSTYPEPAGE_H

#include "contentwidget.h"

namespace dcc {

namespace widgets {
class ContentWidget;
class OptionItem;

}
namespace network {
class ChainsTypePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit ChainsTypePage(QWidget *parent = nullptr);
    void setDefault(const QString &title);

signals:
    void requestTypeChanged(const QString &type) const;

private slots:
    void onTypeChanged();

private:
    QList<dcc::widgets::OptionItem*> m_list;
};
}
}

#endif // CHAINSTYPEPAGE_H
