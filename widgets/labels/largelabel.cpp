#include "largelabel.h"

namespace dcc {
namespace widgets {

LargeLabel::LargeLabel(QWidget *parent)
    : QLabel(parent)
{

}

LargeLabel::LargeLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{

}

}
}
