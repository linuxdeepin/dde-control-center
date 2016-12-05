#include "quickswitchbutton.h"

#define SIZE 30

QuickSwitchButton::QuickSwitchButton(const int index, QWidget *parent)
    : QLabel(parent),

      m_index(index)
{
    setStyleSheet("background-color: red;");
    setFixedSize(SIZE, SIZE);
}
