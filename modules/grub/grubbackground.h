#ifndef GRUBBACKGROUND_H
#define GRUBBACKGROUND_H

#include <QWidget>
#include <QPixmap>

class GrubBackground : public QWidget
{
    Q_OBJECT
public:
    explicit GrubBackground(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap m_background;

    Q_SLOT bool updateBackground(const QString& filename);
};

#endif // GRUBBACKGROUND_H
