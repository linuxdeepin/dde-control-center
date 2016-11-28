#include "fontsizemodel.h"

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
