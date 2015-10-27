#ifndef SEPARATORWIDGET_H
#define SEPARATORWIDGET_H

#include <QFrame>

class SeparatorWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SeparatorWidget(QWidget *parent = 0);
};

class HSeparatorWidget : public SeparatorWidget
{
    Q_OBJECT
public:
    explicit HSeparatorWidget(QWidget *parent = 0);
};

class VSeparatorWidget : public SeparatorWidget
{
    Q_OBJECT
public:
    explicit VSeparatorWidget(QWidget *parent = 0);
};

#endif // SEPARATORWIDGET_H
