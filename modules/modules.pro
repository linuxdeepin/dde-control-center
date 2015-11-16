TEMPLATE = subdirs
SUBDIRS  = sound \
           power \
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
           remote_assistance \
           wacom

isEmpty(WITH_MODULE_GRUB){
    WITH_MODULE_GRUB = YES
}

isEqual(WITH_MODULE_GRUB, YES){
    SUBDIRS += grub
}

isEmpty(WITH_MODULE_BLUETOOTH){
    WITH_MODULE_BLUETOOTH = YES
}

isEqual(WITH_MODULE_BLUETOOTH, YES){
    SUBDIRS += bluetooth
}

isEmpty(WITH_MODULE_NETWORK){
    WITH_MODULE_NETWORK = YES
}

isEqual(WITH_MODULE_NETWORK, YES){
    SUBDIRS += network
}
