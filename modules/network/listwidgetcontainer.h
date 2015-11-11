#ifndef LISTWIDGETCONTAINER_H
#define LISTWIDGETCONTAINER_H

#include <QWidget>

#include <libdui/dboxwidget.h>
#include <libdui/libdui_global.h>
#include <libdui/dlistwidget.h>

DUI_USE_NAMESPACE

class ListWidgetContainer : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit ListWidgetContainer(const QString &title, QWidget *parent = 0);

    void addWidget(QWidget *widget);
    void widgetChecke(int index);
    int checkedWidgetIndex() const;
    void setCheckable(bool checkable);
signals:
    void setTitle(const QString &title);
    void setLeftButtonText(const QString &text);
    void setRightButtonText(const QString &text);
    void setButtonsVisible(bool visible);
    void leftButtonClicked();
    void rightButtonClicked();

private:
    void init(const QString &title);

    DListWidget *m_listWidget;
};

#endif // LISTWIDGETCONTAINER_H
