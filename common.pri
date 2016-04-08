isEmpty(PREFIX){
    PREFIX = /usr
}

isEqual(ARCH_MIPSEL, YES){
    DEFINES += ARCH_MIPSEL
}

include(interfaces/interfaces.pri)
