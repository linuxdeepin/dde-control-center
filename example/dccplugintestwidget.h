#ifndef DCCPLUGINTESTWIDGET_H
#define DCCPLUGINTESTWIDGET_H

#include <QWidget>
#include <QStack>

#include <interface/frameproxyinterface.h>

#include <DMainWindow>

class QPushButton;
class DCCNetworkModule;
class QHBoxLayout;
class FourthColWidget;
class QMouseEvent;
class QPaintEvent;
class QStandardItemModel;
class QTranslator;

namespace dcc {
  namespace widgets {
    class MultiSelectListView;
  }
}

DWIDGET_BEGIN_NAMESPACE
class DBackgroundGroup;
DWIDGET_END_NAMESPACE

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class DccPluginTestWidget : public DMainWindow, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit DccPluginTestWidget(QWidget *parent = nullptr);
    ~DccPluginTestWidget();

    void pushWidget(ModuleInterface *const inter, QWidget *const w, PushType type = Normal);

    void popWidget(ModuleInterface *const inter);

    void setModuleVisible(ModuleInterface *const inter, const bool visible);

    void showModulePage(const QString &module, const QString &page, bool animation);

    void setModuleSubscriptVisible(const QString &module, bool bIsDisplay);

    void setRemoveableDeviceStatus(QString type, bool state);
    bool getRemoveableDeviceStatus(QString type) const;

    void setSearchPath(ModuleInterface *const inter) const;

private slots:
    void onBack();
    void onFirstItemClick(const QModelIndex &index);

private:
    void initNetworkModule();

    void replaceThirdWidget(ModuleInterface *const inter, QWidget *const w);
    void pushNormalWidget(ModuleInterface *const inter, QWidget *const w);
    void popAllWidgets(int place = 0);
    void resetNavList(bool isIconMode);
    void judgeTopWidgetPlace(ModuleInterface *const inter, QWidget *const w);
    void popWidget();
    void pushTopWidget(ModuleInterface *const inter, QWidget *const w);
    void resetTabOrder();
    void findFocusChild(QWidget *w, QWidget *&pre);
    void findFocusChild(QLayout *l, QWidget *&pre);
    void pushFinalWidget(ModuleInterface *const inter, QWidget *const w);
    void updateViewBackground();

    void updateWinsize();

private:
    DCCNetworkModule *m_module;

    QWidget *m_lastPushWidget{nullptr};
    QStack<QPair<ModuleInterface *, QWidget *>> m_contentStack;
    QPair<ModuleInterface *, QWidget *> m_lastThirdPage;
    QHBoxLayout *m_rightContentLayout;
    FourthColWidget *m_topWidget;
    dcc::widgets::MultiSelectListView *m_navView;
    struct CornerItemGroup {
        QString m_name;
        int m_index;
        QPair <DViewItemAction *, DViewItemAction *> m_action;
    };
    QList<CornerItemGroup> m_remindeSubscriptList;//用于记录是否有角标, QString为模块名称，DViewItemAction为角标对象
    QStandardItemModel *m_navModel;
    QList<QPair<ModuleInterface *, QString>> m_modules;
    DBackgroundGroup *m_rightView;
    DIconButton *m_backwardBtn;

    QTranslator *m_translator;

    QStringList m_removeableDeviceList;

    bool m_isFinalWidget;
    bool m_isFromSecondAddWidget;
};

class FourthColWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FourthColWidget(QWidget *parent = nullptr);
    void initWidget(QWidget *showWidget, ModuleInterface *module = nullptr);

    inline QWidget *curWidget() const { return m_curWidget; }
    inline ModuleInterface *curInterface() const { return m_curInterface; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

protected slots:
    void onCurrentWidgetDestroy();

Q_SIGNALS:
    void signalBack();

private:
    QWidget *m_curWidget{nullptr};
    ModuleInterface *m_curInterface{nullptr};
};


#endif // DCCPLUGINTESTWIDGET_H
