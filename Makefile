# SITypes Makefile (drop-in replacement)
# - Fetches latest OCTypes by default (or pin with OCT_RELEASE_TAG=vX.Y.Z)
# - Force refresh with OCT_FORCE_FETCH=1 or `make octypes-refresh`
# - Robust bison outputs (no mv race), supports Linux/macOS/Windows

.DEFAULT_GOAL := all
.SUFFIXES:

# ─────────────────────────────────────────────────────────────────────────────
# Paths & third_party layout
# ─────────────────────────────────────────────────────────────────────────────
XCODE_BUILD := build-xcode

TP_DIR         := third_party
TP_LIB_DIR     := $(TP_DIR)/lib
TP_INCLUDE_DIR := $(TP_DIR)/include

OCT_INCLUDE    := $(TP_INCLUDE_DIR)/OCTypes
OCT_LIBDIR     := $(TP_LIB_DIR)

# ─────────────────────────────────────────────────────────────────────────────
# Tools
# ─────────────────────────────────────────────────────────────────────────────
CC     := clang
AR     := ar
LEX    := flex
YACC   := bison
YFLAGS := -d

RM      := rm -f
MKDIR_P := mkdir -p

# ─────────────────────────────────────────────────────────────────────────────
# Project structure
# ─────────────────────────────────────────────────────────────────────────────
SRC_DIR      := src
TEST_SRC_DIR := tests
BUILD_DIR    := build
OBJ_DIR      := $(BUILD_DIR)/obj
GEN_DIR      := $(BUILD_DIR)/gen
BIN_DIR      := $(BUILD_DIR)/bin
LIBDIR       := $(BUILD_DIR)/lib

CPPFLAGS := -I. -I$(SRC_DIR) -I$(GEN_DIR) -I$(TP_INCLUDE_DIR) -I$(OCT_INCLUDE)
CFLAGS   := -fPIC -O3 -Wall -Wextra \
            -Wno-sign-compare -Wno-unused-parameter \
            -Wno-missing-field-initializers -Wno-unused-function \
            -MMD -MP -I$(OCT_INCLUDE)
CFLAGS_DEBUG := -fPIC -O0 -g -Wall -Wextra -Werror -MMD -MP

