/*
 * Copyright (C) 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     liuxing <liuxing@uniontech.com>
 *
 * Maintainer: liuxing <liuxing@uniontech.com>
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

#include "paperdetailwidget.h"
#include "papericonbutton.h"

#include <DStyle>

#include <QPainter>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QDir>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::personalization;

const int SettingWidth = 150;
const int SettingHeight = 40;
const int MarginWidth = 10;

PaperDetailWidget::PaperDetailWidget(QWidget *parent)
    : QDialog(parent)
    , m_curIndex(0)
    , m_previewWidget(new QWidget)
    , m_toPrevious(new PaperIconButton)
    , m_toNext(new PaperIconButton)
    , m_cancel(new PaperIconButton)
    , m_confirm(new PaperIconButton)
    , m_hideTimer(new QTimer(this))
    , m_settingsWidget(new QWidget)
    , m_setDesktop(new DPushButton(tr("Set as Home screen")))
    , m_setLock(new DPushButton(tr("Set as Lock screen")))
    , m_setBoth(new DPushButton(tr("Set Both")))
{
    m_paperPaths = QDir(WallPaperPath).entryList(QStringList() << "*.jpg", QDir::Files);
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(1000);

    m_toPrevious->setIcon(QPixmap(":/icons/deepin/builtin/actions/arrows_left.svg"));
    m_toPrevious->setFixedSize(30, 30);
    m_toPrevious->setOpacity(40);
    m_toNext->setIcon(QPixmap(":/icons/deepin/builtin/actions/arrows_right.svg"));
    m_toNext->setFixedSize(30, 30);
    m_toNext->setOpacity(40);

    m_cancel->setIcon(QPixmap(":/icons/deepin/builtin/actions/icon_no.svg"));
    m_cancel->setFixedSize(30, 30);
    m_cancel->setOpacity(200);
    m_cancel->setRadius(15);
    m_confirm->setIcon(QPixmap(":/icons/deepin/builtin/actions/icon_yes.svg"));
    m_confirm->setFixedSize(30, 30);
    m_confirm->setOpacity(200);
    m_confirm->setRadius(15);

    // 向前/向后预览按钮布局
    QWidget *switchWidget = new QWidget;
    QHBoxLayout *switchLayout = new QHBoxLayout(switchWidget);
    switchLayout->setContentsMargins(0, 0, 0, 0);
    switchLayout->addWidget(m_toPrevious, 0, Qt::AlignBottom);
    switchLayout->addStretch();
    switchLayout->addWidget(m_toNext, 0, Qt::AlignBottom);

    // 确定/取消按钮布局
    QWidget *chooseWidget = new QWidget;
    QHBoxLayout *chooseLayout = new QHBoxLayout(chooseWidget);
    chooseLayout->setContentsMargins(0, 0, 0, 0);
    chooseLayout->addStretch();
    chooseLayout->addWidget(m_cancel, 0, Qt::AlignBottom);
    chooseLayout->addSpacing(30);
    chooseLayout->addWidget(m_confirm, 0, Qt::AlignBottom);
    chooseLayout->addStretch();

    QVBoxLayout *previewLayout = new QVBoxLayout(m_previewWidget);
    previewLayout->setContentsMargins(MarginWidth, 0, MarginWidth, MarginWidth);
    previewLayout->addWidget(switchWidget);
    previewLayout->addWidget(chooseWidget);

    // 设置控件布局
    QVBoxLayout *settingLayout = new QVBoxLayout(m_settingsWidget);
    m_settingsWidget->setVisible(false);
    settingLayout->addStretch();
    settingLayout->setContentsMargins(0, 0, 0, 0);
    m_setDesktop->setFixedSize(SettingWidth, SettingHeight);
    settingLayout->addWidget(m_setDesktop, 0, Qt::AlignHCenter);
    m_setLock->setFixedSize(SettingWidth, SettingHeight);
    settingLayout->addWidget(m_setLock, 0, Qt::AlignHCenter);
    m_setBoth->setFixedSize(SettingWidth, SettingHeight);
    settingLayout->addWidget(m_setBoth, 0, Qt::AlignHCenter);
    settingLayout->addSpacing(MarginWidth);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_previewWidget);
    mainLayout->addWidget(m_settingsWidget);

    initConnect();
}

void PaperDetailWidget::initConnect()
{
    connect(m_toPrevious, &PaperIconButton::clicked, this, &PaperDetailWidget::switchToPrevious);
    connect(m_toNext, &PaperIconButton::clicked, this, &PaperDetailWidget::switchToNext);
    connect(m_cancel, &PaperIconButton::clicked, this, &PaperDetailWidget::cancelClicked);
    connect(m_confirm, &PaperIconButton::clicked, this, &PaperDetailWidget::onConfirmClicked);
    connect(m_hideTimer, &QTimer::timeout, this, &PaperDetailWidget::hideSwitchButton);
    connect(this, &PaperDetailWidget::wallPaperClicked, this, &PaperDetailWidget::onWallpaperClicked);
    connect(m_setDesktop, &DPushButton::clicked, this, &PaperDetailWidget::onSetDesktop);
    connect(m_setLock, &DPushButton::clicked, this, &PaperDetailWidget::onSetLock);
    connect(m_setBoth, &DPushButton::clicked, this, &PaperDetailWidget::onSetBoth);
}

void PaperDetailWidget::initVisibility()
{
    m_toPrevious->setVisible(m_curIndex > 0);
    m_toNext->setVisible(m_curIndex < m_paperPaths.size() - 1);
    m_hideTimer->start();
    update();
}

void PaperDetailWidget::paintEvent(QPaintEvent *event)
{
    if (m_pixmap.isNull()) {
        return;
    }

    const QRect &rect = contentsRect();
    QPixmap scalePix;
    if (m_pixmap.width() * rect.height() <= rect.width() * m_pixmap.height()) {
        scalePix = m_pixmap.scaledToWidth(rect.width());
    } else {
        scalePix = m_pixmap.scaledToHeight(rect.height());
    }

    QPainter painter(this);
    QWidget::style()->drawItemPixmap(&painter, rect, Qt::AlignCenter, scalePix);
    DWidget::paintEvent(event);
}

void PaperDetailWidget::mousePressEvent(QMouseEvent *event)
{
    Q_EMIT wallPaperClicked();
    m_pressPos = event->pos();
    DWidget::mousePressEvent(event);
}

void PaperDetailWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        DWidget::mouseReleaseEvent(event);
        return;
    }

    const QPoint &releasePos = event->pos();
    // 滑动大于45度或横向滑动距离小于指定像素返回
    if ((qAbs(m_pressPos.x() - releasePos.x()) < qAbs(m_pressPos.y() - releasePos.y()))
            || (qAbs(m_pressPos.x() - releasePos.x()) < 100)) {
        return;
    }

    if (m_pressPos.x() > releasePos.x()) {
        switchToNext();
    } else {
        switchToPrevious();
    }

    DWidget::mouseReleaseEvent(event);
}

void PaperDetailWidget::showEvent(QShowEvent *event)
{
    initVisibility();
    DWidget::showEvent(event);
}

void PaperDetailWidget::setPixmapIndex(const int index)
{
    if (index < 0 || index >= m_paperPaths.size()) {
        qWarning() << "set invalid index:" << index << ", size:" << m_paperPaths.size();
        return;
    }
    m_curIndex = index;
    m_pixmap = QPixmap(WallPaperPath + m_paperPaths.at(index));
    update();
}

const QStringList &PaperDetailWidget::allWallpaperPaths() const
{
    return m_paperPaths;
}

void PaperDetailWidget::hideSwitchButton()
{
    m_toPrevious->setVisible(false);
    m_toNext->setVisible(false);
    update();
}

void PaperDetailWidget::switchToNext()
{
    if (m_curIndex >= m_paperPaths.size() - 1) {
        return;
    }

    ++m_curIndex;
    m_pixmap = QPixmap(WallPaperPath + m_paperPaths.at(m_curIndex));
    initVisibility();
}

void PaperDetailWidget::switchToPrevious()
{
    if (m_curIndex <= 0) {
        return;
    }

    --m_curIndex;
    m_pixmap = QPixmap(WallPaperPath + m_paperPaths.at(m_curIndex));
    initVisibility();
}

void PaperDetailWidget::onWallpaperClicked()
{
    if (m_previewWidget->isVisible()) {
        initVisibility();
    } else {
        m_previewWidget->setVisible(true);
        m_settingsWidget->setVisible(false);
        initVisibility();
    }
}

void PaperDetailWidget::onConfirmClicked()
{
    m_previewWidget->setVisible(false);
    m_settingsWidget->setVisible(true);
}

void PaperDetailWidget::onSetDesktop()
{
    if (m_curIndex < 0 || m_curIndex >= m_paperPaths.size()) {
        return;
    }

    Q_EMIT requestSetDesktop(WallPaperPath + m_paperPaths.at(m_curIndex));
    Q_EMIT finishSetWallpaper();
}

void PaperDetailWidget::onSetLock()
{
    if (m_curIndex < 0 || m_curIndex >= m_paperPaths.size()) {
        return;
    }

    Q_EMIT requestSetLock(WallPaperPath + m_paperPaths.at(m_curIndex));
    Q_EMIT finishSetWallpaper();
}

void PaperDetailWidget::onSetBoth()
{
    if (m_curIndex < 0 || m_curIndex >= m_paperPaths.size()) {
        return;
    }

    const QString &newPaperPath = WallPaperPath + m_paperPaths.at(m_curIndex);
    Q_EMIT requestSetDesktop(newPaperPath);
    Q_EMIT requestSetLock(newPaperPath);
    Q_EMIT finishSetWallpaper();
}
