#ifndef THEMEITEMPIC_H
#define THEMEITEMPIC_H

#include <QWidget>

namespace dcc
{
namespace widgets
{
class TranslucentFrame;
}
namespace personalization
{
class ThemeItemPic : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeItemPic(QWidget *parent = 0);
    void setPicPath(const QString &path);

    void paintEvent(QPaintEvent *event);

private:
    QString m_picPath;

};


}
}


#endif // THEMEITEMPIC_H
