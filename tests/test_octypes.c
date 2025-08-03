#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SITypes.h"
bool octypesTest1(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef string1 = STR("Test String");
    OCStringRef string2 = STR("Test String");
    OCMutableArrayRef array = NULL;  // Initialize to NULL to avoid uninitialized use
    if (OCStringCompare(string1, string2, 0) != kOCCompareEqualTo) {
        success = false;
        goto cleanup;
    }
    array = OCArrayCreateMutable(0, NULL);
    if (!array) return false;
    OCArrayAppendValue(array, string1);
    if (OCArrayGetCount(array) != 1) {
        success = false;
        goto cleanup;
    }
    OCStringRef retrievedString = OCArrayGetValueAtIndex(array, 0);
    if (OCStringCompare(retrievedString, string1, 0) != kOCCompareEqualTo) {
        success = false;
        goto cleanup;
    }
cleanup:
    if (array) OCRelease(array);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool octypesTest2(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef string1 = STR("Test String");
    OCStringRef string2 = STR("Test String");
    if (OCStringCompare(string1, string2, 0) != kOCCompareEqualTo) {
        success = false;
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool octypesTest3(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCMutableArrayRef array = OCArrayCreateMutable(0, NULL);
    if (!array) return false;
    OCStringRef string1 = STR("Element 1");
    OCStringRef string2 = STR("Element 2");
    OCStringRef string3 = STR("Element 3");
    OCArrayAppendValue(array, string1);
    OCArrayAppendValue(array, string2);
    OCArrayAppendValue(array, string3);
    if (OCArrayGetCount(array) != 3) {
        success = false;
        goto cleanup;
    }
    if (OCStringCompare(OCArrayGetValueAtIndex(array, 0), string1, 0) != kOCCompareEqualTo ||
        OCStringCompare(OCArrayGetValueAtIndex(array, 1), string2, 0) != kOCCompareEqualTo ||
        OCStringCompare(OCArrayGetValueAtIndex(array, 2), string3, 0) != kOCCompareEqualTo) {
        success = false;
    }
cleanup:
    OCRelease(array);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool octypesTest4(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCMutableStringRef mutableString = OCStringCreateMutable(0);
    if (!mutableString) return false;
    OCStringAppendCString(mutableString, "Initial String");
    if (OCStringCompare(mutableString, STR("Initial String"), 0) != kOCCompareEqualTo) {
        success = false;
    }
    OCRelease(mutableString);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool octypesTest5(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCNumberRef num1 = OCNumberCreateWithSInt32(10);
    OCNumberRef num2 = OCNumberCreateWithSInt32(20);
    OCNumberRef num3 = OCNumberCreateWithSInt32(10);
    if (!OCTypeEqual(num1, num3)) success = false;
    if (OCTypeEqual(num1, num2)) success = false;
    OCRelease(num1);
    OCRelease(num2);
    OCRelease(num3);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
// Adapter remains the same
static OCComparisonResult OCStringCompareAdapter(const void *a, const void *b, void *context) {
    return OCStringCompare((OCStringRef)a, (OCStringRef)b, 0);
}
bool octypesTest6(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCMutableArrayRef array = OCArrayCreateMutable(0, NULL);
    OCStringRef str1 = STR("c");
    OCStringRef str2 = STR("a");
    OCStringRef str3 = STR("b");
    OCArrayAppendValue(array, str1);
    OCArrayAppendValue(array, str2);
    OCArrayAppendValue(array, str3);
    OCArraySortValues(array, OCRangeMake(0, OCArrayGetCount(array)), OCStringCompareAdapter, NULL);
    if (OCStringCompare(OCArrayGetValueAtIndex(array, 0), str2, 0) != kOCCompareEqualTo) success = false;
    if (OCStringCompare(OCArrayGetValueAtIndex(array, 1), str3, 0) != kOCCompareEqualTo) success = false;
    if (OCStringCompare(OCArrayGetValueAtIndex(array, 2), str1, 0) != kOCCompareEqualTo) success = false;
    OCRelease(array);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
