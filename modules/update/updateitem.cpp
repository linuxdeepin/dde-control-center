#include "updateitem.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "labels/smalllabel.h"
#include "translucentframe.h"

using namespace dcc::widgets;

namespace dcc{
namespace update{

UpdateItem::UpdateItem(QFrame *parent)
    :SettingsItem(parent),
      m_appIcon(new SmallLabel),
      m_appName(new SmallLabel),
      m_appVersion(new SmallLabel),
      m_appChangelog(new SmallLabel)
{
    TranslucentFrame *iconContainer = new TranslucentFrame;
    iconContainer->setFixedWidth(36);

    m_iconLayout = new QVBoxLayout;
    m_iconLayout->setMargin(0);
    m_iconLayout->setSpacing(0);
    m_iconLayout->addWidget(m_appIcon, 0, Qt::AlignCenter);
    m_iconLayout->addStretch();

    iconContainer->setLayout(m_iconLayout);

    m_appIcon->setFixedSize(36, 36);

    m_appChangelog->setWordWrap(true);
    m_appChangelog->setAlignment(Qt::AlignTop);

    m_details = new QPushButton;
    m_details->setObjectName("DCC-Update-UpdateItem-Details");
    m_details->setText(tr("Details"));

    QHBoxLayout* logLayout = new QHBoxLayout;
    logLayout->setMargin(0);
    logLayout->setSpacing(0);
    logLayout->addWidget(m_appChangelog, 1);
    logLayout->addWidget(m_details);

    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->setMargin(0);
    nameLayout->setSpacing(0);
    nameLayout->addWidget(m_appName);
    nameLayout->addSpacing(4);
    nameLayout->addWidget(m_appVersion);
    nameLayout->addStretch();

    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addStretch();
    rightLayout->addLayout(nameLayout);
    rightLayout->addSpacing(1);
    rightLayout->addLayout(logLayout);
    rightLayout->addStretch();

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(10);
    layout->setSpacing(0);
    layout->addWidget(iconContainer);
    layout->addSpacing(10);
    layout->addLayout(rightLayout, 1);

    setLayout(layout);

    connect(m_details, &QPushButton::clicked, [this] {
        const int lines = changelogLines();
        const int expandHeight = (lines - 2) * 20;

        m_appChangelog->setText(m_info.m_changelog);
        m_appChangelog->setFixedHeight(m_appChangelog->height() + expandHeight);

        setFixedHeight(height() + expandHeight + 5);

        m_details->hide();
     });
}

void UpdateItem::setAppInfo(const AppUpdateInfo &info)
{
    m_info = info;

    QPixmap pix = QPixmap(m_info.m_icon).scaled(m_appIcon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_appIcon->setPixmap(pix);

    m_appName->setText(info.m_name.trimmed());
    m_appVersion->setText(info.m_avilableVersion.trimmed());

    if(!info.m_changelog.isEmpty()) {
        setFixedHeight(80);
        m_iconLayout->setContentsMargins(0, 10, 0, 0);
        m_appChangelog->setText(elideChangelog());
    } else {
        setFixedHeight(60);
        m_iconLayout->setContentsMargins(0, 0, 0, 0);
        m_details->hide();
    }
}

QString UpdateItem::elideChangelog() const
{
    const QString text = m_info.m_changelog;

    const QFontMetrics fm(m_appChangelog->font());
    const QRect rect(0, 0, 200, fm.height() * 2);
    const int textFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    if (rect.contains(fm.boundingRect(rect, textFlag, text)))
        return text;

    QString str(text + "...");

    while (true)
    {
        if (str.size() < 4)
            break;

        QRect boundingRect = fm.boundingRect(rect, textFlag, str);
        if (rect.contains(boundingRect))
            break;

        str.remove(str.size() - 4, 1);
    }

    return str;
}

int UpdateItem::changelogLines() const
{
    const QString text = m_info.m_changelog;
    const QFontMetrics fm(m_appChangelog->font());
    const int changelogLineHeight = fm.height();

    QRect rect(0, 0, 240, changelogLineHeight * 2);
    const int textFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    while (true)
    {
        QRect boundingRect = fm.boundingRect(rect, textFlag, text);
        if (rect.contains(boundingRect))
            break;

        rect.setHeight(rect.height() + changelogLineHeight);
    }

    return rect.height() / changelogLineHeight;
}

}
}
