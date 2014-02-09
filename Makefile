CHTHON = chthon

INSTALL_PREFIX = /usr/local
INSTALL_PREFIX_LIB = $(INSTALL_PREFIX)/lib
INSTALL_PREFIX_INCLUDE = $(INSTALL_PREFIX)/include
INSTALL_PREFIX_DOCS = $(INSTALL_PREFIX)/share/doc

TEST_BIN = chthon_test
VERSION = $(shell ./version)
LIBNAME = lib$(CHTHON).so
LIBNAME_VERSION = $(LIBNAME).$(VERSION)
LIBS = -lncurses
HEADERS = $(wildcard src/*.h)
SOURCES = $(wildcard src/*.cpp)
TEST_SOURCES = $(wildcard test/*.cpp)
OBJ = $(addprefix tmp/,$(SOURCES:.cpp=.o))
TEST_OBJ = $(addprefix tmp/,$(TEST_SOURCES:.cpp=.o))
# -Wpadded
WARNINGS = -pedantic -Werror -Wall -Wextra -Wformat=2 -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wuninitialized -Wunused -Wfloat-equal -Wundef -Wno-endif-labels -Wshadow -Wcast-qual -Wcast-align -Wconversion -Wsign-conversion -Wlogical-op -Wmissing-declarations -Wno-multichar -Wredundant-decls -Wunreachable-code -Winline -Winvalid-pch -Wvla -Wdouble-promotion -Wzero-as-null-pointer-constant -Wuseless-cast -Wvarargs -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=format
CXXFLAGS = -MD -MP -std=c++0x $(WARNINGS)

all: lib docs

lib: $(LIBNAME)

docs: Doxyfile $(HEADERS) $(SOURCES)
	doxygen

install: all
	@echo Installing lib...
	cp $(LIBNAME) $(INSTALL_PREFIX_LIB)/$(LIBNAME_VERSION)
	chmod 0755 $(INSTALL_PREFIX_LIB)/$(LIBNAME_VERSION)
	rm -f $(INSTALL_PREFIX_LIB)/$(LIBNAME)
	ln -s $(INSTALL_PREFIX_LIB)/$(LIBNAME_VERSION) $(INSTALL_PREFIX_LIB)/$(LIBNAME)
	ldconfig
	@echo Installing includes...
	mkdir -p $(INSTALL_PREFIX_INCLUDE)/$(CHTHON).$(VERSION)
	cp src/*h $(INSTALL_PREFIX_INCLUDE)/$(CHTHON).$(VERSION)
	rm -f $(INSTALL_PREFIX_INCLUDE)/$(CHTHON)
	ln -s $(INSTALL_PREFIX_INCLUDE)/$(CHTHON).$(VERSION) $(INSTALL_PREFIX_INCLUDE)/$(CHTHON)
	@echo Installing docs...
	mkdir -p $(INSTALL_PREFIX_DOCS)/$(CHTHON).$(VERSION)/
	cp -R docs/html $(INSTALL_PREFIX_DOCS)/$(CHTHON).$(VERSION)
	rm -f $(INSTALL_PREFIX_DOCS)/$(CHTHON)
	ln -s $(INSTALL_PREFIX_DOCS)/$(CHTHON).$(VERSION) $(INSTALL_PREFIX_DOCS)/$(CHTHON)

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
	@$(CXX) $(CXXFLAGS) -c -fpic $< -o $@

.PHONY: clean Makefile check

clean:
	$(RM) -rf tmp/* $(TEST_BIN) $(LIBNAME)* docs/

$(shell mkdir -p tmp)
$(shell mkdir -p tmp/src)
$(shell mkdir -p tmp/test)
-include $(OBJ:%.o=%.d)
-include $(TEST_OBJ:%.o=%.d)

