# SITypes Unit Expression Functions Specification

## Background

SITypes maintains an array of approximately **951 allowed token unit symbols** including base SI units (e.g., `m`, `kg`, `s`, `A`, `K`, `mol`, `cd`), named units (e.g., `Hz`, `N`, `Pa`, `J`, `W`, `V`, `Ω`), units with SI prefixes (e.g., `km`, `mm`, `µm`, `GHz`, `MPa`), and various other recognized units from different measurement systems. These allowed token unit symbols are available from `OCMutableArrayRef SIUnitGetTokenSymbolsLib(void)`. Input expressions must be valid mathematical expressions involving only these recognized token unit symbols through the operations of multiplication, division, and raising to integer powers.

### Key Constraints:
- **Token unit symbols**: Only symbols from the predefined token unit symbols array (~951 symbols) are valid, accessible via `SIUnitGetTokenSymbolsLib()`
- **Mathematical operations**: Multiplication (`*`, `•`), division (`/`), and integer powers (`^`)
- **Forbidden operations**: Addition (`+`) and subtraction (`-`) are not allowed and should return `NULL` (even between similar units like `kg+g`)
- **Power restrictions**: Unit symbols cannot be raised to fractional powers
- **Expression validity**: All expressions must be mathematically valid combinations of the allowed units
- **Derived units**: Units become "derived" only when mathematical operations combine them (e.g., `m/s^2`, `kg*m`, `N*m/s`)

### Expression Examples:
- **Valid**: `m`, `kg*m`, `m/s^2`, `(kg*m)^2`, `N*m/s`
- **Invalid**: `m^0.5` (fractional power), `m*1` (non-unit multiplication), `xyz` (unrecognized symbol), `m+kg` (addition), `m-s` (subtraction), `kg+g` (addition of similar units)

## Overview

This document defines the expected behavior of the two main unit expression processing functions in SITypes:

1. `SIUnitCreateCleanedExpression` - Groups identical symbols without cancellation
2. `SIUnitCreateCleanedAndReducedExpression` - Performs full algebraic reduction

## Function Specifications

### SIUnitCreateCleanedExpression

**Purpose**: Groups identical unit symbols within numerator and denominator separately, **without performing any cancellation** between them. This function only organizes and consolidates symbols within each part of the expression.

**Key Behaviors**:

- Groups identical symbols and combines their powers
- Sorts symbols alphabetically within numerator and denominator
- Uses `•` (bullet) for multiplication
- Uses `^` for powers (omitted when power is 1)
- **Does NOT cancel terms between numerator and denominator**
- **Does NOT perform algebraic reduction**
- Handles parenthetical expressions by expanding them mathematically
- Validates that all symbols are from the predefined token unit symbols array

#### Input/Output Examples

| Input | Expected Output | Description |
|-------|-----------------|-------------|
| `m` | `m` | Single unit unchanged |
| `m^-1` | `(1/m)` | Single unit unchanged |
| `m*kg` | `kg•m` | Alphabetical ordering |
| `kg*m` | `kg•m` | Same result regardless of input order |
| `m*m` | `m^2` | Power consolidation |
| `m*m*kg` | `kg•m^2` | Multiple consolidation + ordering |
| `m/s` | `m/s` | Simple division |
| `m*kg/s` | `kg•m/s` | Numerator ordering |
| `m*m*kg/s/s` | `kg•m^2/s^2` | Full grouping |
| `m/s/m` | `m/(m•s)` | No cancellation, proper grouping |
| `kg*m/kg` | `kg•m/kg` | No cancellation between num/den |
| `m^2/m` | `m^2/m` | No cancellation despite common terms |
| `(m*kg)^2` | `kg^2•m^2` | Parenthetical expansion |
| `(m/s)^2` | `m^2/s^2` | Division in parentheses |
| `(m^2*kg)^3` | `kg^3•m^6` | Complex parenthetical expansion |

| `1` | `" "` | Dimensionless number input |
| `" "` | `" "` | Dimensionless space character input |
| `m^0` | `" "` | Zero power becomes dimensionless |

#### Parenthetical Expression Handling

**Simple Multiplication**: `(m*kg)^2` → `kg^2•m^2`
- Extract inner expression: `m*kg`
- Apply power to each term: `m^2*kg^2`
- Order alphabetically: `kg^2•m^2`

**Division**: `(m/s)^2` → `m^2/s^2`
- Extract inner expression: `m/s`
- Apply power to numerator and denominator: `m^2/s^2`

