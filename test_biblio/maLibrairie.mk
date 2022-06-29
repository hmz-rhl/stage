#
# helloworld
# basic building rules
#
MALIBRAIRIE_VERSION = 1.1
MALIBRAIRIE_SOURCE = maLibrairie.tar.gz
MALIBRAIRIE_SITE = https://github.com/hmz-rhl/$(MALIBRAIRIE_SOURCE)

MALIBRAIRIE_INSTALL_STAGING = YES

define MALIBRAIRIE_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define MALIBRAIRIE_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libmaLibrairie.a $(STAGING_DIR)/usr/lib/libmaLibrairie.a
    $(INSTALL) -D -m 0644 $(@D)/maLibrairie.h $(STAGING_DIR)/usr/include/maLibrairie.h
    $(INSTALL) -D -m 0755 $(@D)/libmaLibrairie.so* $(STAGING_DIR)/usr/lib
endef


$(eval $(call GENTARGETS,package,maLibrairie))