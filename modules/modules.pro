TEMPLATE = subdirs
SUBDIRS  = sound \
           power \
           bluetooth \
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
           datetime

isEmpty(WITH_MODULE_GRUB){
    WITH_MODULE_GRUB = YES
}

isEqual(WITH_MODULE_GRUB, YES){
    SUBDIRS += grub
}
