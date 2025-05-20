#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include "OCTypes/OCArray.h"
#include "OCTypes/OCNumber.h"
#include <assert.h>

void octypesTest1(void) {
    // Test OCString creation and comparison
    OCStringRef string1 = STR("Test String");
    OCStringRef string2 = STR("Test String");
    assert(OCStringCompare(string1, string2, 0) == kOCCompareEqualTo);

    // Test OCArray creation and manipulation
    OCMutableArrayRef array = OCArrayCreateMutable(0, NULL);
    OCArrayAppendValue(array, string1);
    assert(OCArrayGetCount(array) == 1);

    OCStringRef retrievedString = OCArrayGetValueAtIndex(array, 0);
    assert(OCStringCompare(retrievedString, string1, 0) == kOCCompareEqualTo);

    OCRelease(array);
}

void octypesTest2(void) {
    // Test OCString creation and comparison
    OCStringRef string1 = STR("Test String");
    OCStringRef string2 = STR("Test String");
    assert(OCStringCompare(string1, string2, 0) == kOCCompareEqualTo);
}

void octypesTest3(void) {
    // Test OCArray with multiple elements
    OCMutableArrayRef array = OCArrayCreateMutable(0, NULL);
    OCStringRef string1 = STR("Element 1");
    OCStringRef string2 = STR("Element 2");
    OCStringRef string3 = STR("Element 3");

    OCArrayAppendValue(array, string1);
    OCArrayAppendValue(array, string2);
    OCArrayAppendValue(array, string3);

    assert(OCArrayGetCount(array) == 3);

    assert(OCStringCompare(OCArrayGetValueAtIndex(array, 0), string1, 0) == kOCCompareEqualTo);
    assert(OCStringCompare(OCArrayGetValueAtIndex(array, 1), string2, 0) == kOCCompareEqualTo);
    assert(OCStringCompare(OCArrayGetValueAtIndex(array, 2), string3, 0) == kOCCompareEqualTo);

    OCRelease(array);
}

void octypesTest4(void) {
    // Test OCString manipulation
    OCMutableStringRef mutableString = OCStringCreateMutable(0);
    OCStringAppendCString(mutableString, "Initial String");

    assert(OCStringCompare(mutableString, STR("Initial String"), 0) == kOCCompareEqualTo);

    OCRelease(mutableString);
}

void octypesTest5(void) {
    // Test OCNumber creation and comparison
    OCNumberRef num1 = OCNumberCreateWithSInt32(10);
    OCNumberRef num2 = OCNumberCreateWithSInt32(20);
    OCNumberRef num3 = OCNumberCreateWithSInt32(10);

    assert(OCTypeEqual(num1, num3));
    assert(!OCTypeEqual(num1, num2));

    OCRelease(num1);
    OCRelease(num2);
    OCRelease(num3);
}

// Adapter remains the same
static OCComparisonResult OCStringCompareAdapter(const void *a, const void *b, void *context) {
    return OCStringCompare((OCStringRef)a, (OCStringRef)b, 0);
}

void octypesTest6(void) {
    // Test OCArray sorting
    OCMutableArrayRef array = OCArrayCreateMutable(0, NULL);
    OCStringRef str1 = STR("c");
    OCStringRef str2 = STR("a");
    OCStringRef str3 = STR("b");

    OCArrayAppendValue(array, str1);
    OCArrayAppendValue(array, str2);
    OCArrayAppendValue(array, str3);

    OCArraySortValues(array, OCRangeMake(0, OCArrayGetCount(array)), OCStringCompareAdapter, NULL);

    assert(OCStringCompare(OCArrayGetValueAtIndex(array, 0), str2, 0) == kOCCompareEqualTo);
    assert(OCStringCompare(OCArrayGetValueAtIndex(array, 1), str3, 0) == kOCCompareEqualTo);
    assert(OCStringCompare(OCArrayGetValueAtIndex(array, 2), str1, 0) == kOCCompareEqualTo);

    OCRelease(array);
}