**Complex**: `(m^2*kg/s)^3` → `kg^3•m^6/s^3`
- Extract: `m^2*kg/s`
- Apply power: `m^6*kg^3/s^3`
- Order: `kg^3•m^6/s^3`

#### Unicode and Operator Handling

| Input | Expected Output | Description |
|-------|-----------------|-------------|
| `m•kg` | `kg•m` | Unicode bullet preserved |
| `m×kg` | `kg•m` | Unicode multiply → bullet |
| `m÷s` | `m/s` | Unicode divide → slash |
| `μm` | `µm` | Greek mu → micro sign |

#### Error Cases

| Input | Expected Output | Description |
|-------|-----------------|-------------|
| `m^0.5` | `NULL` | Fractional powers rejected |
| `m^(1/2)` | `NULL` | Fractional powers rejected |
| `m*1` | `NULL` | Invalid multiplication |
| `m+kg` | `NULL` | Addition not allowed |
| `m-s` | `NULL` | Subtraction not allowed |
| `kg+g` | `NULL` | Addition of similar units not allowed |
| `m-mm` | `NULL` | Subtraction of similar units not allowed |
| `invalid` | `NULL` | Unrecognized symbols |

---

### SIUnitCreateCleanedAndReducedExpression

**Purpose**: Performs full algebraic reduction by canceling terms between numerator and denominator.

**Key Behaviors**:
- All behaviors of `SIUnitCreateCleanedExpression`
- ADDITIONALLY cancels identical terms between numerator and denominator
- Reduces the expression to its simplest form

#### Input/Output Examples

| Input | Expected Output | Description |
|-------|-----------------|-------------|
| `m` | `m` | Single unit unchanged |
| `m*kg` | `kg•m` | Same as cleaned (no reduction) |
| `m/m` | `" "` | Complete cancellation |
| `m^2/m` | `m` | Partial cancellation |
| `m*kg/kg` | `m` | Cancel kg terms |
| `m*m*kg/s/s` | `kg•m^2/s^2` | Same as cleaned (no cancellation) |
| `m/s/m` | `(1/s)` | Cancel m terms |
| `kg*m/kg` | `m` | Cancel kg terms |
| `m^3/m^2` | `m` | Power subtraction |
| `m^6/m^2` | `m^4` | Power subtraction |
| `kg•m^2/(kg•m)` | `m` | Complex cancellation |

#### Cancellation Rules

1. **Identical symbols**: `m/m` → `" "`
2. **Power subtraction**: `m^3/m^2` → `m^(3-2)` → `m`
3. **Complete elimination**: `kg*m/kg` → `m`
4. **Multiple cancellations**: `kg•m^3/(kg•m^2)` → `m`

---

## Implementation Requirements

### Memory Safety
- All functions must be AddressSanitizer compliant
- No heap-buffer-overflows or use-after-free errors
- Proper cleanup of all allocated memory

### Unicode Support
- Use OCString functions for Unicode-safe processing (avoid C string functions)
- Handle UTF-8 encoded characters properly
- Normalize Unicode variants (μ → µ)
- Leverage OCString API for all string operations to ensure Unicode compliance

### Security
- Reject all invalid expressions safely
- No silent failures that could cause incorrect behavior
- Comprehensive validation of input symbols

### Performance
- Efficient parsing and processing
- Minimal memory allocations
- Fast string operations using OCString (not C string functions)

### Parser Architecture
- **Recommended approach**: Use lex/yacc parser following existing project patterns
- **Concurrency support**: Use unique prefix (e.g., `siue`) to enable concurrent parser execution
- **Integration**: Follow existing parser patterns (`siu` for units, `sis` for scalars)
- **Example structure**:
  - `SIUnitExpressionScanner.l` with `%option prefix="siue"`
  - `SIUnitExpressionParser.y` with `%name-prefix="siue"`
  - OCString integration for all string operations
  - Consistent error handling with `siueError` variable
- **Benefits**: Robust grammar handling, proper precedence, extensibility, namespace isolation for concurrency

---

## Test Cases Summary

### Critical Test Cases for SIUnitCreateCleanedExpression

1. **Basic Ordering**: `m*kg` → `kg•m`
2. **Power Consolidation**: `m*m*kg` → `kg•m^2`
3. **No Cancellation**: `m/s/m` → `m/(m•s)`
4. **Parenthetical**: `(m*kg)^2` → `kg^2•m^2`
5. **Division Parenthetical**: `(m/s)^2` → `m^2/s^2`
6. **Unicode**: `m•kg` → `kg•m`, `m÷s` → `m/s`
7. **Error Handling**: `m^0.5` → `NULL`

