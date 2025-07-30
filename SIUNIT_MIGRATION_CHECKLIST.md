# SIUnit to SIUnit Migration Checklist

## Overview
This checklist guides the migration from `SIUnit.c/SIUnit.h` to `SIUnit.c/SIUnit.h` in the `SIUnits-refactor` branch.

**Current Status:** SIUnit implementation complete but missing 12 critical API functions
**Risk Level:** 🔴 High - Major API gaps will cause compilation failures

---

## ⚠️ Critical Issues Identified

### API Gap Analysis
- **Original SIUnit.h:** 49 functions
- **Current SIUnit.h:** 37 functions  
- **Missing:** 12 essential functions

---

## 📋 Pre-Migration Tasks

### ✅ Phase 1: Complete SIUnit API

#### 🚨 Priority 1: Missing Core Functions
Add these functions to `SIUnit.h` and implement in `SIUnit.c`:

- [ ] `SIUnitFromExpression(OCStringRef expression, double *unit_multiplier, OCStringRef *error)`
  - **Critical:** Used heavily in SIScalar.c and SIScalarConstants.c
  - **Current calls:** ~10+ locations across codebase
  
- [ ] `cJSON *SIUnitCreateJSON(SIUnitRef unit)`
  - **Impact:** JSON serialization will break
  
- [ ] `SIPrefix SIUnitGetNumeratorPrefixAtIndex(SIUnitRef theUnit, uint8_t index)`
- [ ] `SIPrefix SIUnitGetDenominatorPrefixAtIndex(SIUnitRef theUnit, uint8_t index)`
  - **Impact:** Prefix introspection functionality

#### 🔶 Priority 2: Root Property Access
- [ ] `OCStringRef SIUnitCopyRootName(SIUnitRef theUnit)`
- [ ] `OCStringRef SIUnitCopyRootPluralName(SIUnitRef theUnit)`  
- [ ] `OCStringRef SIUnitCopyRootSymbol(SIUnitRef theUnit)`
  - **Impact:** Unit metadata access

#### 🔶 Priority 3: Prefix Metadata
- [ ] `bool SIUnitAllowsSIPrefix(SIUnitRef theUnit)`
- [ ] `SIPrefix SIUnitGetRootSymbolPrefix(SIUnitRef theUnit)`
- [ ] `bool SIUnitGetIsSpecialSISymbol(SIUnitRef theUnit)`

#### 🔶 Priority 4: Type Checking Functions
- [ ] `bool SIUnitIsCoherentSIBaseUnit(SIUnitRef theUnit)`
- [ ] `bool SIUnitIsSIBaseUnit(SIUnitRef theUnit)`
- [ ] `bool SIUnitIsDimensionlessAndUnderived(SIUnitRef theUnit)`

#### 🔶 Priority 5: Unit Arithmetic (if missing)
- [ ] `SIUnitRef SIUnitByMultiplying(...)`
- [ ] `SIUnitRef SIUnitByDividing(...)`
- [ ] `SIUnitRef SIUnitByRaisingToPower(...)`
- [ ] `SIUnitRef SIUnitByReducing(...)`

### ✅ Phase 2: Fix Internal Dependencies

#### 🚨 Critical: SIUnit.c Internal Issues
Fix these mixed function calls in `SIUnit.c`:

- [ ] **Line 309:** `SIUnitReduceExpression(raw_symbol)` 
  - Replace with `SIUnitReduceExpression()` or equivalent
  
- [ ] **Line 311:** `SIUnitCreateLibraryKey(raw_symbol)`
  - Replace with `SIUnitCreateLibraryKey()` or equivalent
  
- [ ] **Lines 581, 585:** `SIUnitLibrarySetImperialVolumes()`
  - Should be `SIUnitLibrarySetImperialVolumes()` (already exists)

### ✅ Phase 3: Constants & Macros
Verify these are present in SIUnit.h:

- [ ] `#define kSIUnitMeter STR("meter")`
- [ ] `#define kSIUnitMeters STR("meters")`
- [ ] `#define kSIUnitGram STR("gram")`
- [ ] ... (all other kSIUnit* constants)
- [ ] Physical constants like `#define kSIMuonGFactor -2.0023318418`

---

## 🧪 Testing Phase

### ✅ Phase 4: Dependency Testing

#### Test Critical Dependencies:
- [ ] **SIScalar.c compilation**
  ```bash
  cd SITypes && make SIScalar.o
  ```
  
- [ ] **SIScalarConstants.c compilation**
  ```bash
  cd SITypes && make SIScalarConstants.o
  ```
  
