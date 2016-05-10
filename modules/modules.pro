TEMPLATE = subdirs
SUBDIRS  = \
    account \
    display \
    sound \
    power \
    default_applications \
    mouse \
    system_info \
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
