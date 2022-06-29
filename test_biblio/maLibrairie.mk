#
# helloworld
# basic building rules
#
HELLOWORLD_VERSION = 1.1
HELLOWORLD_SOURCE = maLibrairie.tar.gz
HELLOWORLD_SITE = http://github.com/hmz-rhl/$(HELLOWORLD_SOURCE)

HELLOWORLD_INSTALL_STAGING = YES

define HELLOWORLD_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define HELLOWORLD_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libmaLibrairie.a $(STAGING_DIR)/usr/lib/libmaLibrairie.a
    $(INSTALL) -D -m 0644 $(@D)/maLibrairie.h $(STAGING_DIR)/usr/include/maLibrairie.h
    $(INSTALL) -D -m 0755 $(@D)/libmaLibrairie.so* $(STAGING_DIR)/usr/lib
endef


$(eval $(call GENTARGETS,package,helloworld))