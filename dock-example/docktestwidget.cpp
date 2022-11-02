// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "docktestwidget.h"
#include "networkplugin.h"

#include <QHBoxLayout>

QPointer<DockPopupWindow> DockTestWidget::PopupWindow = nullptr;
Dock::Position DockTestWidget::DockPosition = Dock::Position::Top;

DockTestWidget::DockTestWidget(QWidget *parent)
    : QWidget(parent)
    , m_pluginInter(new NetworkPlugin(this))
    , m_itemKey(NETWORK_KEY)
{
    initDock();
}

DockTestWidget::~DockTestWidget()
{
}

void DockTestWidget::initDock()
{
    m_pluginInter->init(this);
    QWidget *pluginWidget = m_pluginInter->itemWidget(NETWORK_KEY);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 0, 20, 0);
    layout->addWidget(pluginWidget);
    layout->addStretch();
    pluginWidget->installEventFilter(this);
    this->installEventFilter(this);

    if (PopupWindow.isNull()) {
        DockPopupWindow *arrowRectangle = new DockPopupWindow(nullptr);
        arrowRectangle->setShadowBlurRadius(20);
        arrowRectangle->setRadius(6);
        arrowRectangle->setShadowYOffset(2);
        arrowRectangle->setShadowXOffset(0);
        arrowRectangle->setArrowWidth(18);
        arrowRectangle->setArrowHeight(10);
        arrowRectangle->setObjectName("systemtraypopup");
        PopupWindow = arrowRectangle;
        connect(qApp, &QApplication::aboutToQuit, PopupWindow, &DockPopupWindow::deleteLater);
    }

    // 必须初始化父窗口，否则当主题切换之后再设置父窗口的时候palette会更改为主题切换前的palette
    if (QWidget *w = m_pluginInter->itemPopupApplet(NETWORK_KEY)) {
        w->setParent(PopupWindow.data());
        w->setVisible(false);
    }
}

bool DockTestWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Enter) {
        showHoverTips();
        return true;
    }
    if (event->type() == QEvent::Leave) {
        qInfo()<<this<<event->type();
        // auto hide if popup is not model window
        if (m_popupShown && !PopupWindow->model())
            hidePopup();
        return true;
    }
    if (event->type() == QEvent::MouseButtonPress) {qInfo()<<this<<event->type();
        // auto hide if popup is not model window
        if (m_popupShown && !PopupWindow->model())
            hidePopup();

        return true;
    }
    if (event->type() == QEvent::MouseButtonRelease) {qInfo()<<this<<event->type();
        if (m_pluginInter->itemPopupApplet(m_itemKey)) {
            m_pluginInter->itemPopupApplet(m_itemKey)->setAccessibleName(m_pluginInter->pluginName());
        }
        // auto hide if popup is not model window
        showPopupApplet(m_pluginInter->itemPopupApplet(m_itemKey));
        return true;
    }

    return QWidget::eventFilter(object, event);
}

void DockTestWidget::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    QWidget *tip = m_pluginInter->itemTipsWidget(NETWORK_KEY);
    if (tip) {
        tip->setFixedSize(200, 300);
        tip->show();
    }
}

void DockTestWidget::itemAdded(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
}

void DockTestWidget::itemUpdate(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
}

void DockTestWidget::itemRemoved(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
}

void DockTestWidget::requestWindowAutoHide(PluginsItemInterface * const itemInter, const QString &itemKey, const bool autoHide)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
    Q_UNUSED(autoHide);
}

void DockTestWidget::requestRefreshWindowVisible(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
}

void DockTestWidget::requestSetAppletVisible(PluginsItemInterface * const itemInter, const QString &itemKey, const bool visible)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(itemKey);
    if (visible) {
        showPopupApplet(m_pluginInter->itemPopupApplet(m_itemKey));
    } else {
        hidePopup();
    }
}

void DockTestWidget::saveValue(PluginsItemInterface * const itemInter, const QString &key, const QVariant &value)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(key);
    Q_UNUSED(value);
}

const QVariant DockTestWidget::getValue(PluginsItemInterface * const itemInter, const QString &key, const QVariant &fallback)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(key);
    Q_UNUSED(fallback);
    if (key == "enabled")
        return true;

    return QVariant();
}

