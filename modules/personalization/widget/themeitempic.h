#ifndef THEMEITEMPIC_H
#define THEMEITEMPIC_H

#include "translucentframe.h"
#include <QObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>
#include <QBitmap>
#include <QPaintEvent>
#include <QPainter>
namespace dcc
{
namespace widgets
{
class TranslucentFrame;
}
namespace personalization
{
class ThemeItemPic : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit ThemeItemPic(const QString &path, QWidget *parent = 0);

private:
    QVBoxLayout  *m_mainLayout;
    QLabel       *m_show;

};
}
}


#endif // THEMEITEMPIC_H
