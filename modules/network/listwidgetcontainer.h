#ifndef LISTWIDGETCONTAINER_H
#define LISTWIDGETCONTAINER_H

#include <QWidget>

#include <libdui/dboxwidget.h>
#include <libdui/libdui_global.h>

DUI_USE_NAMESPACE

class ListWidgetContainer : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit ListWidgetContainer(const QString &title, QWidget *parent = 0);

    void addWidget(QWidget *widget);
    void setBoxWidgetContentsMargins(int left, int top, int right, int bottom);
    void setButtonsVisible(bool visible);

signals:
    void setTitle(const QString &title);
    void setLeftButtonText(const QString &text);
    void setRightButtonText(const QString &text);
    void setLeftButtonVisible(bool visible);
    void setRightButtonVisible(bool visible);
    void leftButtonClicked();
    void rightButtonClicked();

private:
    void init(const QString &title);

    DVBoxWidget *m_boxWidget;
    QHBoxLayout *m_buttonLayout;
};

#endif // LISTWIDGETCONTAINER_H
