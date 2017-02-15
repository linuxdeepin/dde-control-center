#ifndef PLANTEXTITEM_H
#define PLANTEXTITEM_H

#include "settingsitem.h"

#include <QPlainTextEdit>

namespace dcc {

namespace widgets {

class PlainTextItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit PlainTextItem(QWidget *parent = 0);

    QPlainTextEdit *plainEdit() const;

private:
    QPlainTextEdit *m_plainEdit;
};

}

}

#endif // PLANTEXTITEM_H
