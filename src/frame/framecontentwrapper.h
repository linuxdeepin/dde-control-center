#ifndef FRAMECONTENTWRAPPER_H
#define FRAMECONTENTWRAPPER_H

#include <QWidget>

class FrameContentWrapper : public QWidget
{
    Q_OBJECT

public:
    explicit FrameContentWrapper(QWidget *parent = nullptr);
};

#endif // FRAMECONTENTWRAPPER_H
