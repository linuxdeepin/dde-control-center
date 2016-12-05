#include "updateitem.h"
#include <QHBoxLayout>


const int ChangelogLineHeight = 20;
UpdateItem::UpdateItem(QFrame *parent)
    :SettingsItem(parent)
{
    m_appIcon = new QLabel();
    m_appName = new QLabel();
    m_appVersion = new QLabel();

    m_appChangelog = new QLabel();
    m_appChangelog->setWordWrap(true);
    m_appChangelog->setAlignment(Qt::AlignTop);

    m_details = new QPushButton();
    m_details->setText(tr("Details"));

    QHBoxLayout* logLayout = new QHBoxLayout();
    logLayout->addWidget(m_appChangelog);
    logLayout->addStretch();
    logLayout->addWidget(m_details);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_appName);
    rightLayout->addWidget(m_appVersion);
    rightLayout->addLayout(logLayout);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(m_appIcon);
    layout->setAlignment(m_appIcon, Qt::AlignCenter);
    layout->addLayout(rightLayout);

    setLayout(layout);

    connect(m_details, &QPushButton::clicked, [this] {
        int lines = changelogLines();
        int expandHeight = (lines - 2)*20;
        m_appChangelog->setText(m_info.m_changelog);
        m_appChangelog->setFixedHeight(m_appChangelog->height() + expandHeight);
        setFixedHeight(height() + expandHeight);
        m_details->hide();
        qDebug()<<Q_FUNC_INFO<<height();
     });
}

void UpdateItem::setAppInfo(const AppUpdateInfo &info)
{
    m_info = info;
    QPixmap pix(QSize(32,32));
    pix.fill(Qt::green);

    m_appIcon->setPixmap(pix);
    m_appName->setText(info.m_name);
    m_appVersion->setText(info.m_avilableVersion);

    if(!info.m_changelog.isEmpty())
    {
        setFixedHeight(100);
        m_appChangelog->setText(elideChangelog());
    }
}

QString UpdateItem::elideChangelog() const
{
    const QString text = m_info.m_changelog;

    const QFontMetrics fm(m_appChangelog->font());
    const QRect rect(0, 0, 200, ChangelogLineHeight * 2);
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

    QRect rect(0, 0, 240, ChangelogLineHeight * 2);
    const QFontMetrics fm(m_appChangelog->font());
    const int textFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    while (true)
    {
        QRect boundingRect = fm.boundingRect(rect, textFlag, text);
        if (rect.contains(boundingRect))
            break;

        rect.setHeight(rect.height() + ChangelogLineHeight);
    }

    return rect.height() / ChangelogLineHeight;
}
