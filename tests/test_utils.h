#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "../src/SILibrary.h"
#include <assert.h>
#include <stdio.h>

/**
 * @brief Check for parsing errors and handle them consistently
 * 
 * This helper function checks if parsing was successful and if not, 
 * prints the error and fails the test with a custom message.
 * 
 * @param unit The unit reference to check
 * @param err The error reference to check and release if needed
 * @param unitSymbol The symbol of the unit being parsed (for error message)
 * @param failMessage The message to display in the assertion failure
 */
static inline void assert_unit_parsed(SIUnitRef unit, OCStringRef err, const char* unitSymbol, const char* failMessage) {
    if (!unit) {
        if (err) {
            printf("Error parsing %s: %s\n", unitSymbol, OCStringGetCString(err));
            OCRelease(err);
        }
        assert(0 && failMessage);
    }
    if (err) {
        OCRelease(err);
    }
}

#define ASSERT_PARSED(obj, err_ptr, obj_name_str, fail_msg) \
    do { \
        if (!(obj)) { \
            if (*(err_ptr)) { \
                printf("Error parsing %s: %s\\n", (obj_name_str), OCStringGetCString(*(err_ptr))); \
                OCRelease(*(err_ptr)); \
                *(err_ptr) = NULL; \
            } \
            assert(0 && (fail_msg)); \
        } \
        if (*(err_ptr)) { \
            OCRelease(*(err_ptr)); \
            *(err_ptr) = NULL; \
        } \
    } while (0)

#endif /* TEST_UTILS_H */
