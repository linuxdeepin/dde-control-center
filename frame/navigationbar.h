#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QPointer>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

class NavigationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NavigationBar(QWidget *parent = nullptr);

public slots:
    void setModuleVisible(const QString &module, bool visible);
    void setModuleChecked(const QString &module);

signals:
    void requestModule(const QString &name) const;

private slots:
    void onNavigationButtonClicked();
    void setModuleChecked(DImageButton *button);

private:
    QMap<QString, DImageButton *> m_navigationButtons;
    QPointer<DImageButton> m_checkedButton;
};

#endif // NAVIGATIONBAR_H
