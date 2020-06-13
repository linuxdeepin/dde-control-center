#ifndef MULTISELECTLISTVIEW_H
#define MULTISELECTLISTVIEW_H

#include <DListView>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class MultiSelectListView : public DListView
{
    Q_OBJECT
public:
    explicit MultiSelectListView(QWidget *parent = nullptr);
    void resetStatus(const QModelIndex &index);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    int m_currentIndex;
};

} // namespace widgets
} // namespace dcc

#endif // MULTISELECTLISTVIEW_H
