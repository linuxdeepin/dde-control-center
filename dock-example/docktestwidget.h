#ifndef DOCKTESTWIDGET_H
#define DOCKTESTWIDGET_H

#include <QWidget>
#include <pluginproxyinterface.h>

class NetworkPlugin;

class DockTestWidget : public QWidget, public PluginProxyInterface
{
    Q_OBJECT

public:
    explicit DockTestWidget(QWidget *parent = nullptr);
    ~DockTestWidget();

private:
    void initDock();

    bool eventFilter(QObject *object, QEvent *event);
    void enterEvent(QEvent *event);

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

private:
    NetworkPlugin *m_networkPlugin;
};

#endif // DOCKTESTWIDGET_H