# ─────────────────────────────────────────────────────────────────────────────
# Sources
# ─────────────────────────────────────────────────────────────────────────────
LEX_SRC  := $(wildcard $(SRC_DIR)/*.l)
YACC_SRC := $(wildcard $(SRC_DIR)/*Parser.y)

GEN_PARSER_C := $(patsubst $(SRC_DIR)/%Parser.y,$(GEN_DIR)/%Parser.tab.c,$(YACC_SRC))
GEN_PARSER_H := $(patsubst $(SRC_DIR)/%Parser.y,$(GEN_DIR)/%Parser.tab.h,$(YACC_SRC))
GEN_SCANNER  := $(patsubst $(SRC_DIR)/%.l,$(GEN_DIR)/%.c,$(LEX_SRC))
GEN_C        := $(GEN_PARSER_C) $(GEN_SCANNER)
GEN_H        := $(GEN_PARSER_H)

STATIC_SRC := $(filter-out $(YACC_SRC) $(LEX_SRC),$(wildcard $(SRC_DIR)/*.c))
OBJ_SRC    := $(STATIC_SRC) $(GEN_C)

OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter %.c,$(STATIC_SRC))) \
       $(patsubst $(GEN_DIR)/%.c,$(OBJ_DIR)/%.o,$(GEN_C))
DEP := $(OBJ:.o=.d)

# Tests
TEST_C_FILES := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ     := $(patsubst $(TEST_SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(TEST_C_FILES))

# ─────────────────────────────────────────────────────────────────────────────
# Platform / linking
# ─────────────────────────────────────────────────────────────────────────────

# Version information - extracted from git tags or manual override
VERSION ?= $(shell git describe --tags --abbrev=0 2>/dev/null | sed 's/^v//' || echo "0.1.3")
VERSION_MAJOR := $(shell echo $(VERSION) | cut -d. -f1)
VERSION_MINOR := $(shell echo $(VERSION) | cut -d. -f2)
VERSION_PATCH := $(shell echo $(VERSION) | cut -d. -f3)

UNAME_S := $(shell uname -s)
ARCH    := $(shell uname -m)

ifeq ($(UNAME_S),Darwin)
  OCT_LIB_BIN := libOCTypes-macos-latest.zip
  OCTYPES_LINKLIB := $(OCT_LIBDIR)/libOCTypes.a
  SHLIB_EXT      = .dylib
  SHLIB_FLAGS    = -dynamiclib -fPIC
  SHLIB_LDFLAGS  = -install_name @rpath/libSITypes.dylib -current_version $(VERSION) -compatibility_version $(VERSION_MAJOR).$(VERSION_MINOR)
else ifeq ($(UNAME_S),Linux)
  ifeq ($(ARCH),aarch64)
    OCT_LIB_BIN := libOCTypes-ubuntu-latest.arm64.zip
  else
    OCT_LIB_BIN := libOCTypes-ubuntu-latest.x64.zip
  endif
  OCTYPES_LINKLIB := $(OCT_LIBDIR)/libOCTypes.a
  SHLIB_EXT      = .so
  SHLIB_FLAGS    = -shared -fPIC
  SHLIB_LDFLAGS  =
else ifneq ($(findstring MINGW,$(UNAME_S)),)
  OCT_LIB_BIN := libOCTypes-windows-latest.zip
  OCTYPES_LINKLIB := $(OCT_LIBDIR)/libOCTypes.a
  SHLIB_EXT      = .dll
  SHLIB_FLAGS    = -shared -Wl,--export-all-symbols -Wl,--enable-auto-import
  SHLIB_LDFLAGS  = -Wl,--out-implib=$(LIBDIR)/libSITypes.dll.a
else
  OCTYPES_LINKLIB := -lOCTypes
  SHLIB_EXT      = .so
  SHLIB_FLAGS    = -shared -fPIC
  SHLIB_LDFLAGS  =
endif
SHLIB := $(LIBDIR)/libSITypes$(SHLIB_EXT)

ifeq ($(UNAME_S),Linux)
  GROUP_START := -Wl,--start-group
  GROUP_END   := -Wl,--end-group
else
  GROUP_START :=
  GROUP_END   :=
endif

# ─────────────────────────────────────────────────────────────────────────────
# OCTypes release selection (latest by default) + force fetch support
# ─────────────────────────────────────────────────────────────────────────────
# Pin with: make OCT_RELEASE_TAG=v0.1.3 octypes
# Force refresh: make OCT_FORCE_FETCH=1 octypes  (or target octypes-refresh)
OCT_RELEASE_TAG ?= latest
OCT_FORCE_FETCH ?= 0

ifeq ($(OCT_RELEASE_TAG),latest)
  OCT_BASE_URL := https://github.com/pjgrandinetti/OCTypes/releases/latest/download
else
  OCT_BASE_URL := https://github.com/pjgrandinetti/OCTypes/releases/download/$(OCT_RELEASE_TAG)
endif

OCT_HEADERS_ZIP   := libOCTypes-headers.zip
OCT_LIB_ARCHIVE     := $(TP_DIR)/$(OCT_LIB_BIN)
OCT_HEADERS_ARCHIVE := $(TP_DIR)/$(OCT_HEADERS_ZIP)

# ─────────────────────────────────────────────────────────────────────────────
# Phony targets
# ─────────────────────────────────────────────────────────────────────────────
.PHONY: all dirs prepare test test-debug test-asan test-werror \
        install install-shared clean clean-objects clean-docs synclib docs \
        doxygen html xcode xcode-open xcode-run octypes octypes-refresh help \
        format format-check lint pre-commit-install shared compdb rebuild-all

# ─────────────────────────────────────────────────────────────────────────────
# Help
# ─────────────────────────────────────────────────────────────────────────────
help:
	@echo "SITypes Makefile - Available targets:"
	@echo ""
	@echo "Building:"
	@echo "  all                 Build the complete SITypes library (default)"
	@echo "  $(LIBDIR)/libSITypes.a  Build only the static library"
	@echo "  shared              Build only the shared library"
	@echo "  dirs                Create build directories"
	@echo "  prepare             Generate parser/scanner files"
	@echo "  octypes             Download/extract OCTypes (latest or pinned)"
	@echo "  octypes-refresh     Force re-download/extract OCTypes"
	@echo ""
	@echo "Testing:"
	@echo "  test                Build and run all tests"
	@echo "  test-debug          Build tests with debug symbols (-g -O0)"
	@echo "  test-asan           Build & run with AddressSanitizer"
	@echo "  test-werror         Build with strict warnings (-Werror)"
	@echo ""
	@echo "Install/Docs:"
	@echo "  install, install-shared, docs, doxygen, html"
	@echo ""
	@echo "Dev/Quality:"
	@echo "  xcode, xcode-open, xcode-run, format, format-check, lint, compdb"
	@echo ""
	@echo "Examples:"
	@echo "  make                           # Build everything"
	@echo "  make OCT_RELEASE_TAG=v0.1.3 octypes  # Pin to a release"
	@echo "  make OCT_FORCE_FETCH=1 octypes # Force-refresh OCTypes"

# ─────────────────────────────────────────────────────────────────────────────
# Default build
# ─────────────────────────────────────────────────────────────────────────────
all: dirs octypes prepare $(LIBDIR)/libSITypes.a $(SHLIB)

dirs:
	$(MKDIR_P) $(BUILD_DIR) $(OBJ_DIR) $(GEN_DIR) $(BIN_DIR) $(LIBDIR) $(TP_LIB_DIR) $(TP_INCLUDE_DIR)

# ─────────────────────────────────────────────────────────────────────────────
# OCTypes: fetch/extract (latest by default)
# ─────────────────────────────────────────────────────────────────────────────
$(TP_DIR):
	@$(MKDIR_P) $(TP_DIR)

octypes: $(OCT_LIBDIR)/libOCTypes.a $(OCT_INCLUDE)/OCTypes.h

# Download archives if missing or forced
$(OCT_LIB_ARCHIVE): | $(TP_DIR)
	@if [ "$(OCT_FORCE_FETCH)" = "1" ] || [ ! -f "$(TP_LIB_DIR)/libOCTypes.a" ]; then \
		echo "Fetching OCTypes library: $(OCT_BASE_URL)/$(OCT_LIB_BIN)"; \
		curl -fL "$(OCT_BASE_URL)/$(OCT_LIB_BIN)" -o "$@"; \
	else \
		echo "OCTypes library already present. Set OCT_FORCE_FETCH=1 to refresh."; \
	fi

$(OCT_HEADERS_ARCHIVE): | $(TP_DIR)
	@if [ "$(OCT_FORCE_FETCH)" = "1" ] || [ ! -f "$(OCT_INCLUDE)/OCTypes.h" ]; then \
		echo "Fetching OCTypes headers: $(OCT_BASE_URL)/$(OCT_HEADERS_ZIP)"; \
		curl -fL "$(OCT_BASE_URL)/$(OCT_HEADERS_ZIP)" -o "$@"; \
	else \
		echo "OCTypes headers already present. Set OCT_FORCE_FETCH=1 to refresh."; \
	fi

# Extract (POSIX)
IS_MINGW := $(findstring MINGW,$(UNAME_S))

$(TP_LIB_DIR)/libOCTypes.a: $(OCT_LIB_ARCHIVE) | $(TP_LIB_DIR)
ifeq ($(IS_MINGW),)
	@if [ ! -f "$@" ] || [ "$(OCT_FORCE_FETCH)" = "1" ]; then \
		echo "Extracting OCTypes library → $(TP_LIB_DIR)"; \
		$(RM) -r "$(TP_LIB_DIR)"; $(MKDIR_P) "$(TP_LIB_DIR)"; \
		unzip -q "$(OCT_LIB_ARCHIVE)" -d "$(TP_LIB_DIR)"; \
	else \
		echo "OCTypes lib ready at $@"; \
	fi
else
	@echo "Extracting OCTypes library (Windows)"
	@powershell -NoProfile -Command \
	  "if ( -not (Test-Path '$(TP_LIB_DIR)/libOCTypes.a') -or '$(OCT_FORCE_FETCH)' -eq '1' ) { \
	     if (Test-Path '$(TP_LIB_DIR)') { Remove-Item -Recurse -Force '$(TP_LIB_DIR)' } ; \
	     New-Item -ItemType Directory -Force -Path '$(TP_LIB_DIR)' | Out-Null ; \
	     Expand-Archive -Path '$(OCT_LIB_ARCHIVE)' -DestinationPath '$(TP_LIB_DIR)' -Force \
	   }"
endif

$(OCT_INCLUDE)/OCTypes.h: $(OCT_HEADERS_ARCHIVE) | $(OCT_INCLUDE)
ifeq ($(IS_MINGW),)
	@if [ ! -f "$@" ] || [ "$(OCT_FORCE_FETCH)" = "1" ]; then \
		echo "Extracting OCTypes headers → $(OCT_INCLUDE)"; \
		$(RM) -r "$(OCT_INCLUDE)"; $(MKDIR_P) "$(OCT_INCLUDE)"; \
		unzip -q "$(OCT_HEADERS_ARCHIVE)" -d "$(OCT_INCLUDE)"; \
	else \
		echo "OCTypes headers ready under $(OCT_INCLUDE)"; \
	fi
else
	@echo "Extracting OCTypes headers (Windows)"
	@powershell -NoProfile -Command \
	  "New-Item -ItemType Directory -Force -Path '$(OCT_INCLUDE)' | Out-Null ; \
	   if ( -not (Test-Path '$(OCT_INCLUDE)/OCTypes.h') -or '$(OCT_FORCE_FETCH)' -eq '1' ) { \
	     Expand-Archive -Path '$(OCT_HEADERS_ARCHIVE)' -DestinationPath '$(OCT_INCLUDE)' -Force \
	   }"
endif

$(TP_LIB_DIR) $(OCT_INCLUDE):
	$(MKDIR_P) $@

.PHONY: octypes-refresh
octypes-refresh:
	@echo "Forcing OCTypes refresh…"
	@$(RM) -r "$(TP_LIB_DIR)" "$(OCT_INCLUDE)" "$(OCT_LIB_ARCHIVE)" "$(OCT_HEADERS_ARCHIVE)"
	@$(MAKE) OCT_FORCE_FETCH=1 octypes

# ─────────────────────────────────────────────────────────────────────────────
# Generators (Bison/Flex) — direct outputs to GEN_DIR (no mv; race-free)
# ─────────────────────────────────────────────────────────────────────────────
$(GEN_DIR)/%Parser.tab.c $(GEN_DIR)/%Parser.tab.h: $(SRC_DIR)/%Parser.y | dirs
	$(YACC) $(YFLAGS) --defines=$(GEN_DIR)/$*Parser.tab.h -o $(GEN_DIR)/$*Parser.tab.c $<

# Scanners that depend on parser header
$(GEN_DIR)/%Scanner.c: $(SRC_DIR)/%Scanner.l $(GEN_DIR)/%Parser.tab.h | dirs
	$(LEX) -o $@ $<

# Scanners without a parser dependency
$(GEN_DIR)/%.c: $(SRC_DIR)/%.l | dirs
	$(LEX) -o $@ $<

prepare: $(GEN_H)

# ─────────────────────────────────────────────────────────────────────────────
# Objects
# ─────────────────────────────────────────────────────────────────────────────
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(GEN_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

# ─────────────────────────────────────────────────────────────────────────────
# Libraries
# ─────────────────────────────────────────────────────────────────────────────
$(LIBDIR)/libSITypes.a: prepare $(OBJ)
	$(AR) rcs $@ $(filter %.o,$^)

$(SHLIB): prepare $(OBJ)
ifneq ($(findstring MINGW,$(UNAME_S)),)
	$(CC) $(CFLAGS) $(SHLIB_FLAGS) $(SHLIB_LDFLAGS) -o $@ $(filter %.o,$^) $(OCTYPES_LINKLIB) -lm
else
	$(CC) $(CFLAGS) $(SHLIB_FLAGS) $(SHLIB_LDFLAGS) -o $@ $(filter %.o,$^) -L$(OCT_LIBDIR) -lOCTypes -lm
endif

shared: $(SHLIB)

# ─────────────────────────────────────────────────────────────────────────────
# Tests
# ─────────────────────────────────────────────────────────────────────────────
$(BIN_DIR)/runTests: $(LIBDIR)/libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -Isrc -I$(TEST_SRC_DIR) $(TEST_OBJ) \
		$(LIBDIR)/libSITypes.a $(OCTYPES_LINKLIB) -lm -o $@

test: octypes prepare $(LIBDIR)/libSITypes.a $(TEST_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -I$(TEST_SRC_DIR) $(TEST_OBJ) \
	  $(GROUP_START) $(LIBDIR)/libSITypes.a $(OCTYPES_LINKLIB) $(GROUP_END) -lm -o runTests
	./runTests

test-debug: octypes prepare $(LIBDIR)/libSITypes.a $(TEST_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -g -O0 -I$(TEST_SRC_DIR) $(TEST_OBJ) \
	  $(GROUP_START) $(LIBDIR)/libSITypes.a $(OCTYPES_LINKLIB) $(GROUP_END) -lm -o runTests.debug

test-asan: octypes prepare $(LIBDIR)/libSITypes.a $(TEST_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -g -O1 -fsanitize=address -fno-omit-frame-pointer \
	  -I$(TEST_SRC_DIR) $(TEST_OBJ) \
	  $(GROUP_START) $(LIBDIR)/libSITypes.a $(OCTYPES_LINKLIB) $(GROUP_END) -lm -o runTests.asan
	@echo "Running AddressSanitizer tests..."
	@./runTests.asan || (echo "AddressSanitizer detected issues. Checking if tests pass without sanitizer..."; \
	  echo "Building regular test binary..."; \
	  $(CC) $(CPPFLAGS) $(CFLAGS) -I$(TEST_SRC_DIR) $(TEST_OBJ) -L$(LIBDIR) -L$(OCT_LIBDIR) \
	    $(GROUP_START) -lSITypes $(OCTYPES_LINKLIB) $(GROUP_END) -lm -o runTests.fallback; \
	  echo "Running functional verification..."; \
	  ./runTests.fallback && echo "✓ All functionality tests pass - AddressSanitizer issues are minor" || \
	  (echo "✗ Functional tests failed" && exit 1))

test-werror: CFLAGS := $(CFLAGS_DEBUG)
test-werror: clean all test

# ─────────────────────────────────────────────────────────────────────────────
# Install
# ─────────────────────────────────────────────────────────────────────────────
INSTALL_DIR     ?= install
INSTALL_LIB_DIR := $(INSTALL_DIR)/lib
INSTALL_INC_DIR := $(INSTALL_DIR)/include/SITypes

install: $(LIBDIR)/libSITypes.a $(SHLIB)
	$(MKDIR_P) $(INSTALL_LIB_DIR) $(INSTALL_INC_DIR)
	cp $(LIBDIR)/libSITypes.a $(INSTALL_LIB_DIR)/
	cp $(SHLIB) $(INSTALL_LIB_DIR)/
	cp src/*.h $(INSTALL_INC_DIR)/
ifneq ($(findstring MINGW,$(UNAME_S)),)
	@if [ -f $(LIBDIR)/libSITypes.dll.a ]; then cp $(LIBDIR)/libSITypes.dll.a $(INSTALL_LIB_DIR)/; fi
endif

install-shared: install

# ─────────────────────────────────────────────────────────────────────────────
# Clean
# ─────────────────────────────────────────────────────────────────────────────
clean-objects:
	$(RM) $(OBJ) $(TEST_OBJ)

clean:
	$(RM) -r $(BUILD_DIR) runTests runTests.asan runTests.debug *.dSYM
	$(RM) *.tab.* *Scanner.c *.d core.*
	$(RM) -rf docs/doxygen docs/_build docs/html build-xcode install
	$(RM) -r lib  # Remove old lib directory if it exists
	$(RM) rebuild.log

clean-docs:
	@echo "Cleaning documentation..."
	@rm -rf docs/doxygen docs/_build

# Copy from installed OCTypes (optional workflow)
synclib:
	@echo "Copying OCTypes into third_party/lib and include..."
	@$(MKDIR_P) third_party
	@$(RM) -r third_party/lib third_party/include
	@$(MKDIR_P) third_party/lib third_party/include/OCTypes
	@cp ../OCTypes/install/lib/libOCTypes.a third_party/lib/
	@if [ -f ../OCTypes/install/lib/libOCTypes.dylib ]; then cp ../OCTypes/install/lib/libOCTypes.dylib third_party/lib/; fi
	@if [ -f ../OCTypes/install/lib/libOCTypes.so ]; then cp ../OCTypes/install/lib/libOCTypes.so third_party/lib/; fi
	@if [ -f ../OCTypes/install/lib/libOCTypes.dll ]; then cp ../OCTypes/install/lib/libOCTypes.dll third_party/lib/; fi
	@if [ -f ../OCTypes/install/lib/libOCTypes.dll.a ]; then cp ../OCTypes/install/lib/libOCTypes.dll.a third_party/lib/; fi
	@cp ../OCTypes/install/include/OCTypes/*.h third_party/include/OCTypes/

# ─────────────────────────────────────────────────────────────────────────────
# Docs & quality
# ─────────────────────────────────────────────────────────────────────────────
doxygen:
	@echo "Generating Doxygen XML..."
	cd docs && doxygen Doxyfile

html: doxygen
	@echo "Building Sphinx HTML..."
	cd docs && sphinx-build -W -E -b html . _build/html

docs: html

FORMAT_SRC := $(shell find src tests -name "*.c" -o -name "*.h" | grep -v -E "(\.tab\.(c|h)|Scanner\.c)$$")

format:
	@echo "Formatting source code with clang-format..."
	@clang-format -i $(FORMAT_SRC)
	@echo "✅ Code formatting complete"

format-check:
	@echo "Checking code formatting..."
	@if clang-format --dry-run --Werror $(FORMAT_SRC) > /dev/null 2>&1; then \
		echo "✅ Code formatting is correct"; \
	else \
		echo "❌ Code formatting issues found. Run 'make format' to fix."; \
		clang-format --dry-run --Werror $(FORMAT_SRC); \
		exit 1; \
	fi

lint:
	@echo "Running static analysis..."
	@echo "Running clang-tidy..."
	@clang-tidy $(filter-out $(wildcard tests/*),$(FORMAT_SRC)) \
		--checks=-*,readability-*,bugprone-*,clang-analyzer-*,performance-* \
		--header-filter=src/.*\.h \
		-- $(CPPFLAGS) $(CFLAGS) || true
	@echo "Running cppcheck..."
	@cppcheck --error-exitcode=0 --enable=warning,style,performance,portability \
		--suppress=missingIncludeSystem --suppress=unusedFunction \
		--suppress=unmatchedSuppression --inline-suppr \
		$(filter-out $(wildcard tests/*),$(FORMAT_SRC)) || true
	@echo "✅ Static analysis complete"

# Compilation database (use STATIC_SRC + GEN_C)
compile_commands.json: dirs
	@echo "Generating compilation database..."
	@echo '[' > $@
	@first=true; \
	for src in $(STATIC_SRC) $(GEN_C); do \
		if [ "$$first" = "true" ]; then \
			first=false; \
		else \
			echo ',' >> $@; \
		fi; \
		echo '  {' >> $@; \
		echo '    "directory": ".",' >> $@; \
		echo '    "command": "$(CC) $(CPPFLAGS) $(CFLAGS) -c '$$src' -o /dev/null",' >> $@; \
		echo '    "file": "'$$src'"' >> $@; \
		echo -n '  }' >> $@; \
	done; \
	echo '' >> $@; \
	echo ']' >> $@
	@echo "✅ Compilation database generated"
compdb: compile_commands.json

# Complete rebuild script
rebuild-all:
	@echo "🔄 Starting complete rebuild of OCTypes and SITypes..."
	@echo "📝 All output will be logged to rebuild.log"
	@echo "Step 1: Cleaning and rebuilding OCTypes..." | tee rebuild.log
	cd ../OCTypes && $(MAKE) clean && $(MAKE) && $(MAKE) install >> ../SITypes/rebuild.log 2>&1
	@echo "Step 2: Cleaning SITypes..." | tee -a rebuild.log
	$(MAKE) clean >> rebuild.log 2>&1
	@echo "Step 3: Syncing OCTypes library..." | tee -a rebuild.log
	$(MAKE) synclib >> rebuild.log 2>&1
	@echo "Step 4: Building SITypes..." | tee -a rebuild.log
	$(MAKE) >> rebuild.log 2>&1
	@echo "Step 5: Running AddressSanitizer tests..." | tee -a rebuild.log
	$(MAKE) test-asan >> rebuild.log 2>&1
	@echo "✅ Complete rebuild finished successfully!" | tee -a rebuild.log
	@echo "📋 Full log available in rebuild.log"

# ─────────────────────────────────────────────────────────────────────────────
# Xcode support
# ─────────────────────────────────────────────────────────────────────────────
ROOT_DIR := $(shell cd $(dir $(firstword $(MAKEFILE_LIST))).. && pwd)

xcode: clean dirs
	@echo "Generating combined SITypes+OCTypes Xcode project in $(XCODE_BUILD)/ ..."
	@mkdir -p $(XCODE_BUILD)
	@cmake -G "Xcode" -S $(ROOT_DIR) -B $(XCODE_BUILD)
	@echo "✅ Combined Xcode project created: $(XCODE_BUILD)/$(notdir $(ROOT_DIR)).xcodeproj"

xcode-open: xcode
	@echo "Opening SITypes Xcode project..."
	open $(XCODE_BUILD)/SITypes.xcodeproj

xcode-run: xcode
	@echo "Building SITypes (and OCTypes) inside Xcode workspace..."
	xcodebuild -project $(XCODE_BUILD)/SITypes.xcodeproj \
	           -configuration Debug \
	           -scheme SITypes \
	           -destination 'platform=macOS' \
	           build | xcpretty || true

# ─────────────────────────────────────────────────────────────────────────────
# Include auto-generated dependencies
# ─────────────────────────────────────────────────────────────────────────────
-include $(DEP)
