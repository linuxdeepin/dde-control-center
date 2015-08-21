#ifndef DTIPSFRAME_H
#define DTIPSFRAME_H

#include <QFrame>
#include <QLabel>
#include <QPropertyAnimation>

class DTipsFrame : public QFrame
{
    Q_OBJECT
public:
    DTipsFrame();
    ~DTipsFrame();

    void move(int x, int y);

private:

public slots:
    void followTheSender();
    void setTipsText(const QString & text);

private:
    QLabel *m_label;

    QPropertyAnimation *m_moveAni;

    bool m_isFirstMove = true;

    const int m_extraOffsetX = -8;
    const int m_extraOffsetY = 0;
};

#endif // DTIPSFRAME_H
