// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "popup_menu.h"

#include "file_util.h"
#include "popup_menu_delegate.h"

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QListView>
#include <QPainter>
#include <QPainterPath>
#include <QStringListModel>

namespace installer {

namespace {

const int kBorderRadius = 5;
const int kBorderDiameter = kBorderRadius * 2;

// _____
// \ | /
//  \|/
const int kTriangleHeight = 6;

// Height of menu item is also defined in styles/popup_menu.css
const int kMenuItemHeight = 24;
const int kMenuViewVerticalMargin = 4;
const int kMenuViewBottomPadding = 5;
const int kMenuViewHorizontalMargins = 20;
const int kMenuViewMinimumWidth = 60;

} // namespace

PopupMenu::PopupMenu(QWidget *parent)
    : QFrame(parent)
{
    this->setObjectName("popup_menu");
    this->initUI();
    this->initConnections();
}

QStringList PopupMenu::stringList() const
{
    return menu_model_->stringList();
}

void PopupMenu::popup(const QPoint &pos)
{
    const QSize size = menu_view_->size();
    this->move(pos.x() - size.width() / 2, pos.y() - size.height());
    this->show();

    // Grab global keyboard events when menu window is popup.
    this->grabKeyboard();
}

void PopupMenu::setStringList(const QStringList &strings)
{
    menu_model_->setStringList(strings);

    int item_width = kMenuViewMinimumWidth;
    const QFontMetrics metrics(menu_view_->font());
    for (const QString &str : strings) {
        const int curr_width = metrics.horizontalAdvance(str);
        item_width = (curr_width > item_width) ? curr_width : item_width;
    }
    // Add margin to list view.
    const int width = item_width + kMenuViewHorizontalMargins * 2;

    // Add more space at bottom of list view.
    const int height = kMenuItemHeight * strings.length() + kMenuViewVerticalMargin * 2
            + kMenuViewBottomPadding;

    this->resize(width, height + kTriangleHeight);
    menu_view_->adjustSize();
    menu_view_->resize(width, height);
}

bool PopupMenu::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
        if (mouse_event) {
            // If mouse press event is not happened within menu area, hide menu.
            if (!this->geometry().contains(mouse_event->pos())) {
                this->hide();
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void PopupMenu::hideEvent(QHideEvent *event)
{
    // No need to monitor global mouse event when menu is hidden.
    qApp->removeEventFilter(this);

    this->releaseKeyboard();
    QWidget::hideEvent(event);
    Q_EMIT this->onHide();
}

void PopupMenu::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}

void PopupMenu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    const int kWidth = this->width();
    const int kHalfWidth = kWidth / 2;
    const int kHeight = this->height();

    // First draw background with round corner.
    QPainterPath background_path;
    background_path.moveTo(kWidth, kBorderRadius);
    background_path.arcTo(kWidth - kBorderDiameter, 0, kBorderDiameter, kBorderDiameter, 0.0, 90.0);
    background_path.lineTo(kBorderRadius, 0);
    background_path.arcTo(0, 0, kBorderDiameter, kBorderDiameter, 90.0, 90.0);
    background_path.lineTo(0, kHeight - kBorderRadius - kTriangleHeight);
    background_path.arcTo(0,
                          kHeight - kBorderDiameter - kTriangleHeight,
                          kBorderDiameter,
                          kBorderDiameter,
                          180.0,
                          90.0);
    background_path.lineTo(kHalfWidth - kTriangleHeight, kHeight - kTriangleHeight);
    // Draw isosceles right-angled triangle in bottom center of label.
    background_path.lineTo(kHalfWidth, kHeight);
    background_path.lineTo(kHalfWidth + kTriangleHeight, kHeight - kTriangleHeight);
    background_path.lineTo(kWidth - kBorderRadius, kHeight - kTriangleHeight);
    background_path.arcTo(kWidth - kBorderDiameter,
                          kHeight - kBorderDiameter - kTriangleHeight,
                          kBorderDiameter,
                          kBorderDiameter,
                          270.0,
                          90.0);
    background_path.closeSubpath();
    const QColor background_color(255, 255, 255, 230);
    painter.fillPath(background_path, QBrush(background_color));
}

void PopupMenu::showEvent(QShowEvent *event)
{
    qApp->installEventFilter(this);
    QFrame::showEvent(event);
}

void PopupMenu::initConnections()
{
    connect(menu_view_, &QListView::pressed, this, &PopupMenu::onMenuViewActivated);
}

void PopupMenu::initUI()
{
    menu_model_ = new QStringListModel(this);
    menu_view_ = new QListView(this);
    menu_view_->setObjectName("menu_view");
    menu_view_->setAccessibleName("menu_view");
    menu_view_->setContentsMargins(0, kMenuViewVerticalMargin, 0, kMenuViewVerticalMargin);
    menu_view_->setModel(menu_model_);
    menu_view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    menu_view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    menu_view_->setUniformItemSizes(true);
    menu_view_->setSelectionMode(QListView::SingleSelection);
    menu_view_->setEditTriggers(QListView::NoEditTriggers);
    PopupMenuDelegate *popup_delegate = new PopupMenuDelegate(this);
    menu_view_->setItemDelegate(popup_delegate);
    menu_view_->setMouseTracking(true);
    menu_view_->setStyleSheet(ReadFile(":/icons/deepin/builtin/images/popup_menu.css"));

    this->setContentsMargins(0, 0, 0, 0);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFocusPolicy(Qt::StrongFocus);

    this->setWindowFlags(Qt::Popup);
}

void PopupMenu::onMenuViewActivated(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());
    if (index.isValid()) {
        Q_EMIT this->menuActivated(index.row());
    }
}

} // namespace installer
