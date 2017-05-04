#include "resultitem.h"

#include <QVBoxLayout>
#include <QLabel>

#include "labels/normallabel.h"

namespace dcc {
namespace update {

ResultItem::ResultItem(QFrame *parent)
    : SettingsItem(parent),
      m_message(new dcc::widgets::NormalLabel),
      m_icon(new QLabel)
{
    m_icon->setFixedSize(50, 50);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_icon, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(m_message, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);
    setFixedHeight(110);
}

void ResultItem::setSuccess(bool success)
{
    const QString pix = success ? ":/update/themes/common/icons/update_success.png" :
                                  ":/update/themes/common/icons/update_failed.png";
    const QString message = success ? tr("Update successfully") :
                                      tr("Failed to update");
    m_icon->setPixmap(QPixmap(pix));
    m_message->setText(message);
}

} // namespace update
} // namespace dcc
