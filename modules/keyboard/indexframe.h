#ifndef INDEXFRAME_H
#define INDEXFRAME_H

#include "translucentframe.h"
#include <QRect>

using namespace dcc;

namespace dcc {
namespace keyboard{

class IndexFrame : public dcc::widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit IndexFrame(QFrame* parent = 0);
    void setLetters(const QStringList& lists);

protected:
    void paintEvent(QPaintEvent* );
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void resizeEvent(QResizeEvent *);
    void leaveEvent(QEvent *event);

signals:
    void click(const QString& ch);

private:
    QVector<QString> m_letters;
    int m_w;
    int m_index;
    QRect m_rect;
    bool m_pressed;
};

}
}
#endif // INDEXFRAME_H