- [ ] **SIQuantity.c compilation**
  ```bash
  cd SITypes && make SIQuantity.o  
  ```

#### Verify Key Function Calls:
- [ ] `SIUnitFromExpression()` calls work (Lines: SIScalarConstants.c:70, etc.)
- [ ] `SIUnitWithSymbol()` calls work (Lines: SIScalarConstants.c:238, 273, etc.)
- [ ] `SIUnitGetDimensionality()` calls work (Lines: SIScalar.c:544, 566, etc.)

### ✅ Phase 5: Build System Test
- [ ] **Full SITypes compilation:**
  ```bash
  cd SITypes && make clean && make
  ```

- [ ] **Test suite execution:**
  ```bash
  cd SITypes && make test
  ```

---

## 🔄 Migration Execution

### ✅ Phase 6: Safe Migration Process

#### Pre-Migration Backup:
- [ ] Create backup tag:
  ```bash
  git tag backup-before-SIUnit-rename
  ```

#### File Operations:
- [ ] **Delete original files:**
  ```bash
  rm SITypes/src/SIUnit.c
  rm SITypes/src/SIUnit.h
  ```

- [ ] **Rename SIUnit files:**
  ```bash
  mv SITypes/src/SIUnit.c SITypes/src/SIUnit.c
  mv SITypes/src/SIUnit.h SITypes/src/SIUnit.h
  ```

#### Global Search & Replace:
- [ ] **In SIUnit.c:**
  ```bash
  sed -i '' 's/SIUnit/SIUnit/g' SITypes/src/SIUnit.c
  ```
  
- [ ] **In SIUnit.h:**
  ```bash
  sed -i '' 's/SIUnit/SIUnit/g' SITypes/src/SIUnit.h
  ```

#### Update Header Guard:
- [ ] **In SIUnit.h, change:**
  ```c
  #ifndef SIUnit_H
  #define SIUnit_H
  ```
  **To:**
  ```c
  #ifndef SIUnit_h
  #define SIUnit_h
  ```

---

## ✅ Post-Migration Validation

### ✅ Phase 7: Immediate Testing
- [ ] **Compilation test:**
  ```bash
  cd SITypes && make clean && make 2>&1 | head -20
  ```

- [ ] **Basic functionality test:**
  ```bash
  cd SITypes && make test_unit_0 && ./test_unit_0
  ```

### ✅ Phase 8: Comprehensive Testing
- [ ] **All unit tests pass:**
  ```bash
  cd SITypes && make test && ./test
  ```

- [ ] **Python wrapper compatibility:**
  ```bash
  cd RMNpy && python -m pytest tests/test_sitypes/test_unit.py -v
  ```

### ✅ Phase 9: Edge Case Testing
- [ ] **SIScalar operations work:**
  ```bash
  # Test scalar with units
  cd SITypes && make test_SIScalarCreateWithDouble && ./test_SIScalarCreateWithDouble
  ```

- [ ] **Unit arithmetic operations:**
  ```bash
  # Test unit multiplication, division, etc.
  cd SITypes && make test_unit_by_multiplying && ./test_unit_by_multiplying
  ```

---

## 🚑 Rollback Plan

### If Issues Arise:
- [ ] **Quick rollback:**
  ```bash
  git reset --hard backup-before-SIUnit-rename
  ```

- [ ] **Investigate specific failures:**
  ```bash
  git log --oneline -10  # See what changed
  git diff HEAD~1        # See exact changes
  ```

---

## 📊 Success Criteria

### ✅ Migration Complete When:
- [ ] All 49 SIUnit functions have SIUnit equivalents
- [ ] SITypes compiles without errors
- [ ] All unit tests pass
- [ ] SIScalar/SIQuantity functionality intact
- [ ] Python wrappers still work
- [ ] No performance regressions

---

## 📝 Notes

### Current Issues Discovered:
- SIUnit.c has mixed function calls to old SIUnit functions
- 12 critical functions missing from API
- Constants/macros may need verification

### Dependencies Identified:
- **SIScalar.c:** Heavy user of SIUnit functions
- **SIScalarConstants.c:** Uses SIUnitFromExpression, SIUnitWithSymbol
- **SIQuantity.c:** Direct unit operations
- **RMNpy:** Python wrapper depends on complete API

### Performance Considerations:
- SIUnit may have different performance characteristics
- Library loading behavior might differ
- Memory usage patterns could change

---

**Last Updated:** $(date)  
**Branch:** SIUnits-refactor  
**Risk Assessment:** High (due to API gaps)  
**Estimated Time:** 4-6 hours for complete migration
