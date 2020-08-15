/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SYSTEMINFOMODEL_H
#define SYSTEMINFOMODEL_H

#include <QObject>
#include <QPixmap>

namespace dcc{
namespace systeminfo{

// !!! 不要用C++11的前置声明枚举类型，这里lupdate命令有个bug．具体见
// https://stackoverflow.com/questions/6504902/lupdate-error-qualifying-with-unknown-namespace-class
#ifndef DISABLE_ACTIVATOR
//授权状态
enum ActiveState {
    Unauthorized = 0,  //未授权
    Authorized,  //已授权
    AuthorizedLapse,  //授权失效
    TrialAuthorized, //试用期已授权
    TrialExpired //试用期已过期
};
#endif

class SystemInfoModel : public QObject
{
    Q_OBJECT
public:
    explicit SystemInfoModel(QObject *parent = 0);

    void setEntryLists(const QStringList& list);
    QStringList entryLists() const { return m_entryLists;}
    QString defaultEntry() const { return m_defaultEntry;}

    QString distroid() const { return m_distroid;}
    QString distrover() const { return m_distrover;}
    QString productName() const { return m_productName;}
    QString versionNumber() const { return m_versionNumber;}
    QString version() const { return m_version;}
    QString type() const { return m_type;}
    QString processor() const { return m_processor;}
    QString memory() const { return m_memory;}
    QString disk() const { return m_disk;}
    QString kernel() const { return m_kernel;}

#ifndef DISABLE_ACTIVATOR
    inline ActiveState licenseState() const { return m_licenseState; }
#endif

    bool bootDelay() const;
    bool themeEnabled() const { return m_themeEnabled; }
    bool updating() const { return m_updating; }
    QPixmap background() const;
    void setBackground(const QPixmap &background);

Q_SIGNALS:
    void bootDelayChanged(const int timeout) const;
    void themeEnabledChanged(const bool enabled) const;
    void entryListsChanged(const QStringList &list);
    void defaultEntryChanged(const QString& entry);
    void updatingChanged(const bool &updating);
    void backgroundChanged(const QPixmap &pixmap);
    void distroidChanged(const QString& distroid);
    void distroverChanged(const QString& distrover);
    void productNameChanged(const QString& version);
    void versionNumberChanged(const QString& version);
    void versionChanged(const QString& version);
    void typeChanged(const QString& type);
    void processorChanged(const QString& processor);
    void memoryChanged(const QString& memory);
    void diskChanged(const QString& disk);
    void kernelChanged(const QString& kernel);

#ifndef DISABLE_ACTIVATOR
    void licenseStateChanged(ActiveState state);
#endif

public Q_SLOTS:
    void setBootDelay(bool bootDelay);
    void setThemeEnabled(const bool enabled);
    void setDefaultEntry(const QString& entry);
    void setUpdating(bool updating);
    void setDistroID(const QString& distroid);
    void setDistroVer(const QString& distrover);
    void setProductName(const QString& name);
    void setVersionNumber(const QString& number);
    void setVersion(const QString& version);
    void setType(qlonglong type);
    void setProcessor(const QString& processor);
    void setMemory(qulonglong totalMemory, qulonglong installedMemory);
    void setDisk(qulonglong disk);
    void setKernel(const QString &kernel);

#ifndef DISABLE_ACTIVATOR
    void setLicenseState(ActiveState state);
#endif

private:
    bool m_bootDelay;
    bool m_themeEnabled;
    bool m_updating;
    QStringList m_entryLists;
    QString m_defaultEntry;
    QPixmap m_background;

    QString m_distroid;
    QString m_distrover;
    QString m_version;
    QString m_productName;
    QString m_versionNumber;
    QString m_type;
    QString m_processor;
    QString m_memory;
    QString m_disk;
    QString m_kernel;
#ifndef DISABLE_ACTIVATOR
    ActiveState m_licenseState;
#endif
};

}
}

#ifndef DISABLE_ACTIVATOR
Q_DECLARE_METATYPE(dcc::systeminfo::ActiveState);
#endif

#endif // SYSTEMINFOMODEL_H
