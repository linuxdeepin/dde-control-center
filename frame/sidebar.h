#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QFrame>
#include <QAbstractButton>
#include <QVBoxLayout>

#include "modulemetadata.h"
#include "dtipsframe.h"
#include "pluginsmanager.h"

class SideBarButton;
class SideBar : public QFrame
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = 0);

    void switchToModule(const QString &pluginId);
    DTipsFrame *getTipFrame() const;

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void switchToSideBarButton(SideBarButton *btn);
    void addSideBarButton(const ModuleMetaData& meta);

signals:
    void moduleSelected(ModuleMetaData meta);

private slots:
    void onSideBarButtonClicked();

private:
    SideBarButton *m_selectedBtn = nullptr;
    DTipsFrame *m_tips;
    QVBoxLayout *m_sidebarLayout;
    PluginsManager *m_pluginsManager;
};


class QLabel;
class QEvent;
class QMouseEvent;
class SideBarButton : public QAbstractButton
{
    Q_OBJECT

private:
    enum State { Normal, Hover, Selected };

public:
    SideBarButton(ModuleMetaData metaData, QWidget *parent = 0);

    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

    void presse();
    void release();

    inline const ModuleMetaData&& metaData() const {return std::move(m_meta);}

private:
    void setState(State m_state);

signals:
    void clicked();
    void hovered();

private:
    State m_state;
    ModuleMetaData m_meta;
    QLabel *m_icon;
};

#endif // SIDEBAR_H