### Critical Test Cases for SIUnitCreateCleanedAndReducedExpression

1. **Simple Cancellation**: `m/m` → `" "`
2. **Power Reduction**: `m^3/m^2` → `m`
3. **Partial Cancellation**: `kg*m/kg` → `m`
4. **Complex Reduction**: `kg•m^3/(kg•m^2)` → `m`
5. **No Reduction Needed**: `m*kg/s` → `kg•m/s`

---

## Design Decisions

Based on requirements analysis, the following design decisions have been finalized:

1. **Ordering Convention**: **Alphabetical ordering** for all unit symbols within numerator and denominator
2. **Power of 1**: **Always simplified** - `m^1` becomes `m` in all cases
3. **Complete Cancellation**: Returns `"1"` (the string "1", not empty string)
4. **Error Handling**: **Return `NULL`** for all invalid inputs
5. **Unicode Normalization**: Handled by **separate preprocessing function** called on initial expression
6. **Power Simplification**: Powers of 1 are omitted in output (e.g., `m^1` → `m`)

---

## Unicode Preprocessing Function

The existing `SIUnitCreateNormalizedExpression` function should be used for Unicode normalization before parsing:

```c
OCMutableStringRef SIUnitCreateNormalizedExpression(OCStringRef expression, bool forLibraryLookup);
```

**Existing Normalization Rules**:

### Multiplication Symbol Normalization
- `×` (U+00D7) → `*` (multiplication sign)
- `·` (U+00B7) → `*` (middle dot)
- `⋅` (U+22C5) → `*` (dot operator)
- `∙` (U+2219) → `*` (bullet operator)

### Division Symbol Normalization
- `÷` (U+00F7) → `/` (division sign)
- `∕` (U+2215) → `/` (division slash)
- `⁄` (U+2044) → `/` (fraction slash)

### Micro Symbol Normalization (to µ = U+00B5)
- `μ` (U+03BC) → `µ` (Greek small letter mu)
- `Μ` (U+039C) → `µ` (Greek capital letter mu)
- `ɥ` (U+0265) → `µ` (Latin small letter turned h)
- `𝜇` (U+1D707) → `µ` (mathematical italic small mu)
- `𝝁` (U+1D741) → `µ` (mathematical bold small mu)
- `𝝻` (U+1D77B) → `µ` (mathematical bold italic small mu)

### Whitespace and Formatting
- Removes spaces around operators (`" * "` → `*`, `" / "` → `/`, `" ^ "` → `^`)
- Trims leading/trailing whitespace
- For library lookup: `*` → `•` (bullet for consistent representation)
- For display: `•` → `*` (asterisk for multiplication)

---

## Comprehensive Edge Case Testing Requirements

The implementation must be rigorously tested for **all possible edge cases** with the following guarantees:

### Robustness Requirements
1. **Never crash** - no segmentation faults, memory errors, or exceptions
2. **Never return invalid strings** - all outputs must be well-formed or `NULL`
3. **Never return incorrect strings** - all valid outputs must be mathematically accurate

### Critical Edge Case Categories

#### 1. Memory and String Safety
- **Empty strings**: `""`, `NULL` input
- **Very long expressions**: Test with expressions containing hundreds of units
- **Memory exhaustion**: Behavior under low memory conditions
- **Buffer boundaries**: Strings at exact buffer size limits
- **Unicode edge cases**: Malformed UTF-8, mixed encodings

#### 2. Mathematical Edge Cases
- **Zero powers**: `m^0` (should return `NULL`)
- **Negative powers**: `m^-1`, `m^-5`
- **Large powers**: `m^999`, `m^-999`
- **Power overflow**: Powers beyond integer limits
- **Complete cancellation variants**: `m/m`, `m^5/m^5`, `(m*kg)/(m*kg)`

#### 3. Parsing Edge Cases
- **Malformed parentheses**: `(m*kg`, `m*kg)`, `((m))`, `()`
- **Invalid operators**: `m**kg`, `m//s`, `m^^^2`
- **Mixed operators**: `m*/kg`, `m/*kg`
- **Operator precedence**: Complex nested expressions
- **Trailing operators**: `m*`, `m/`, `m^`

#### 4. Symbol Validation Edge Cases
- **Case sensitivity**: `M` vs `m`, `KG` vs `kg`
- **Similar symbols**: `m` vs `μ`, `O` (letter) vs `0` (zero)
- **Partial matches**: `meter`, `kilogram` (words vs symbols)
- **Special characters**: `m$`, `m#`, `m@`
- **Numbers in symbols**: `m2`, `2m`, `m1`

