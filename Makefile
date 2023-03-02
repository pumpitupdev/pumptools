V               ?= @
BUILDDIR        ?= build

DOCKER_CONTAINER := pumptools:build
DOCKER_CONTAINER_NAME := pumptools-build

gitrev          := $(shell git rev-parse HEAD)
libcver         := $(shell ldd --version | head -n 1)

.DEFAULT_GOAL := help

# -----------------------------------------------------------------------------
# Targets
# -----------------------------------------------------------------------------

.PHONY: build # Build the pumptools project and create a distribution package. Build output in build/
build: git-version libc-version
	$(V)mkdir -p $(BUILDDIR)
	$(V)cd $(BUILDDIR) && cmake .. && make -j $(grep -c ^processor /proc/cpuinfo)

.PHONY: clean # Cleanup all build output
clean:
	$(V)rm -rf $(BUILDDIR)

.PHONY: test # Run pumptools's unit tests
test: build
	$(V)scripts/run-tests.sh $(BUILDDIR)

.PHONY: build-docker # Build pumptools in a docker container and extract the build output to build/docker
build-docker:
	$(V)rm -rf $(BUILDDIR)/docker
	$(V)mkdir -p $(BUILDDIR)/docker
	$(V)docker build -t $(DOCKER_CONTAINER) .
	$(V)docker create --name $(DOCKER_CONTAINER_NAME) $(DOCKER_CONTAINER)
	$(V)docker cp $(DOCKER_CONTAINER_NAME):/pumptools/build $(BUILDDIR)/docker
	$(V)mv $(BUILDDIR)/docker/build/* $(BUILDDIR)/docker
	$(V)rm -r $(BUILDDIR)/docker/build
	$(V)docker rm -f $(DOCKER_CONTAINER_NAME)
	$(V)echo "Build output of docker build can be found in build/docker subfolder."

.PHONY: git-version # Generate a text file with the current git revision
git-version:
	$(V)mkdir -p $(BUILDDIR)
	$(V)echo "$(gitrev)" > $(BUILDDIR)/git-version

.PHONY: libc-version # Generate a text file with the libc version available
libc-version:
	$(V)mkdir -p $(BUILDDIR)
	$(V)echo "$(libcver)" > $(BUILDDIR)/libc-version

# Do not format external imports
.PHONY: clang-format # Apply code style defined in .clang-format style to all code in src/
clang-format:
	$(V)find src/api -iname *.h -o -iname *.c | xargs clang-format -i -style=file
	$(V)find src/main -iname *.h -o -iname *.c | xargs clang-format -i -style=file
	$(V)find src/test -iname *.h -o -iname *.c | xargs clang-format -i -style=file

.PHONY: package # Package the build output into distribution zip files
package: $(BUILDDIR)/pumptools.zip $(BUILDDIR)/pumptools-public.zip

include Package.mk

# -----------------------------------------------------------------------------
# Utility, combo and alias targets
# -----------------------------------------------------------------------------

# Help screen note:
# Variables that need to be displayed in the help screen need to strictly
# follow the pattern "^[A-Z_]+ \?= .* # .*".
# Targets that need to be displayed in the help screen need to add a separate
# phony definition strictly following the pattern "^\.PHONY\: .* # .*".

.PHONY: help # Print help screen
help:
	$(V)echo Pumptools project makefile.
	$(V)echo
	$(V)echo "Environment variables:"
	$(V)grep -E '^[A-Z_]+ \?\= .* #' Makefile | gawk 'match($$0, /([A-Z_]+) \?= [$$\(]*([^\)]*)[\)]{0,1} # (.*)/, a) { printf("  \033[0;35m%-25s \033[0;0m%-45s [%s]\n", a[1], a[3], a[2]) }'
	$(V)echo ""
	$(V)echo "Targets:"
	$(V)grep '^.PHONY: .* #' Makefile | gawk 'match($$0, /\.PHONY: (.*) # (.*)/, a) { printf("  \033[0;32m%-25s \033[0;0m%s\n", a[1], a[2]) }'
