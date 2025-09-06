#ifndef TEST_JSON_TYPED_H
#define TEST_JSON_TYPED_H
#include <stdbool.h>
// JSONTyped tests for SITypes
bool test_SIScalar_json_typed_roundtrip_simple(void);
bool test_SIScalar_json_typed_roundtrip_complex(void);
bool test_SIScalar_json_typed_roundtrip_with_units(void);
bool test_SIUnit_json_typed_roundtrip_basic(void);
bool test_SIUnit_json_typed_roundtrip_compound(void);
bool test_SIDimensionality_json_typed_roundtrip(void);
bool test_json_typed_error_handling(void);
bool test_json_typed_comprehensive_coverage(void);
#endif /* TEST_JSON_TYPED_H */
