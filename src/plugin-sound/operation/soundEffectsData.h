// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDEFFECTSDATA_H
#define SOUNDEFFECTSDATA_H

#include <QString>
#include <DDesktopServices>

DGUI_USE_NAMESPACE

class SoundEffectsData
{
public:
    explicit SoundEffectsData();

    QString dispalyText() const;
    void setDispalyText(const QString &newDispalyText);

    QString name() const;
    void setName(const QString &newName);

    bool checked() const;
    void setChecked(bool newChecked);

    QString path() const;
    void setPath(const QString &newPath);

    DDesktopServices::SystemSoundEffect systemSoundEffect() const;
    void setSystemSoundEffect(DDesktopServices::SystemSoundEffect newSystemSoundEffect);

    QString aniIconPath() const;
    void setAniIconPath(const QString &newAniIconPath);

signals:


private:
    QString m_name;
    QString m_dispalyText;
    QString m_path;
    DDesktopServices::SystemSoundEffect m_systemSoundEffect;
    bool m_checked;
    QString m_aniIconPath;
};

#endif // SOUNDEFFECTSDATA_H
