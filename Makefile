# Makefile for SITypes static library (mirroring the OCTypes pattern)

##────────────────────────────────────────────────────────────────────────────
##  1) BASIC DEFINITIONS
##────────────────────────────────────────────────────────────────────────────

CC      := clang
AR      := ar
LEX     := flex
YACC    := bison
YFLAGS  := -d

RM      := rm -rf
MKDIR_P := mkdir -p

SRC_DIR        := src
TEST_SRC_DIR   := tests
BUILD_DIR      := build
OBJ_DIR        := $(BUILD_DIR)/obj
GEN_DIR        := $(BUILD_DIR)/gen
BIN_DIR        := $(BUILD_DIR)/bin

XCODE_BUILD    := build-xcode


# OCTypes “third_party” integration
OCTYPES_DIR    := third_party/OCTypes
OCT_INCLUDE    := $(OCTYPES_DIR)/include
OCT_LIBDIR     := $(OCTYPES_DIR)/lib

# THIS LINE ensures the proper include path for OCTypes:
CPPFLAGS := -DDEBUG -I. -I$(SRC_DIR) -I$(OCT_INCLUDE)
WARNINGS := -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter \
             -Wno-missing-field-initializers -Wno-unused-function
OPT      := -O3
CFLAGS   := $(CPPFLAGS) $(WARNINGS) $(OPT)

CFLAGS_DEBUG := -O0 -g -Wall -Wextra -Werror

# Detect platform for OCTypes binary archive name
UNAME_S := $(shell uname -s)
ARCH    := $(shell uname -m)
ifeq ($(UNAME_S),Darwin)
  OCT_LIB_BIN := libOCTypes-libOCTypes-macos-latest.zip
else ifeq ($(UNAME_S),Linux)
  ifeq ($(ARCH),aarch64)
    OCT_LIB_BIN := libOCTypes-libOCTypes-linux-arm64.zip
  else
    OCT_LIB_BIN := libOCTypes-libOCTypes-ubuntu-latest.zip
  endif
else ifneq ($(findstring MINGW,$(UNAME_S)),)
  OCT_LIB_BIN := libOCTypes-libOCTypes-windows-latest.zip
endif
OCT_LIB_ARCHIVE     := third_party/$(OCT_LIB_BIN)
OCT_HEADERS_ARCHIVE := third_party/libOCTypes-headers.zip

ifeq ($(UNAME_S),Linux)
  GROUP_START := -Wl,--start-group
  GROUP_END   := -Wl,--end-group
else
  GROUP_START :=
  GROUP_END   :=
endif

##────────────────────────────────────────────────────────────────────────────
##  Disable built‐in implicit rules
### MODIFIED: this prevents `make` from inferring a `%.c: %.l` rule
MAKEFLAGS += -rR
.SUFFIXES:
##────────────────────────────────────────────────────────────────────────────

.PHONY: all dirs octypes prepare test test-debug test-asan \
        test-werror install clean clean-objects clean-docs \
        sync-libs docs doxygen html xcode xcode-open xcode-run

.DEFAULT_GOAL := all

##────────────────────────────────────────────────────────────────────────────
##  2) TOP‐LEVEL “all” (make vs. make xcode)
##────────────────────────────────────────────────────────────────────────────

all: dirs octypes prepare libSITypes.a

##────────────────────────────────────────────────────────────────────────────
##  3) CREATE NECESSARY DIRECTORIES
##────────────────────────────────────────────────────────────────────────────

dirs:
	$(MKDIR_P) $(BUILD_DIR) $(OBJ_DIR) $(GEN_DIR) $(BIN_DIR)

##────────────────────────────────────────────────────────────────────────────
##  4) FETCH/UNPACK OCTypes (if needed)
##────────────────────────────────────────────────────────────────────────────

octypes: $(OCT_LIBDIR)/libOCTypes.a $(OCT_INCLUDE)/OCTypes/OCLibrary.h

third_party:
	$(MKDIR_P) third_party

$(OCT_LIB_ARCHIVE): | third_party
	@echo "Fetching OCTypes library: $(OCT_LIB_BIN)"
	curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.1/$(OCT_LIB_BIN) -o $@

$(OCT_HEADERS_ARCHIVE): | third_party
	@echo "Fetching OCTypes headers"
	curl -L https://github.com/pjgrandinetti/OCTypes/releases/download/v0.1.1/libOCTypes-headers.zip -o $@

# ----------------------------------------------------------------------------
# On Unix/macOS: use 'unzip'; on Windows: use PowerShell Expand-Archive
# ----------------------------------------------------------------------------

$(OCT_INCLUDE)/OCTypes/OCLibrary.h: $(OCT_HEADERS_ARCHIVE)
	@echo "Extracting OCTypes headers into $(OCT_INCLUDE)/OCTypes"
	$(RM) $(OCT_INCLUDE)
	$(MKDIR_P) $(OCT_INCLUDE)/OCTypes

