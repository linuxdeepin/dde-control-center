#include "updateitem.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QFile>
#include <QDesktopServices>

#include "labels/smalllabel.h"
#include "translucentframe.h"

using namespace dcc::widgets;

namespace dcc{
namespace update{

static QRegularExpression AnchorReg("<a href='(?<href>.*?)'>(?<content>.*?)</a>");

bool UpdateItem::isAnchor(const QString &input)
{
    return AnchorReg.match(input).hasMatch();
}

QPair<QString, QString> UpdateItem::parseAnchor(const QString &input)
{
    QPair<QString, QString> ret;

    QRegularExpressionMatch match = AnchorReg.match(input);
    if (match.hasMatch()) {
        ret.first = match.captured("href");
        ret.second = match.captured("content");
    }

    return ret;
}

const QString clearHTMLTags(const QString &text)
{
    const QRegularExpression regex("<(\\w+)[^>]*>([^<]*)</\\1>");

    QString ret = text;
    do
    {
        const auto match = regex.match(ret);
        if (!match.isValid() || !match.hasMatch())
            break;

        const int start = match.capturedStart();
        const int len = match.capturedLength();
        const QString &cap = match.captured(2);

        ret.replace(start, len, cap);
    } while (true);

    return ret;
}

UpdateItem::UpdateItem(QFrame *parent)
    :SettingsItem(parent),
      m_appIcon(new SmallLabel),
      m_appName(new SmallLabel),
      m_appVersion(new SmallLabel),
      m_appChangelog(new SmallLabel),
      m_details(new QPushButton)
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
    m_appChangelog->setTextFormat(Qt::RichText);
    m_appChangelog->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_appChangelog->setOpenExternalLinks(true);
    m_appChangelog->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_details->setFlat(true);
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
        m_details->hide();
        // The point of this timer is that the calculation should be taken
        // after the relayout of this item caused by the hide of details button.
        QTimer::singleShot(0, this, &UpdateItem::expandChangelog);
    });

//    connect(m_openWebsite, &QPushButton::clicked, [this] {
//        qDebug() << QString("open website %1 to see release notes of %2").arg(m_anchorAddress).arg(m_anchorName);
//        QDesktopServices::openUrl(m_anchorAddress);
//    });
}

void UpdateItem::setAppInfo(const AppUpdateInfo &info)
{
    m_info = info;
    QFile file(m_info.m_icon);
    QPixmap pix;

    if (file.exists())
        pix = QPixmap(m_info.m_icon).scaled(m_appIcon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    else {
        pix = QIcon::fromTheme(m_info.m_packageId, QIcon::fromTheme("application-x-desktop")).pixmap(m_appIcon->size());
    }

    m_appIcon->setPixmap(pix);

    m_appName->setText(info.m_name.trimmed());
    m_appVersion->setText(info.m_avilableVersion.trimmed());

    const QString changelog = m_info.m_changelog;

    if (!changelog.isEmpty())
    {
        setFixedHeight(80);
        m_iconLayout->setContentsMargins(0, 10, 0, 0);
        m_appChangelog->setText(elidedChangelog());

//    if(!changelog.isEmpty()) {
//        setFixedHeight(80);
//        m_iconLayout->setContentsMargins(0, 10, 0, 0);

//        const bool changelogIsAnchor = isAnchor(changelog);

//        m_openWebsite->setVisible(changelogIsAnchor);
//        m_appChangelog->setVisible(!changelogIsAnchor);

//        if (!changelogIsAnchor)
//            m_appChangelog->setText(elidedChangelog());
//        else {
//            m_details->setVisible(false);

//            QPair<QString, QString> pair = parseAnchor(changelog);
//            m_anchorName = pair.second;
//            m_anchorAddress = pair.first;
//        }

    } else {
        setFixedHeight(60);
        m_iconLayout->setContentsMargins(0, 0, 0, 0);
        m_details->setVisible(false);
    }
}

QString UpdateItem::elidedChangelog() const
{
    const QString text = clearHTMLTags(m_info.m_changelog);

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

void UpdateItem::expandChangelog()
{
    const int textFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    QFontMetrics fm = m_appChangelog->fontMetrics();
    const QRect rect = fm.boundingRect(m_appChangelog->rect(), textFlag, clearHTMLTags(m_info.m_changelog));
    const int heightDelta = rect.height() - m_appChangelog->height();

    m_appChangelog->setText(m_info.m_changelog.replace('\n', "<br>"));
    m_appChangelog->setFixedHeight(rect.height());

    setFixedHeight(height() + heightDelta);
}

}
}
