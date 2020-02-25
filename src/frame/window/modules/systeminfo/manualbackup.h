#pragma once

#include "namespace.h"
#include <QWidget>
#include <dfilechooseredit.h>

DWIDGET_USE_NAMESPACE

class QLabel;
class QPushButton;

namespace DCC_NAMESPACE {
    namespace systeminfo {
        class ManualBackup : public QWidget {
            Q_OBJECT
        public:
            explicit ManualBackup(QWidget* parent = nullptr);

        private Q_SLOTS:
            void onChoose();
            void backup();

        private:
            DFileChooserEdit* m_directoryChooseWidget;
            QLabel          * m_tipsLabel;
            QPushButton* m_backupBtn;
        };
    }
}
