TEMPLATE = subdirs
SUBDIRS  = sound \
           power \
#           network \
           default_applications \
#           grub \
           mouse \
           system_info \
           display \
           account \
           shortcuts \
           personalization\
           keyboard \
           datetime \
           wacom

isEmpty(WITH_MODULE_GRUB){
    WITH_MODULE_GRUB = YES
}

isEqual(WITH_MODULE_GRUB, YES){
    SUBDIRS += grub

    CONFIG(debug, debug|release) {
        message(Debug)
        SUBDIRS += bluetooth
    }
}
