#ifndef SOUNDCONTENT_H
#define SOUNDCONTENT_H

#include <QWidget>
#include <QLabel>
#include <QFile>
#include <QVBoxLayout>
#include <QDebug>
#include "appframe.h"
#include "deviceframe.h"

class SoundContent : public QWidget
{
    Q_OBJECT
public:
    explicit SoundContent(QWidget *parent = 0);

signals:

public slots:

private:
    void initStyleSheet();
    void initFrame();
};

#endif // SOUNDCONTENT_H
