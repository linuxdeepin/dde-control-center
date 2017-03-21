#include "resolutiondetailpage.h"
#include "settingsgroup.h"
#include "monitor.h"
#include "displaymodel.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::display;

ResolutionDetailPage::ResolutionDetailPage(QWidget *parent)
    : ContentWidget(parent),

      m_resolutions(new SettingsGroup),
      m_currentItem(nullptr)
{
    setTitle(tr("Resolution"));

    TranslucentFrame *widget = new TranslucentFrame;
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setMargin(0);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_resolutions);

    setContent(widget);
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
    const auto currentMode = mon->currentMode();

    bool first = true;
    for (auto m : modes)
    {
        const QString res = QString::number(m.width()) + "×" + QString::number(m.height());
        OptionItem *item = new OptionItem;
        item->setContentsMargins(20, 0, 10, 0);

        connect(item, &OptionItem::selectedChanged, this, &ResolutionDetailPage::onItemClicked);

        if (first)
        {
            first = false;
            item->setTitle(res + tr(" (Recommended)"));
        } else {
            item->setTitle(res);
        }

        if (m == currentMode)
            m_currentItem = item;

        m_options[item] = m.id();
        m_resolutions->appendItem(item);
    }

    Q_ASSERT(m_currentItem);
    m_currentItem->blockSignals(true);
    m_currentItem->setSelected(true);
    m_currentItem->blockSignals(false);
}

void ResolutionDetailPage::onItemClicked()
{
    OptionItem *item = qobject_cast<OptionItem *>(sender());
    Q_ASSERT(m_options.contains(item));

    if (item == m_currentItem)
        return;

    Q_ASSERT(m_currentItem);
    m_currentItem->blockSignals(true);
    m_currentItem->setSelected(false);
    m_currentItem->blockSignals(false);
    m_currentItem = item;

    emit requestSetResolution(m_model->monitorList().first(), m_options[item]);
}
