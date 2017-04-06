#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include <QWidget>
#include <QGridLayout>

class NavWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NavWidget(QWidget *parent = 0);

signals:
    void requestModule(const QString &module) const;

private:
    void refershGridLayout();

private:
    QGridLayout *m_gridLayout;

    QStringList m_moduleList;
};

#endif // NAVWIDGET_H