void DockTestWidget::removeValue(PluginsItemInterface * const itemInter, const QStringList &keyList)
{
    Q_UNUSED(itemInter);
    Q_UNUSED(keyList);
}

void DockTestWidget::showPopupWindow(QWidget * const content, const bool model)
{
    m_popupShown = true;
    m_lastPopupWidget = content;

    if (model)
        emit requestWindowAutoHide(false);

    DockPopupWindow *popup = PopupWindow.data();
    QWidget *lastContent = popup->getContent();
    if (lastContent)
        lastContent->setVisible(false);

    switch (DockPosition) {
    case Dock::Position::Top:   popup->setArrowDirection(DockPopupWindow::ArrowTop);     break;
    case Dock::Position::Bottom: popup->setArrowDirection(DockPopupWindow::ArrowBottom);  break;
    case Dock::Position::Left:  popup->setArrowDirection(DockPopupWindow::ArrowLeft);    break;
    case Dock::Position::Right: popup->setArrowDirection(DockPopupWindow::ArrowRight);   break;
    }
    popup->resize(content->sizeHint());
    popup->setContent(content);

    QPoint p = popupMarkPoint();
    if (!popup->isVisible())
        QMetaObject::invokeMethod(popup, "show", Qt::QueuedConnection, Q_ARG(QPoint, p), Q_ARG(bool, model));
    else
        popup->show(p, model);

    connect(popup, &DockPopupWindow::accept, this, &DockTestWidget::popupWindowAccept, Qt::UniqueConnection);
}

void DockTestWidget::showHoverTips()
{
    // another model popup window already exists
    if (PopupWindow->model())
        return;

    // if not in geometry area
    const QRect r(topleftPoint(), size());
    if (!r.contains(QCursor::pos()))
        return;

    QWidget *const content = trayTipsWidget();
    if (!content)
        return;

    showPopupWindow(content);
}

const QPoint DockTestWidget::popupMarkPoint() const
{
    QPoint p(topleftPoint());

    const QRect r = rect();
    const QRect wr = window()->rect();

    switch (DockPosition) {
    case Dock::Position::Top:
        p += QPoint(r.width() / 2, r.height() + (wr.height() - r.height()) / 2);
        break;
    case Dock::Position::Bottom:
        p += QPoint(r.width() / 2, 0 - (wr.height() - r.height()) / 2);
        break;
    case Dock::Position::Left:
        p += QPoint(r.width() + (wr.width() - r.width()) / 2, r.height() / 2);
        break;
    case Dock::Position::Right:
        p += QPoint(0 - (wr.width() - r.width()) / 2, r.height() / 2);
        break;
    }

    return p;
}
// 获取在最外层的窗口(MainWindow)中的位置
const QPoint DockTestWidget::topleftPoint() const
{
    QPoint p;
    const QWidget *w = this;
    do {
        p += w->pos();
        w = qobject_cast<QWidget *>(w->parent());
    } while (w);

    return p;
}

void DockTestWidget::popupWindowAccept()
{
    if (!PopupWindow->isVisible())
        return;

    disconnect(PopupWindow.data(), &DockPopupWindow::accept, this, &DockTestWidget::popupWindowAccept);

    hidePopup();
}

void DockTestWidget::showPopupApplet(QWidget * const applet)
{
    if (!applet)
        return;

    // another model popup window already exists
    if (PopupWindow->model()) {
        applet->setVisible(false);
        return;
    }

    showPopupWindow(applet, true);
}

void DockTestWidget::hidePopup()
{
    m_popupShown = false;
    PopupWindow->hide();

    DockPopupWindow *popup = PopupWindow.data();
    QWidget *content = popup->getContent();
    if (content) {
        content->setVisible(false);
    }

    emit PopupWindow->accept();
    emit requestWindowAutoHide(true);
}

QWidget *DockTestWidget::trayTipsWidget()
{
    if (m_pluginInter->itemTipsWidget(m_itemKey)) {
        m_pluginInter->itemTipsWidget(m_itemKey)->setAccessibleName(m_pluginInter->pluginName());
    }

    return m_pluginInter->itemTipsWidget(m_itemKey);
}