#### 5. Division Edge Cases
- **Multiple divisions**: `m/s/kg`, `m/s/kg/A`
- **Division by compound**: `m/(s*kg)`, `m/(s^2*kg)`
- **Nested divisions**: `(m/s)/(kg/A)`
- **Division by power**: `m^2/s^3`, `m^6/s^2`

#### 6. Parenthetical Edge Cases
- **Nested parentheses**: `((m*kg)^2)`, `(m*(kg*s))^2`
- **Division in parentheses**: `(m/s/kg)^2`
- **Powers of parentheses**: `(m*kg)^0`, `(m*kg)^-1`
- **Empty parentheses**: `()`, `()^2`
- **Single unit in parentheses**: `(m)`, `(m)^2`

#### 7. Unicode and Encoding Edge Cases
- **Mixed Unicode**: `μm•kg×s÷A`
- **Invalid Unicode**: Corrupted UTF-8 sequences
- **Unicode normalization**: Multiple representations of same character
- **Non-ASCII symbols**: Chinese, Arabic characters
- **Control characters**: Tabs, newlines within expressions

#### 8. Cancellation Edge Cases
- **Partial cancellation**: `m^5/m^2`, `m^2/m^5`
- **Multiple symbol cancellation**: `kg*m*s/(kg*m)`, `kg^2*m^3/(kg*m^2)`
- **Complex reduction**: `(kg*m^2*s)/(kg^2*m*s^2)`
- **No cancellation possible**: `m*kg/s*A`

### Testing Framework Requirements

1. **Automated test suite** with thousands of test cases
2. **Property-based testing** for random expression generation
3. **Memory leak detection** using AddressSanitizer/Valgrind
4. **Stress testing** with high-volume inputs
5. **Fuzzing** with malformed inputs
6. **Unicode compliance testing** with various encodings
7. **Performance benchmarking** for large expressions

### Test Data Requirements

- **Comprehensive symbol coverage**: Test all ~951 valid symbols
- **Boundary conditions**: Empty inputs, single characters, maximum lengths
- **Combinatorial testing**: All operator combinations
- **Error path coverage**: Every possible error condition
- **Regression testing**: Previous bug scenarios

The implementation must pass **100% of these edge case tests** before being considered production-ready.

---

## Implementation Plan

### **Architecture: Hybrid Lex/Yacc + OCString Approach**

The implementation will follow the existing lex/yacc pattern with OCString integration, leveraging established codebase patterns for consistency and reliability.

### **Phase 1: Foundation (Week 1-2)**

#### **Files to Create**
```
SIUnitExpressionScanner.l   // Lexical analysis with siue prefix
SIUnitExpressionParser.y    // Grammar with siue prefix  
SIUnitExpression.h          // Public API declarations
SIUnitExpression.c          // Implementation logic
test_unitexpression.c       // Comprehensive test suite
test_unitexpression.h       // Test declarations
```

#### **Core Data Structures**
```c
typedef struct SIUnitTerm {
    OCStringRef symbol;     // Unit symbol (e.g., "m", "kg")
    int power;             // Power of the symbol
} SIUnitTerm;

typedef struct SIUnitExpression {
    OCArrayRef numerator;   // Array of SIUnitTerm
    OCArrayRef denominator; // Array of SIUnitTerm
} SIUnitExpression;
```

#### **Public API Functions**
```c
OCStringRef SIUnitCreateCleanedExpression(OCStringRef expression);
OCStringRef SIUnitCreateCleanedAndReducedExpression(OCStringRef expression);
```

#### **Foundation Tasks**
1. **Set up lex/yacc parser** with `siue` prefix for namespace isolation
2. **Implement basic expression parsing** for simple cases (`m`, `m*kg`, `m/s`)
3. **Symbol validation** against existing ~951 token unit symbols array (via `SIUnitGetTokenSymbolsLib()`)
4. **Unicode normalization integration** using `SIUnitCreateNormalizedExpression`
5. **Memory management framework** following OCTypes patterns
6. **Basic test infrastructure** with AddressSanitizer validation

### **Phase 2: Core Logic (Week 3-4)**

#### **SIUnitCreateCleanedExpression Implementation**
1. **Expression parsing** into numerator/denominator arrays
2. **Symbol grouping** within each array (combine identical symbols)
3. **Power consolidation** (e.g., `m*m*kg` → `kg•m^2`)
4. **Alphabetical sorting** within numerator and denominator
5. **Parenthetical expression handling** with proper expansion
6. **Output formatting** with `•` and `^` notation (powers of 1 omitted)

