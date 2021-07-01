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

QSize UpdateItem::sizeHint() const
{
    return QSize(width(), heightForWidth(width()));
}

UpdateItem::UpdateItem(QFrame *parent)
    : SettingsItem(parent)
    , m_appIcon(new SmallLabel)
    , m_appName(new SmallLabel)
    , m_appVersion(new SmallLabel)
    , m_appChangelog(new SmallLabel)
    , m_isLogExpand(false)
    , m_details(new QPushButton)
{
    TranslucentFrame *iconContainer = new TranslucentFrame;
    iconContainer->setFixedWidth(36);

    m_iconLayout = new QVBoxLayout;
    m_iconLayout->setMargin(0);
    m_iconLayout->setSpacing(0);
    m_iconLayout->addWidget(m_appIcon);
    m_iconLayout->setContentsMargins(0, 0, 0, 0);

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
    m_appChangelog->installEventFilter(this);

    m_details->setFlat(true);
    m_details->setText(tr("Details"));

    QHBoxLayout* logLayout = new QHBoxLayout;
    logLayout->setMargin(0);
    logLayout->setSpacing(0);
    logLayout->addWidget(m_appChangelog, 1);
    logLayout->addWidget(m_details, 0, Qt::AlignRight | Qt::AlignBottom);

    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->setMargin(0);
    nameLayout->setSpacing(0);
    nameLayout->addWidget(m_appName);
    nameLayout->addSpacing(4);
    nameLayout->addWidget(m_appVersion, 1, Qt::AlignRight);
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
        m_isLogExpand = !m_isLogExpand;
        m_details->setText(m_isLogExpand ? tr("Collapse") : tr("Details"));
        updateChangelogDisplay();
    });
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    setMinimumHeight(60);
}

void UpdateItem::setAppInfo(const AppUpdateInfo &info)
{
    m_info = info;
    QPixmap pix;
    const qreal ratio = devicePixelRatioF();
    if (info.m_packageId != "dde") {
         pix = QIcon::fromTheme(info.m_packageId, QIcon::fromTheme(info.m_icon, QIcon::fromTheme("application-x-desktop")))
                .pixmap(m_appIcon->size() * ratio)
                .scaled(m_appIcon->size() * ratio,
                        Qt::KeepAspectRatioByExpanding,
                        Qt::FastTransformation);
         pix.setDevicePixelRatio(ratio);
    } else {
        pix = loadPixmap(":/update/themes/dde.svg").scaled(m_appIcon->size() * ratio,
                                               Qt::KeepAspectRatioByExpanding,
                                               Qt::SmoothTransformation);
    }

    m_appIcon->setPixmap(pix);

    m_appName->setText(info.m_name.trimmed());
    m_appVersion->setText(info.m_avilableVersion.trimmed());

    updateChangelogDisplay();
}

QString UpdateItem::elidedChangelog() const
{
    QString text = m_info.m_changelog;

    const QFontMetrics fm(m_appChangelog->fontMetrics());
    const QRect rect(0, 0, m_appChangelog->width(), fontMetrics().height());
    const int textFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    if (rect.contains(fm.boundingRect(rect, textFlag, text)))
        return text;

    QString str(text + "...");

    while (true) {
        if (str.size() < 4)
            break;

        QRect boundingRect = fm.boundingRect(rect, textFlag, str);
        if (rect.contains(boundingRect))
            break;

        str.remove(str.size() - 4, 1);
    }

    return str;
}

/**
 * @brief UpdateItem::getExtendLog
 * 自定义更新日志字符串换行函数，英文在qlabel中没有空格字符时不会换行 + ‘\n’不会分行显示
 * @return 一行字符宽度超过qlabel宽度时添加换行，‘\n’全部添加换行
 */
const QString UpdateItem::getExtendLog() const
{
    const QFontMetrics fm(m_appChangelog->fontMetrics());
    QString retLog = m_info.m_changelog;

    int fontWidth = fm.width(retLog);

    retLog.prepend("<style type=\"text/css\"></style>");

    const int labelWidth = m_appChangelog->width();
    if (fontWidth <= labelWidth) {
        return retLog.replace('\n', "<br/>");
    }

    // 当前行字符串
    QString curLine = m_info.m_changelog;
    // 返回字符中的一个记录序号
    int pos = QString("<style type=\"text/css\"></style>").size();
    // 文本宽度计数
    int widthCount = 0;
    while (true) {
        if (curLine.isEmpty()) {
            break;
        }
        int logSize = curLine.size();
        for (int i = 0; i < logSize; ++i) {
            const QChar word = curLine.at(i);

            // '\n'转换行
            if (word == '\n') {
                curLine = curLine.right(curLine.size() - i -1);
                if (curLine.isEmpty()) {
                    return retLog;
                }
                retLog.replace(pos, 1, "<br/>");
                // 5个字符替换一个字符加4
                pos += 4;
                widthCount = 0;
                break;
            }

            widthCount += fm.width(word);

            // 字符行宽大于标签宽度添加换行
            if (widthCount >= labelWidth) {
                curLine = curLine.right(curLine.size() - i -1);
                if (curLine.isEmpty()) {
                    return retLog;
                }

                retLog.insert(pos, "<br/>");
                // 插入5个字符加5
                pos += 5;
                widthCount = 0;
                break;
            }

            // 不足一行时返回
            if (i == logSize - 1) {
                return retLog;
            }
            ++pos;
        }
    }

    return retLog;
}

void UpdateItem::updateChangelogDisplay()
{
    const QString elidedText = elidedChangelog();
    bool needElide = m_info.m_changelog != elidedText;
    m_details->setVisible(needElide);

    if (m_isLogExpand) {
        m_appChangelog->setText(m_info.m_changelog);
    } else {
        m_appChangelog->setText(elidedText);
    }
}

bool UpdateItem::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize && obj == m_appChangelog) {
        updateChangelogDisplay();
    }

    return SettingsItem::eventFilter(obj, event);
}
}
}
