#ifndef VLAYOUTWIDGET_H
#define VLAYOUTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class VLayoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VLayoutWidget(QWidget *parent = 0);
    void addWidget(QWidget *w, int height, Qt::Alignment alignment = 0);

private:
    QVBoxLayout *m_layout;
};

#endif // VLAYOUTWIDGET_H
