SUBDIRS = help libgd lib embed src data po doc

if ENABLE_TESTS
SUBDIRS += tests
endif

AM_DISTCHECK_CONFIGURE_FLAGS = \
	--enable-appdata-check	\
	--enable-debug

@CODE_COVERAGE_RULES@

# Ignore gtk theme cache files on distcheck
distuninstallcheck_listfiles = find . -type f -print | grep -v 'icon-theme.cache'

# Build ChangeLog from git history
ChangeLog:
	@if test -f $(top_srcdir)/.git/HEAD; then \
		pushd $(top_srcdir); \
		git log --max-count=500 --no-decorate --stat > $(abs_top_builddir)/$@; \
		popd; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

MAINTAINERCLEANFILES = \
        $(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
        $(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
        $(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL)

-include $(top_srcdir)/git.mk