#### **Key Implementation Functions**
```c
// Internal functions
static SIUnitExpression* siueParseExpression(OCStringRef normalized);
static void siueGroupIdenticalTerms(OCMutableArrayRef terms);
static void siueSortTermsAlphabetically(OCMutableArrayRef terms);
static OCStringRef siueCreateFormattedExpression(SIUnitExpression* expr, bool reduced);
static bool siueValidateSymbol(OCStringRef symbol); // Check against SIUnitGetTokenSymbolsLib()
```

#### **Error Handling**
- Return `NULL` for all invalid inputs
- Comprehensive validation at each parsing step
- No silent failures or incorrect outputs

### **Phase 3: Reduction Logic (Week 5-6)**

#### **SIUnitCreateCleanedAndReducedExpression Implementation**
1. **All functionality** from `SIUnitCreateCleanedExpression`
2. **Cancellation algorithm** between numerator and denominator
3. **Power subtraction** (e.g., `m^3/m^2` → `m`)
4. **Complete cancellation handling** (e.g., `m/m` → `"1"`)
5. **Complex reduction** (e.g., `kg•m^3/(kg•m^2)` → `m`)

#### **Cancellation Algorithm**
```c
static void siueCancelTerms(SIUnitExpression* expr) {
    // For each symbol in numerator
    //   Find matching symbol in denominator
    //   Subtract powers
    //   Remove zero-power terms
    //   Handle complete cancellation
}
```

#### **Edge Cases**
- Handle partial cancellation (`m^5/m^2` → `m^3`)
- Handle negative result powers (`m^2/m^5` → `(1/m^3)`)
- Handle complete cancellation → return `"1"`

### **Phase 4: Testing & Validation (Week 7-8)**

#### **Comprehensive Test Suite**
1. **All 328 edge cases** specified in the document
2. **Memory safety testing** with AddressSanitizer/Valgrind
3. **Unicode compliance testing** with various encodings
4. **Performance benchmarking** with large expressions
5. **Stress testing** with hundreds of units
6. **Fuzzing** with malformed inputs

#### **Test Categories**
- **Basic functionality**: Simple expressions and expected outputs
- **Edge cases**: All categories from specification
- **Error handling**: Invalid inputs return `NULL`
- **Memory safety**: No leaks, overflows, or use-after-free
- **Performance**: Acceptable speed for large expressions
- **Unicode**: Proper handling of all normalization cases

#### **Validation Criteria**
- **100% edge case test passage**
- **Zero memory leaks** (AddressSanitizer clean)
- **Zero crashes** on any input
- **Correct mathematical output** for all valid inputs
- **NULL return** for all invalid inputs

### **Implementation Timeline**

| Week | Focus | Deliverables |
|------|-------|-------------|
| 1-2 | Foundation | Parser setup, basic parsing, symbol validation |
| 3-4 | Core Logic | SIUnitCreateCleanedExpression complete |
| 5-6 | Reduction | SIUnitCreateCleanedAndReducedExpression complete |
| 7-8 | Testing | Comprehensive validation, performance tuning |

### **Risk Mitigation Strategies**

#### **Complexity Management**
- Start with simple cases, incrementally add complexity
- Extensive unit testing at each development step
- Regular AddressSanitizer validation during development

#### **Performance Considerations**
- Profile with large expressions early in development
- Optimize string operations using OCString efficiently
- Minimize memory allocations in hot paths

#### **Memory Safety**
- Follow OCTypes retain/release patterns exactly
- Implement comprehensive cleanup on all exit paths
- Use AddressSanitizer throughout development

#### **Maintainability**
- Follow existing code patterns and conventions exactly
- Comprehensive inline documentation
- Clear separation of parsing, processing, and formatting concerns

### **Success Criteria**

The implementation will be considered complete and production-ready when:

1. **All specified functionality** works correctly
2. **All 328 edge cases** pass testing
3. **Zero memory safety issues** (AddressSanitizer clean)
4. **Performance** meets or exceeds existing parser performance
5. **Code quality** matches existing codebase standards
6. **Documentation** is complete and accurate

### **Integration with Existing Codebase**

- **Namespace isolation**: Use `siue` prefix for all parser functions
- **Concurrency support**: Enable parallel parser execution
- **OCString integration**: Use OCString API for all string operations
- **Error handling**: Follow existing error patterns (`siueError` variable)
- **Memory management**: Follow OCTypes patterns exactly
- **Testing integration**: Add to existing test framework
