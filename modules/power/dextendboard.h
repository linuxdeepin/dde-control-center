#ifndef DEXTENDBOARD_H
#define DEXTENDBOARD_H

#include <QtCore/QStringList>
#include <QtCore/QPropertyAnimation>
#include <QtPlugin>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QResizeEvent>
#include <libdui/dbuttongrid.h>
#include <libdui/dseparatorhorizontal.h>

#include "constants.h"
#include "trianglerectangle.h"

DUI_USE_NAMESPACE
class DExtendBoard : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(int height READ height WRITE setHeight)
public:
    DExtendBoard(QFrame *parent = 0);
    ~DExtendBoard();

    bool expand = false;
    bool value = false;
    bool firstExpand = true;
    void setExpand(bool expand);
    int ex_width = 200;
    int ex_height = 60;

    void setWidth(int ex_width);
    void setHeight(int ex_height);
    void resizeEvent(QResizeEvent *e);
    DButtonGrid* m_suspendButtonGrid;
    DButtonGrid* m_poweroffButtonGrid;

private:
    QHBoxLayout* customPowerOffLayout;
    QHBoxLayout* powerOffLayout;
    QHBoxLayout* suspendLayout;
    QLabel* m_label;
    TriangleRectangle* m_guideExtendTriangle;
    QVBoxLayout* m_contentLayout;
    QLabel* m_customLabel;
    QLabel* m_customChooseLabel;
    QLabel* m_poweroffLabel;

    QLabel* m_suspendLabel;
    QStringList m_suspendButtons;
    QStringList m_poweroffButtons;
    DSeparatorHorizontal* m_bottomSeparator;
    QPropertyAnimation* m_animation=NULL;

};

#endif // DEXTENDBOARD_H

