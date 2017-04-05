#ifndef INDEXVIEW_H
#define INDEXVIEW_H

#include <QListView>
#include <QLabel>

namespace dcc {
namespace keyboard{
class IndexView : public QListView
{
    Q_OBJECT

public:
    explicit IndexView(QWidget* parent = 0);

public slots:
    void onClick(const QString& ch);

protected:
//    void paintEvent(QPaintEvent* e);
//    void wheelEvent(QWheelEvent *e);
    void showEvent(QShowEvent* e);
    void scrollContentsBy(int dx, int dy);

private:
    QString m_section;
    QLabel* m_label;
};
}
}
#endif // INDEXVIEW_H
