#include "smalllabel.h"

namespace dcc {
namespace widgets {

SmallLabel::SmallLabel(QWidget *parent)
    : QLabel(parent)
{

}

SmallLabel::SmallLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{

}

}
}