ifneq ($(findstring MINGW,$(UNAME_S)),$(UNAME_S))
# ------------------ Unix / macOS path ------------------
	unzip -q $< -d $(OCT_INCLUDE)
	# Move any .h files at top level into the OCTypes/ subdir
	-mv $(OCT_INCLUDE)/*.h $(OCT_INCLUDE)/OCTypes/ 2>/dev/null || true
else
# ------------------ Windows path (Git Bash, MSYS, or MINGW) ------------------
	@echo "Detected Windows; using PowerShell to unzip headers"
	powershell -NoProfile -Command " \
	    if (Test-Path '$(subst /,\\,$<)') { \
	      Expand-Archive -Force '$(subst /,\\,$<)' -DestinationPath '$(subst /,\\,$(OCT_INCLUDE))' ; \
	    } else { \
	      Write-Error 'Header archive not found: $(OCT_HEADERS_ARCHIVE)' ; exit 1;\
	    }"
	# Move any top-level .h into the OCTypes subdirectory if present
	-if exist "$(subst /,\\,$(OCT_INCLUDE))\*.h" ( \
	    move /Y "$(subst /,\\,$(OCT_INCLUDE))\*.h" "$(subst /,\\,$(OCT_INCLUDE))\OCTypes\" \
	) else ( \
	    rem No top-level .h to move \
	)
endif
	@echo " → Extracted headers to $(OCT_INCLUDE)/OCTypes"
# ----------------------------------------------------------------------------

$(OCT_LIBDIR)/libOCTypes.a: $(OCT_LIB_ARCHIVE)
	@echo "Extracting OCTypes library"
	$(RM) $(OCT_LIBDIR)
	$(MKDIR_P) $(OCT_LIBDIR)
	unzip -q $< -d $(OCT_LIBDIR)

##────────────────────────────────────────────────────────────────────────────
##  5) RUN BISON / FLEX → $(GEN_DIR)
##     (exactly like OCTypes does for OCComplexParser / OCComplexScanner)
##────────────────────────────────────────────────────────────────────────────

LEX_SRC  := $(wildcard $(SRC_DIR)/*.l)
YACC_SRC := $(wildcard $(SRC_DIR)/*Parser.y)

# e.g. if there is SIDimensionalityParser.y  → GEN_DIR/SIDimensionalityParser.tab.c/h
GEN_PARSER_C := $(patsubst $(SRC_DIR)/%Parser.y,$(GEN_DIR)/%Parser.tab.c,$(YACC_SRC))
GEN_PARSER_H := $(patsubst $(SRC_DIR)/%Parser.y,$(GEN_DIR)/%Parser.tab.h,$(YACC_SRC))

# e.g. if there is SIDimensionalityScanner.l → GEN_DIR/SIDimensionalityScanner.c
GEN_SCANNER := $(patsubst $(SRC_DIR)/%.l,$(GEN_DIR)/%.c,$(LEX_SRC))

GEN_C := $(GEN_PARSER_C) $(GEN_SCANNER)
GEN_H := $(GEN_PARSER_H)

##────────────────────────────────────────────────────────────────────────────
##  5.1) Bison rule: each *.y → two files under $(GEN_DIR)/
### MODIFIED: Tell Bison _exactly_ where to write its .c and .h (no mv)
$(GEN_DIR)/%Parser.tab.c $(GEN_DIR)/%Parser.tab.h: $(SRC_DIR)/%Parser.y | dirs
	$(YACC) $(YFLAGS) -o $(GEN_DIR)/$*Parser.tab.c --defines=$(GEN_DIR)/$*Parser.tab.h $<
##────────────────────────────────────────────────────────────────────────────

##────────────────────────────────────────────────────────────────────────────
##  5.2) Flex rule: each *.l → single .c under $(GEN_DIR)/
### MODIFIED: drop the parser-header prerequisite
$(GEN_DIR)/%Scanner.c: $(SRC_DIR)/%Scanner.l | dirs
	$(LEX) -o $@ $<

# Catch‐all: if there were any other “.l” files without parser dependencies
$(GEN_DIR)/%.c: $(SRC_DIR)/%.l | dirs
	$(LEX) -o $@ $<

##────────────────────────────────────────────────────────────────────────────
##  6) COMPILE OBJECTS (non‐xcode path)
##────────────────────────────────────────────────────────────────────────────

STATIC_SRC := $(filter-out $(YACC_SRC) $(LEX_SRC),$(wildcard $(SRC_DIR)/*.c))

# Combine all the “.c” files we need to compile:
#
#   • every .c under src/       (STATIC_SRC)
#   • every generated .c under build/gen   (GEN_C)
OBJ_SRC := $(STATIC_SRC) $(GEN_C)

# Convert each source .c → corresponding .o under $(OBJ_DIR)/
OBJ := \
  $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter %.c,$(STATIC_SRC))) \
  $(patsubst $(GEN_DIR)/%.c,$(OBJ_DIR)/%.o,$(GEN_C))

DEP := $(OBJ:.o=.d)

# 6.1) Build rule for a “.c” in src/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -c -o $@ $<

# 6.2) Build rule for a generated “.c” in build/gen/
$(OBJ_DIR)/%.o: $(GEN_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -I$(GEN_DIR) -c -o $@ $<

##────────────────────────────────────────────────────────────────────────────
##  6.3) Ensure scanner object is compiled only after its parser header exists
### MODIFIED: this prevents any attempt to compile scanner .o before .tab.h exists
$(OBJ_DIR)/%Scanner.o: $(GEN_DIR)/%Parser.tab.h
##────────────────────────────────────────────────────────────────────────────

##────────────────────────────────────────────────────────────────────────────
##  7) LINK INTO STATIC LIB
##────────────────────────────────────────────────────────────────────────────

libSITypes.a: $(OBJ)
	$(AR) rcs $@ $^

##────────────────────────────────────────────────────────────────────────────
##  8) TESTS (exactly as before)
##────────────────────────────────────────────────────────────────────────────

TEST_C_FILES := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ     := $(patsubst $(TEST_SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(TEST_C_FILES))

$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | dirs
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $@ $<

$(BIN_DIR)/runTests: libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(TEST_OBJ) \
	    -L. -L$(OCT_LIBDIR) $(GROUP_START) -lOCTypes -lSITypes $(GROUP_END) -lm \
	    -o $@

test: octypes $(BIN_DIR)/runTests
	$(BIN_DIR)/runTests

test-debug: octypes libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS_DEBUG) -I$(SRC_DIR) $(TEST_OBJ) \
	    -L. -L$(OCT_LIBDIR) -lSITypes -lOCTypes -lm \
	    -o $(BIN_DIR)/runTests.debug

test-asan: octypes libSITypes.a $(TEST_OBJ)
	$(CC) $(CFLAGS) -fsanitize=address -O1 -g -I$(SRC_DIR) $(TEST_OBJ) \
	    -L. -L$(OCT_LIBDIR) -lSITypes -lOCTypes -lm \
	    -o $(BIN_DIR)/runTests.asan
	@$(BIN_DIR)/runTests.asan

test-werror: CFLAGS := $(CFLAGS_DEBUG)
test-werror: clean all test

# Copy from installed OCTypes
sync-libs:
	@echo "Copying OCTypes from installed location..."
	@$(RM) -r third_party/OCTypes
	@$(MKDIR_P) third_party/OCTypes/lib third_party/OCTypes/include/OCTypes
	@cp ../OCTypes/install/lib/libOCTypes.a third_party/OCTypes/lib/
	@cp ../OCTypes/install/include/OCTypes/*.h third_party/OCTypes/include/OCTypes/

##────────────────────────────────────────────────────────────────────────────
##  9) INSTALL (same as before)
##────────────────────────────────────────────────────────────────────────────

INSTALL_DIR     := install
INSTALL_LIB_DIR := $(INSTALL_DIR)/lib
INSTALL_INC_DIR := $(INSTALL_DIR)/include/SITypes

install: all
	$(MKDIR_P) $(INSTALL_LIB_DIR) $(INSTALL_INC_DIR)
	cp libSITypes.a $(INSTALL_LIB_DIR)/
	cp src/*.h $(INSTALL_INC_DIR)/

##────────────────────────────────────────────────────────────────────────────
## 10) CLEAN AND DEPENDENCIES
##────────────────────────────────────────────────────────────────────────────

clean-objects:
	$(RM) $(OBJ) $(TEST_OBJ)

clean: clean-objects
	$(RM) libSITypes.a
	$(RM) $(BUILD_DIR)
	$(RM) build-xcode
	$(RM) SITypes-OCTypes.xcworkspace
	$(RM) *.dSYM

clean-docs:
	$(RM) docs/doxygen docs/_build

# Include generated “.d” files (dependency tracking)
-include $(DEP)

##────────────────────────────────────────────────────────────────────────────
## 11) XCODE target (mirroring OCTTypes exactly)
##────────────────────────────────────────────────────────────────────────────

xcode: clean dirs
	@echo "Generating Xcode project in $(XCODE_BUILD)/ ..."
	@mkdir -p $(XCODE_BUILD)
	@cmake -G "Xcode" -S . -B $(XCODE_BUILD)
	@echo "✅ Xcode project created: $(XCODE_BUILD)/SITypes.xcodeproj"

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

##────────────────────────────────────────────────────────────────────────────
