#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class NavigationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NavigationBar(QWidget *parent = nullptr);

public slots:
    void setModuleVisible(const QString &module, bool visible);

signals:
    void requestModule(const QString &name) const;

private slots:
    void onNavigationButtonClicked();

private:
    QMap<QString, DImageButton *> m_navigationButtons;
};

#endif // NAVIGATIONBAR_H
