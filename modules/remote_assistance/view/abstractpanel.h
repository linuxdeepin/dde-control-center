#ifndef ABSTRACTPANEL_H
#define ABSTRACTPANEL_H

#include <QWidget>
#include <QLayoutItem>
#include "../remoteassistance.h"

class ModuleHeader;

class QVBoxLayout;

class AbstractPanel : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractPanel(const QString& title=QLatin1String(""), QWidget *parent = 0);
    AbstractPanel* addWidget(QWidget*);
    AbstractPanel* addLayout(QLayout* l, int stretch=0);

protected:
    void setTitle(const QString&);

protected slots:
    virtual void onNoNetwork();
    virtual void emitChangePanel();
    virtual void abort();

signals:
    void changePanel(ViewPanel);

protected:
    QVBoxLayout* m_viewLayout;
    QWidget* m_view;

    void setWidget(QWidget*);

private:
    ModuleHeader * m_headerLine;
};

#endif // ABSTRACTPANEL_H
