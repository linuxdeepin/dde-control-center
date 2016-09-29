#include "frame.h"

#include <QScreen>
#include <QApplication>

Frame::Frame(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(300, 900);
    move(qApp->primaryScreen()->geometry().center() - rect().center());
}
