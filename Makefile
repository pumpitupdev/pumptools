V               ?= @
BUILDDIR        ?= build

gitrev          := $(shell git rev-parse HEAD)
libcver         := $(shell ldd --version | head -n 1)

all: \
usage

.PHONY: \
build \
clean \
test \
build-docker \
git-version \
libc-version \
clang-format \
package

usage:
	$(V)echo "Pumptools project build makefile"
	$(V)echo "  build: Build the pumptools project"
	$(V)echo "  clean: Clean up all build output"
	$(V)echo "  test: Run pumptools's unit tests"
	$(V)echo "  build-docker: Build the pumptools project in a docker container"
	$(V)echo "  test-docker: Run pumptools's unit tests in a docker container"
	$(V)echo "  git-version: Generate a text file with the current git revision"
	$(V)echo "  libc-version: Generate a text file with the libc version available"
	$(V)echo "  clang-format: Apply code style defined in .clang-format style to all code in src/"
	$(V)echo "  package: Package the build output into distribution zip files"

# Standard build target to build pumptools
build: git-version libc-version
	$(V)mkdir -p $(BUILDDIR)
	$(V)cd $(BUILDDIR) && cmake .. && make -j $(grep -c ^processor /proc/cpuinfo)

clean:
	$(V)rm -rf $(BUILDDIR)

test: build
	$(V)scripts/run-tests.sh $(BUILDDIR)

# Build pumptools in a docker image and extract the build output
build-docker:
	$(V)rm -rf $(BUILDDIR)/docker
	$(V)mkdir -p $(BUILDDIR)/docker
	$(V)docker build -f Dockerfile.build -t pumptools:build .
	$(V)docker create --name pumptools-build pumptools:build
	$(V)docker cp pumptools-build:/pumptools/build $(BUILDDIR)/docker
	$(V)mv $(BUILDDIR)/docker/build/* $(BUILDDIR)/docker
	$(V)rm -r $(BUILDDIR)/docker/build
	$(V)docker rm -f pumptools-build
	$(V)echo "Build output of docker build can be found in build/docker subfolder."

test-docker:
	$(V)docker build -f Dockerfile.test -t pumptools:test .

# Generate a version file to identify the build
git-version:
	$(V)mkdir -p $(BUILDDIR)
	$(V)echo "$(gitrev)" > $(BUILDDIR)/git-version

# Generate a version file to identify the libc runtime used to build
libc-version:
	$(V)mkdir -p $(BUILDDIR)
	$(V)echo "$(libcver)" > $(BUILDDIR)/libc-version

clang-format:
	$(V)find src/ -iname *.h -o -iname *.c | xargs clang-format -i -style=file

include Package.mk