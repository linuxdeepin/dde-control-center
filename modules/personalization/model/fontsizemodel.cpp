#include "fontsizemodel.h"
using namespace dcc;
using namespace dcc::personalization;
FontSizeModel::FontSizeModel(QObject *parent) : QObject(parent)
{

}

void FontSizeModel::setFontSize(const int size)
{
    if (m_size) {
        m_size = size;
        emit sizeChanged(size);
    }
}
