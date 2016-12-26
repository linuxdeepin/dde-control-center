#ifndef DEFAPPVIEWER_H
#define DEFAPPVIEWER_H

#include "contentwidget.h"
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
namespace dcc
{
namespace widgets {
class SwitchWidget;
class SettingsGroup;
class TranslucentFrame;
}
namespace defapp
{
class DefCategoryWidget;
class DefAppModel;
class DefAppViewer : public ContentWidget
{
    Q_OBJECT

public:
    explicit DefAppViewer(QWidget *parent = 0);
    QWidget *defappDetail();
    void setModel(DefAppModel * const model);

signals:
    void requestSetDefaultApp(const QString &category, const QJsonObject &item);
    void requestAddUserApp(const QString &category, const QString &item);
    void requestDelUserApp(const QString &name, const QJsonObject &item);
    void autoOpenChanged(const bool state);
    void requestFrameAutoHide(const bool autoHide) const;

private slots:
    void setAutoChanged(const bool visible);

private:

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

    widgets::SwitchWidget *m_switchWidget = nullptr;

};
}
}

#endif // DEFAPPVIEWER_H
