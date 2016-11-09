#ifndef MODULEWIDGET_H
#define MODULEWIDGET_H

#include "labels/largelabel.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class ModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleWidget();

    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);

private:
    bool event(QEvent *event);

protected:
    QLabel *m_moduleIcon;
    LargeLabel *m_moduleTitle;
    QVBoxLayout *m_centeralLayout;
};

#endif // MODULEWIDGET_H
