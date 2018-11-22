/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "updateitem.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QFile>
#include <QDesktopServices>

#include "widgets/labels/smalllabel.h"
#include "widgets/translucentframe.h"
#include "widgets/basiclistdelegate.h"

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

QSize UpdateItem::sizeHint() const
{
    return QSize(width(), heightForWidth(width()));
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

    QFont changelogFont("Noto Mono");
    changelogFont.setPointSize(10);

    m_appChangelog->setFont(changelogFont);
    m_appChangelog->setWordWrap(true);
    m_appChangelog->setTextFormat(Qt::RichText);
    m_appChangelog->setTextInteractionFlags(Qt::NoTextInteraction);
    m_appChangelog->setContextMenuPolicy(Qt::NoContextMenu);
    m_appChangelog->setOpenExternalLinks(true);
    m_appChangelog->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_appChangelog->setFocusPolicy(Qt::NoFocus);
    m_appChangelog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
}

void UpdateItem::setAppInfo(const AppUpdateInfo &info)
{
    m_info = info;
    QFile file(m_info.m_icon);
    QPixmap pix;
    const qreal ratio = devicePixelRatioF();

    if (file.exists())
        pix = loadPixmap(m_info.m_icon).scaled(m_appIcon->size() * ratio,
                                               Qt::KeepAspectRatioByExpanding,
                                               Qt::SmoothTransformation);
    else {
        pix = QIcon::fromTheme(m_info.m_packageId, QIcon::fromTheme("application-x-desktop"))
                .pixmap(m_appIcon->size() * ratio)
                .scaled(m_appIcon->size() * ratio,
                        Qt::KeepAspectRatioByExpanding,
                        Qt::SmoothTransformation);
        pix.setDevicePixelRatio(ratio);
    }

    m_appIcon->setPixmap(pix);

    m_appName->setText(info.m_name.trimmed());
    m_appVersion->setText(info.m_avilableVersion.trimmed());

    const QString changelog = m_info.m_changelog;
    const QString elidedText = elidedChangelog();

    if (changelog != elidedText)
    {
        m_iconLayout->setContentsMargins(0, 10, 0, 0);
        m_appChangelog->setText(elidedText);
    } else {
        setMinimumHeight(60);
        m_iconLayout->setContentsMargins(0, 0, 0, 0);
        m_appChangelog->setText(changelog);
        m_details->setVisible(false);
    }
}

QString UpdateItem::elidedChangelog() const
{
    const QString text = QString(clearHTMLTags(m_info.m_changelog)).replace("\n", "");

    const QFontMetrics fm(m_appChangelog->font());
    const QRect rect(0, 0, 200, fontMetrics().height() * 2);
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
    const QString stylesheet = "<style type=\"text/css\">"
                               "a { color: #0082FA; }"
                               "</style> ";

    m_appChangelog->setText(stylesheet + m_info.m_changelog.replace('\n', "<br>"));
}

}
}
