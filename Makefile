CHTHON = chthon2

ARCH = $(shell dpkg --print-architecture)
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
	@debpackage.py \
		$(CHTHON) \
		-v $(VERSION) \
		--maintainer 'umi041 <umi0451@gmail.com>' \
		--lib $(LIBNAME) \
		--header-root src \
		$$(find src -name '*.h' | sed 's|^|--header |') \
		--doc docs/html \
		--build-dir tmp/ \
		--dest-dir . \
		--description 'A compact C++ library (primarily for roguelike development). Contains utility classes and functions for game development such as FOV and pathfinding algorithms, 2D map class, some utilities for logging, formatting, strings, XML reading. Also XPM image utils and unit test framework.'

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

