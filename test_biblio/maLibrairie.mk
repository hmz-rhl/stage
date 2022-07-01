#
# maLibrairie
# basic building rules
#
MALIBRAIRIE_VERSION = 1.1
MALIBRAIRIE_SOURCE = main.tar.gz
MALIBRAIRIE_SITE = https://github.com/hmz-rhl/maLibrairie/archive/refs/heads


MALIBRAIRIE_INSTALL_STAGING = YES

define MALIBRAIRIE_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define MALIBRAIRIE_INSTALL_STAGING_CMDS
	$(make) CC="$(TARGET_CC)" DIR="$(STAGING_DIR)" -C $(@D) install
endef


$(eval $(generic-package))