include ../../Makefile.include
DEPS = ../../Makefile.include Makefile ../../download-files.include

# lib name, version
LIBNAME=dbus
VERSION=1.13.14
SOURCE=$(LIBNAME)-$(VERSION)
ARCHIVE=$(SOURCE).tar.xz
SHA512=64290c16a32111cfe342b62e30cacfc700b2f23623e3eca9687def3a4ace6bf9054f5ab97b41b072040b29e6736cc32233e007998a912b56b3f52974af223308
include ../../download-files.include

# configuration settings
CONFIGURE=./configure --prefix=$(PREFIX) \
  --without-x --disable-xml-docs --disable-doxygen-docs

LIBDYLIB=$(PLATFORM)/$(LIBNAME)/.libs/lib$(LIBNAME)-1.so

all: $(LIBDYLIB) .installed-$(PLATFORM)


$(PLATFORM): $(DEPS) | $(TARBALLS_LOCATION)/$(ARCHIVE).$(HASH_TYPE)
	rm -rf $(PLATFORM)/*; mkdir -p $(PLATFORM)
	cd $(PLATFORM); $(ARCHIVE_TOOL) $(ARCHIVE_TOOL_FLAGS) $(TARBALLS_LOCATION)/$(ARCHIVE)
	cd $(PLATFORM); $(CONFIGURE)

$(LIBDYLIB): $(PLATFORM)
	$(MAKE) -C $(PLATFORM)

.installed-$(PLATFORM): $(LIBDYLIB)
	$(MAKE) -C $(PLATFORM) install
	touch $@

clean:
	$(MAKE) -C $(PLATFORM) clean
	rm -f .installed-$(PLATFORM)

distclean::
	rm -rf $(PLATFORM) .installed-$(PLATFORM)
