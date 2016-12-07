#include "resolutiondetailpage.h"

using namespace dcc::widgets;

ResolutionDetailPage::ResolutionDetailPage(QWidget *parent)
    : ContentWidget(parent),

      m_resolutions(new SettingsGroup)
{
    setTitle(tr("Resolution"));
    setContent(m_resolutions);
}

void ResolutionDetailPage::setModel(DisplayModel *model)
{
    m_model = model;

    // delete old items
    for (auto item : m_options.keys())
    {
        m_resolutions->removeItem(item);
        item->deleteLater();
    }
    m_options.clear();

    const Monitor *mon = model->monitorList().first();
    const auto modes = mon->modeList();

    bool first = true;
    for (auto m : modes)
    {
        const QString res = QString::number(m.width()) + "×" + QString::number(m.height());
        OptionItem *item = new OptionItem;

        connect(item, &OptionItem::selectedChanged, this, &ResolutionDetailPage::onItemClicked);

        if (first)
        {
            first = false;
            item->setTitle(res + tr(" (Recommended)"));
        } else {
            item->setTitle(res);
        }

        m_options[item] = m.id();
        m_resolutions->appendItem(item);
    }
}

void ResolutionDetailPage::onItemClicked()
{
    OptionItem *item = qobject_cast<OptionItem *>(sender());
    Q_ASSERT(m_options.contains(item));

    emit requestSetResolution(m_model->monitorList().first(), m_options[item]);
}
