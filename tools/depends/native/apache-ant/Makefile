include ../../Makefile.include
PREFIX=$(NATIVEPREFIX)
PLATFORM=$(NATIVEPLATFORM)

DEPS = ../../Makefile.include Makefile ../../download-files.include \
       01-fix-chmod.patch

# lib name, version
LIBNAME=apache-ant
VERSION=1.10.13
SOURCE=$(LIBNAME)-$(VERSION)-src
ARCHIVE=$(SOURCE).tar.gz
SHA512=5ee92097c27229507941bcb7a929dddbe50ad319ab2b33a617bdc0df0231541f65424dd1613dbe4d49d3dd7303bff8c36467be17dd0e23642c05a62e8d78da3f
BASE_URL=https://archive.apache.org/dist/ant/source
include ../../download-files.include

APP=$(PLATFORM)/dist/bin/ant

all: .installed-$(PLATFORM)

$(PLATFORM): $(DEPS) | $(TARBALLS_LOCATION)/$(ARCHIVE).$(HASH_TYPE)
	rm -rf $(PLATFORM)/*; mkdir -p $(PLATFORM)
	cd $(PLATFORM); $(ARCHIVE_TOOL) $(ARCHIVE_TOOL_FLAGS) $(TARBALLS_LOCATION)/$(ARCHIVE)
	cd $(PLATFORM); patch -p1 -i ../01-fix-chmod.patch
	cd $(PLATFORM); ./bootstrap.sh

$(APP): $(PLATFORM)
	cd $(PLATFORM); ./build.sh

.installed-$(PLATFORM): $(APP)
	cd $(PLATFORM); ANT_HOME=$(PREFIX) ./build.sh install-lite
	touch $@

clean:
	cd $(PLATFORM); ./build.sh clean
	rm -f .installed-$(PLATFORM)

distclean::
	rm -rf $(PLATFORM) .installed-$(PLATFORM)
