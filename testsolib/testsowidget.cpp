#include "testsowidget.h"
#include "thirdpage1.h"

#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QListView>

using namespace DCC_NAMESPACE;

TestsoWidget::TestsoWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("Testso");
    QVBoxLayout *contentLayout = new QVBoxLayout(this);
    m_listView = new QListView(this);
    contentLayout->setMargin(0);
    contentLayout->addWidget(m_listView);
    setLayout(contentLayout);
}

void TestsoWidget::initUI()
{
    QStandardItemModel *listviewModel = new QStandardItemModel(m_listView);
    QList<QPair<QIcon, QString>> menuIconText;
    menuIconText = {
        { QIcon::fromTheme("dcc_general_purpose"), tr("General")},
        //~ contents_path /mouse/Mouse
        { QIcon::fromTheme("dcc_mouse"), tr("Mouse")},
        //~ contents_path /mouse/Touchpad
        { QIcon::fromTheme("dcc_touchpad"), tr("Touchpad")},
        //~ contents_path /mouse/TrackPoint
        { QIcon::fromTheme("dcc_trackpoint"), tr("TrackPoint")}
    };
    QStandardItem *testSoItem = nullptr;
    for (auto it = menuIconText.cbegin(); it != menuIconText.cend(); ++it) {
        testSoItem = new QStandardItem(it->first, it->second);
        listviewModel->appendRow(testSoItem);
    }
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setModel(listviewModel);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setCurrentIndex(listviewModel->index(0, 0));
    connect(m_listView, &QListView::clicked, this, &TestsoWidget::onItemClieck);
    connect(m_listView, &QListView::activated, m_listView, &QListView::clicked);
}

void TestsoWidget::onItemClieck(const QModelIndex &index)
{
    switch (index.row()) {
    case 0:
        Q_EMIT showPage1();
        break;
    case 1:
        Q_EMIT showPage2();
        break;
    case 2:
        Q_EMIT showPage3();
        break;
    case 3:
        Q_EMIT showPage4();
        break;
    default:
        Q_EMIT showPage1();
        break;
    }
}
