#ifndef DTIPSFRAME_H
#define DTIPSFRAME_H

#include <QFrame>
#include <QLabel>

class DTipsFrame : public QFrame
{
    Q_OBJECT
public:
    DTipsFrame();

    void move(int x, int y);

private:

public slots:
    void followTheSender();

private:
    QLabel *m_label;

    bool m_isFirstMove = true;
};

#endif // DTIPSFRAME_H
