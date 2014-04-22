import QtQuick 2.1
import Deepin.Widgets 1.0

BaseEditPage {
    id: root
    activeExpandIndex: 0

    EditTitle {}

    EditSectionSeparator {relatedSection: sectionGeneral}
    EditSectionGeneral {
        myIndex: 0
        id: sectionGeneral
        activeExpandIndex: root.activeExpandIndex
    }

    EditSectionSeparator {relatedSection: sectionIpv4}
    EditSectionIpv4 {
        myIndex: 1
        id: sectionIpv4
        activeExpandIndex: root.activeExpandIndex
    }

    EditSectionSeparator {relatedSection: sectionSecurity}
    EditSectionSecurity {
        myIndex: 2
        id: sectionSecurity
        activeExpandIndex: root.activeExpandIndex
    }
}
