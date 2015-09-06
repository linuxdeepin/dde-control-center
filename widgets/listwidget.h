#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QVBoxLayout>
#include <QMap>

#include <libdui/dscrollarea.h>
#include <libdui/libdui_global.h>

DUI_USE_NAMESPACE

class ListWidget : public DScrollArea
{
    Q_OBJECT

    Q_PROPERTY(int firstChecked READ firstChecked NOTIFY firstCheckedChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int visibleCount READ visibleCount NOTIFY visibleCountChanged FINAL)
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable)
    Q_PROPERTY(CheckMode checkMode READ checkMode)

    Q_ENUMS(CheckMode)
public:
    enum CheckMode{
        Radio,
        MultipleCheck
    };

    explicit ListWidget(CheckMode checkMode = Radio, QWidget *parent = 0);

public slots:
    int addWidget(QWidget *w);
    void addWidgets(const QList<QWidget*> &ws);
    void insertWidget(int index, QWidget *w);
    void insertWidgets(int index, const QList<QWidget*> &ws);
    void setItemSize(int w, int h);
    void clear(bool isDelete = true);
    void removeWidget(int index, bool isDelete = true);
    void showWidget(int index);
    void hideWidget(int index);
    void setChecked(int index, bool checked);
    void setCheckMode(CheckMode checkMode);
    void setCheckable(bool checkable);

public:
    int count() const;
    int indexOf(const QWidget* widget);
    QWidget *getWidget(int index) const;
    int firstChecked() const;
    QList<int> checkedList() const;
    bool checkable() const;
    int visibleCount() const;
    CheckMode checkMode() const;
    QList<QWidget*> widgetList() const;
    QSize itemSize() const;

signals:
    void firstCheckedChanged(int index);
    void checkedChanged(int index, bool checked);
    void countChanged();
    void checkableChanged(bool checkable);
    void visibleCountChanged(int visibleCount);

protected:
    bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;
    void setVisibleCount(int count);
    void setHeight(int height);

private:
    int m_itemWidth;
    int m_itemHeight;
    QVBoxLayout *m_layout;
    QList<QWidget*> m_widgetList;
    QList<int> m_checkedList;
    CheckMode m_checkMode;
    QWidget *m_mainWidget;
    int m_visibleCount;
    bool m_checkable;
    QMap<int, bool> m_mapVisible;
};

#endif // LISTWIDGET_H
