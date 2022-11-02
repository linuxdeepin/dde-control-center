// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DOCKTESTWIDGET_H
#define DOCKTESTWIDGET_H

#include <QWidget>
#include <pluginproxyinterface.h>
#include "dockpopupwindow.h"

class NetworkPlugin;

class DockTestWidget : public QWidget, public PluginProxyInterface
{
    Q_OBJECT

public:
    explicit DockTestWidget(QWidget *parent = nullptr);
    ~DockTestWidget() override;

private:
    void initDock();

    bool eventFilter(QObject *object, QEvent *event) override;
    void enterEvent(QEvent *event) override;

private:
    void itemAdded(PluginsItemInterface * const itemInter, const QString &itemKey) override;
    void itemUpdate(PluginsItemInterface * const itemInter, const QString &itemKey) override;
    void itemRemoved(PluginsItemInterface * const itemInter, const QString &itemKey) override;
    void requestWindowAutoHide(PluginsItemInterface * const itemInter, const QString &itemKey, const bool autoHide) override;
    void requestRefreshWindowVisible(PluginsItemInterface * const itemInter, const QString &itemKey) override;
    void requestSetAppletVisible(PluginsItemInterface * const itemInter, const QString &itemKey, const bool visible) override;
    void saveValue(PluginsItemInterface * const itemInter, const QString &key, const QVariant &value) override;
    const QVariant getValue(PluginsItemInterface *const itemInter, const QString &key, const QVariant& fallback = QVariant()) override;
    void removeValue(PluginsItemInterface *const itemInter, const QStringList &keyList) override;

protected:
    virtual void showPopupWindow(QWidget * const content, const bool model = false);
    virtual void showHoverTips();
    const QPoint popupMarkPoint() const;
    const QPoint topleftPoint() const;
    void popupWindowAccept();
    void showPopupApplet(QWidget * const applet);
    void hidePopup();
    QWidget *trayTipsWidget();

Q_SIGNALS:
    void requestWindowAutoHide(const bool autoHide);

private:
    NetworkPlugin *m_pluginInter;
    QString m_itemKey;

    bool m_popupShown;
    QPointer<QWidget> m_lastPopupWidget;

    static Dock::Position DockPosition;
    static QPointer<DockPopupWindow> PopupWindow;
};

#endif // DOCKTESTWIDGET_H
