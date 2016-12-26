#ifndef OPTIONLISTPAGE_H
#define OPTIONLISTPAGE_H

#include "contentwidget.h"

namespace dcc {

namespace widgets {

class OptionListPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit OptionListPage(const QStringList &options, QWidget *parent = 0);

signals:

private:
    const QStringList m_options;
};

}

}

#endif // OPTIONLISTPAGE_H
