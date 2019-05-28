#ifndef SYNCSTATEICON_H
#define SYNCSTATEICON_H

#include <QLabel>
#include <QVariantAnimation>

namespace dcc {
namespace cloudsync {
class SyncStateIcon : public QLabel
{
    Q_OBJECT
public:
    SyncStateIcon(QWidget* parent = nullptr);
    ~SyncStateIcon();

    void setRotatePixmap(const QPixmap &pixmap);

    void play();
    void stop();

private:
    QLabel* m_centerLbl;
    QPixmap m_rotatePixmap;
    QVariantAnimation m_rotateAni;
};
}  // namespace cloudsync
}  // namespace dcc

#endif  // !SYNCSTATEICON_H
