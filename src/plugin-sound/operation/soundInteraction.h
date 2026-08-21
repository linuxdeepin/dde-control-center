// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDINTERACTION_H
#define SOUNDINTERACTION_H

#include "soundmodel.h"
#include "soundworker.h"

// using namespace  dccV25;

class soundInteraction : public QObject
{
    Q_OBJECT
public:
    explicit soundInteraction(QObject *parent);

    ~soundInteraction();

    //    Q_INVOKABLE SoundModel getUiModel();
    //    Q_INVOKABLE QObject getUiWork();
    // QObject *create(QObject * = nullptr) override { return this; }

    Q_INVOKABLE SoundModel *model() const;
    void setSoundModel(SoundModel *newSoundModel);

    Q_INVOKABLE SoundWorker *worker() const;
    void setSoundWork(SoundWorker *newSoundWork);

    Q_INVOKABLE void setSinkVolume(double value);

private:
    SoundModel *m_soundModel;
    SoundWorker *m_soundWork;
};

#endif // SOUNDINTERACTION_H
