#include "chainstypepage.h"
#include "translucentframe.h"
#include "settingsgroup.h"
#include "optionitem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::network;

ChainsTypePage::ChainsTypePage(QWidget *parent) : ContentWidget(parent)
{
    setTitle(tr("Proxy Type"));

    OptionItem* http = new OptionItem;
    http->setTitle("http");
    http->setContentsMargins(20, 0, 10, 0);

    OptionItem* socks4 = new OptionItem;
    socks4->setTitle("socks4");
    socks4->setContentsMargins(20, 0, 10, 0);

    OptionItem* socks5 = new OptionItem;
    socks5->setTitle("socks5");
    socks5->setContentsMargins(20, 0, 10, 0);

    SettingsGroup *grp = new SettingsGroup;

    grp->appendItem(http);
    grp->appendItem(socks4);
    grp->appendItem(socks5);

    m_list << http << socks4 << socks5;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(10);
    layout->addWidget(grp);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);

    setContent(w);

    for (OptionItem *item : m_list)
        connect(item, &OptionItem::selectedChanged, this, &ChainsTypePage::onTypeChanged);
}

void ChainsTypePage::setDefault(const QString &title)
{
    for (OptionItem *item : m_list) {
        item->blockSignals(true);
        item->setSelected(item->title() == title);
        item->blockSignals(false);
    }
}

void ChainsTypePage::onTypeChanged()
{
    OptionItem *item = qobject_cast<OptionItem*>(sender());

    Q_ASSERT(item);

    for (OptionItem *w : m_list) {
        if (w == item) {
            emit requestTypeChanged(w->title());
            emit back();
            break;
        }
    }

}
