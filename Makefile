# $Id$

RASPPI:=2

QEMU=YES

NAME=testTools

GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'

ROOT_DIR:=$(shell pwd)
RULE_DIR:=$(ROOT_DIR)
#/../

# ARCH=arm

export ROOT_DIR
export RULE_DIR
export GCC_COLORS

EXTENSION=

#BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
BUILDTIME := $(shell TZ=CN date -u "+%Y_%m_%d")
#GCC_VERSION := $(shell $(CC) -dumpversion )


RELEASES_NAME=$(NAME)_$(GCC_VERSION)_$(ARCH)_$(EDITION)_$(BUILDTIME).tar.gz  

# this directory defined the configuration file for this program


export ARCH
export BUILDTIME
export RELEASES_NAME



# SUBDIRS += ptpd/src
SUBDIRS += linuxptp/src
SUBDIRS += linuxptp/programs
SUBDIRS += linuxptp/phc

# SUBDIRS += ethtool/src

# SUBDIRS += ntools
# SUBDIRS += ntools/timers

# SUBDIRS += omtools



all: 
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

clean: 
	rm -rf Linux.bin.* 
	rm -rf *.log
	- find . -name Linux.obj.* -prune -exec rm -r -f {} \;

#	rm -rf $(NAME)_*
	
# all	
install:
	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)
	@$(SHELL) $(ROOT_DIR)/install.sh $(ROOT_DIR)/Linux.bin.$(ARCH) $(ROOT_DIR)/releases  

#	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)

package:clean
	cd ..; tar -cvjf $(NAME).$(BUILDTIME).tar.bz2 $(NAME)

