#ifndef DISPLAYCONTROLPAGE_H
#define DISPLAYCONTROLPAGE_H

#include "display/displaymodel.h"
#include "display/displayworker.h"

#include <QWidget>
#include <QModelIndex>

class DisplayControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayControlPage(dcc::display::DisplayModel *model, QWidget *parent = 0);

signals:
    void mouseLeaveView() const;
    void requestDuplicateMode() const;
    void requestExtendMode() const;
    void requestOnlyMonitor(const QString &monName) const;
    void requestConfig(const QString &config) const;
    void requestCustom() const;

private:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void onItemClicked(const QModelIndex &index) const;
};

#endif // DISPLAYCONTROLPAGE_H
