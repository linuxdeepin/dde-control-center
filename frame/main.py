#!/usr/bin/env python

import os
import sys

from PyQt5.QtCore import QUrl
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQuick import QQuickView

app = QGuiApplication(sys.argv)

view = QQuickView()
view.engine().quit.connect(app.quit)

view.setSource(QUrl.fromLocalFile(os.path.join(
    os.path.dirname(__file__), 'Main.qml')))
view.show()

sys.exit(app.exec_())
