#include "normallabel.h"

namespace dcc {
namespace widgets {

NormalLabel::NormalLabel(QWidget *parent)
    : QLabel(parent)
{

}

NormalLabel::NormalLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{

}

}
}
