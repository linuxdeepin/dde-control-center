#ifndef MIRACASTCONTROLPAGE_H
#define MIRACASTCONTROLPAGE_H

#include "miracastmodel.h"

#include <QWidget>

class MiracastModel;
class MiracastWorker;
class MiracastControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit MiracastControlPage(QWidget *parent = 0);
    ~MiracastControlPage();

private slots:
    void onItemClicked(const QModelIndex &index);

private:
    MiracastModel *m_miracastModel;
    MiracastWorker *m_miracastWorker;
};

#endif // MIRACASTCONTROLPAGE_H
