// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef WIRELESSCASTINGITEM_H
#define WIRELESSCASTINGITEM_H

#include <QWidget>

#define WIRELESS_CASTING_KEY "wireless-casting-item-key"

class QPixmap;
class QLabel;
class WirelessCastingModel;
class DisplayModel;
namespace dde {
namespace wirelesscasting {
class QuickPanelWidget;
class WirelessCastingApplet;

class WirelessCastingItem : public QWidget
{
    Q_OBJECT
public:
    WirelessCastingItem(QWidget *parent = nullptr);
    ~WirelessCastingItem() override;

    QWidget *quickPanelWidget() const;
    QWidget *trayIcon();
    QWidget *appletWidget() const;
    QWidget *refreshButton() const;
    QLabel *tips();
    bool canCasting() const{ return m_canCasting; }
    void setAppletMinHeight(int minHeight);
    const QString contextMenu() const;
    void invokeMenuItem(const QString menuId, const bool checked);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watcher, QEvent *event) override;

public Q_SLOTS:

signals:
    void canCastingChanged(bool canCasting);
    void requestExpand();
    void requestHideApplet();

private:
    void init();

    WirelessCastingModel *m_model;
    DisplayModel *m_displayMode;
    QuickPanelWidget *m_quickPanel;
    WirelessCastingApplet *m_appletWidget;
    QIcon *m_trayIcon;
    QLabel *m_tips;
    bool m_canCasting = false;
};

} // namespace wirelesscasting
} // namespace dde
#endif // WIRELESSCASTINGITEM_H
