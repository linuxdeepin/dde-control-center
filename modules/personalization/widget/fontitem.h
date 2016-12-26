#ifndef FONTITEM_H
#define FONTITEM_H

#include "translucentframe.h"
#include <QObject>
#include <QLabel>
#include <QFont>
#include <QWidget>
#include <QHBoxLayout>
#include <QDebug>

namespace dcc
{
namespace personalization
{
class FontItem : public QWidget
{
    Q_OBJECT
public:
    explicit FontItem(const QString &value, QWidget *parent = 0);

private:
    QHBoxLayout *m_mainLayout;
    QLabel      *m_title;
};
}
}


#endif // FONTITEM_H
