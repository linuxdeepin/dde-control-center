#ifndef DEFAPPVIEWER_H
#define DEFAPPVIEWER_H

#include "contentwidget.h"
#include "defcategorywidget.h"
#include "switchwidget.h"
#include "defappmodel.h"
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QWidget>


class DefAppViewer : public ContentWidget
{
    Q_OBJECT

public:
    explicit DefAppViewer();
    QWidget *defappDetail();
    void setModel(DefAppModel * const model);

signals:
    void SetDefaultApp(const QString &category, const QJsonObject &item);
    void AddUserApp(const QString &category, const QJsonObject &item);
    void DelUserApp(const QJsonObject &item);
    void Reset();
    void autoOpenChanged(const bool state);
    void requestFrameAutoHide(const bool autoHide) const;

private slots:
    void setAutoChanged(const bool visible);

private:
    QVBoxLayout         *m_mainlayout;
    QWidget             *m_mainwidget;

    DefCategoryWidget *m_modBrowser = nullptr;
    DefCategoryWidget *m_modMail = nullptr;
    DefCategoryWidget *m_modText = nullptr;
    DefCategoryWidget *m_modMusic = nullptr;
    DefCategoryWidget *m_modVideo = nullptr;
    DefCategoryWidget *m_modPicture = nullptr;
    DefCategoryWidget *m_modTerminal = nullptr;

    DefCategoryWidget *m_modCDAudio = nullptr;
    DefCategoryWidget *m_modDVDVideo = nullptr;
    DefCategoryWidget *m_modMusicPlayer = nullptr;
    DefCategoryWidget *m_modCamera = nullptr;
    DefCategoryWidget *m_modSoftware = nullptr;

    dcc::SwitchWidget *m_switchWidget = nullptr;

};

#endif // DEFAPPVIEWER_H
