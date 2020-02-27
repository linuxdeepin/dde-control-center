#ifndef TESTSOWIDGET_H
#define TESTSOWIDGET_H

#include "interface/namespace.h"
#include "interface/moduleinterface.h"

#include <QWidget>
#include <QListView>

class TestsoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestsoWidget(QWidget *parent = nullptr);
    void initUI();
private:
    void onItemClieck(const QModelIndex &index);
Q_SIGNALS:
    void showPage1();
    void showPage2();
    void showPage3();
    void showPage4();
private:
    DCC_NAMESPACE::FrameProxyInterface *m_frameProxy;
    QListView *m_listView;
};

#endif // TESTSOWIDGET_H
