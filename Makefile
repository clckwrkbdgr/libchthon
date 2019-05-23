CHTHON = chthon2

ARCH = $(shell dpkg --print-architecture)
DEB_DIR = tmp/lib$(CHTHON)_$(VERSION)_$(ARCH)
DEB_NAME = lib$(CHTHON)_$(VERSION)_$(ARCH).deb

ifneq (,$(findstring mingw,$(CXX)))
FPIC = 
LIB_EXT = dll
else
FPIC = -fpic
LIB_EXT = so
endif

TEST_BIN = chthon_test
VERSION = $(shell ./version)
LIBNAME = lib$(CHTHON).$(LIB_EXT)
LIBNAME_VERSION = $(LIBNAME).$(VERSION)
HEADERS = $(wildcard src/*.h)
SOURCES = $(wildcard src/*.cpp)
TEST_SOURCES = $(wildcard test/*.cpp)
OBJ = $(addprefix tmp/,$(SOURCES:.cpp=.o))
TEST_OBJ = $(addprefix tmp/,$(TEST_SOURCES:.cpp=.o))
# -Wpadded -Wuseless-cast -Wvarargs 
WARNINGS = -pedantic -Werror -Wall -Wextra -Wformat=2 -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wuninitialized -Wunused -Wfloat-equal -Wundef -Wno-endif-labels -Wshadow -Wcast-qual -Wcast-align -Wconversion -Wsign-conversion -Wlogical-op -Wmissing-declarations -Wno-multichar -Wredundant-decls -Wunreachable-code -Winline -Winvalid-pch -Wvla -Wdouble-promotion -Wzero-as-null-pointer-constant -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn
CXXFLAGS = -MD -MP -std=c++0x $(WARNINGS) -Wno-sign-compare

all: lib

lib: $(LIBNAME)

docs: Doxyfile $(HEADERS) $(SOURCES)
	doxygen

deb: $(DEB_NAME)

$(DEB_NAME): lib docs
	rm -rf $(DEB_DIR)
	mkdir -p $(DEB_DIR)
	install -D $(LIBNAME) $(DEB_DIR)/usr/local/lib/$(LIBNAME_VERSION)
	mkdir -p $(DEB_DIR)/usr/local/include/$(CHTHON).$(VERSION)
	install -Dm0644 src/*h $(DEB_DIR)/usr/local/include/$(CHTHON).$(VERSION)
	mkdir -p $(DEB_DIR)/usr/local/share/doc/
	cp -R docs/html $(DEB_DIR)/usr/local/share/doc/$(CHTHON).$(VERSION)
	ln -fs $(LIBNAME_VERSION) $(DEB_DIR)/usr/local/lib/$(LIBNAME)
	ln -fs $(CHTHON).$(VERSION) $(DEB_DIR)/usr/local/include/$(CHTHON)
	ln -fs $(CHTHON).$(VERSION) $(DEB_DIR)/usr/local/share/doc/$(CHTHON)
	mkdir -p $(DEB_DIR)/DEBIAN
	echo 'Package: lib$(CHTHON)' > $(DEB_DIR)/DEBIAN/control
	echo 'Version: $(VERSION)' >> $(DEB_DIR)/DEBIAN/control
	echo 'Architecture: $(ARCH)' >> $(DEB_DIR)/DEBIAN/control
	echo 'Maintainer: umi041 <umi0451@gmail.com>' >> $(DEB_DIR)/DEBIAN/control
	echo 'Description: A compact C++ library (primarily for roguelike development).' >> $(DEB_DIR)/DEBIAN/control
	echo ' Contains utility classes and functions for game development such as' >> $(DEB_DIR)/DEBIAN/control
	echo ' FOV and pathfinding algorithms, 2D map class, some utilities for logging,' >> $(DEB_DIR)/DEBIAN/control
	echo ' formatting, strings, XML reading. Also XPM image utils and unit test framework.' >> $(DEB_DIR)/DEBIAN/control
	fakeroot dpkg-deb --build $(DEB_DIR)
	mv tmp/$(DEB_NAME) .

install:
	@[ -f $(DEB_NAME) ] || (echo 'First you need to create .deb package: `make deb`'; false)
	dpkg -i $(DEB_NAME)

check: test
	cppcheck --enable=all --template='{file}:{line}: {severity}: {message} ({id})' .

test: $(TEST_BIN)
	./$(TEST_BIN) $(TESTS)

$(LIBNAME): $(OBJ)
	$(CXX) -shared $(LIBS) -o $@ $^

$(TEST_BIN): $(OBJ) $(TEST_OBJ)
	$(CXX) $(LIBS) -o $@ $^

tmp/%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -c $(FPIC) $< -o $@

.PHONY: clean Makefile check test deb

clean:
	$(RM) -rf tmp/* $(TEST_BIN) $(LIBNAME)* docs/

$(shell mkdir -p tmp)
$(shell mkdir -p tmp/src)
$(shell mkdir -p tmp/test)
-include $(OBJ:%.o=%.d)
-include $(TEST_OBJ:%.o=%.d)

