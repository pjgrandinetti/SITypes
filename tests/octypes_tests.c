#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include "OCTypes/OCArray.h"
#include "OCTypes/OCNumber.h"
#include <stdio.h>

#ifndef PRINTERROR
#define PRINTERROR printf("failure: line %d, %s\n", __LINE__, __FUNCTION__)
#endif

bool octypesTest1(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    // Test OCString creation and comparison
    OCStringRef string1 = STR("Test String");
    OCStringRef string2 = STR("Test String");
    if (OCStringCompare(string1, string2, 0) != kOCCompareEqualTo) {
        PRINTERROR;
        return false;
    }

    // Test OCArray creation and manipulation
    OCMutableArrayRef array = OCArrayCreateMutable(0, NULL);
    OCArrayAppendValue(array, string1);
    if (OCArrayGetCount(array) != 1) {
        PRINTERROR;
        return false;
    }

    OCStringRef retrievedString = OCArrayGetValueAtIndex(array, 0);
    if (OCStringCompare(retrievedString, string1, 0) != kOCCompareEqualTo) {
        PRINTERROR;
        return false;
    }

    OCRelease(array);

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool octypesTest2(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    // Test OCString creation and comparison
    OCStringRef string1 = STR("Test String");
    OCStringRef string2 = STR("Test String");
    if (OCStringCompare(string1, string2, 0) != kOCCompareEqualTo) {
        PRINTERROR;
        return false;
    }

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool octypesTest3(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    // Test OCArray with multiple elements
    OCMutableArrayRef array = OCArrayCreateMutable(0, NULL);
    OCStringRef string1 = STR("Element 1");
    OCStringRef string2 = STR("Element 2");
    OCStringRef string3 = STR("Element 3");

    OCArrayAppendValue(array, string1);
    OCArrayAppendValue(array, string2);
    OCArrayAppendValue(array, string3);

    if (OCArrayGetCount(array) != 3) {
        PRINTERROR;
        return false;
    }

    if (OCStringCompare(OCArrayGetValueAtIndex(array, 0), string1, 0) != kOCCompareEqualTo ||
        OCStringCompare(OCArrayGetValueAtIndex(array, 1), string2, 0) != kOCCompareEqualTo ||
        OCStringCompare(OCArrayGetValueAtIndex(array, 2), string3, 0) != kOCCompareEqualTo) {
        PRINTERROR;
        return false;
    }

    OCRelease(array);

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool octypesTest4(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    // Test OCString manipulation
    OCMutableStringRef mutableString = OCStringCreateMutable(0); // Fix OCStringCreateMutable call by providing a valid capacity
    OCStringAppendCString(mutableString, "Initial String");

    if (OCStringCompare(mutableString, STR("Initial String"), 0) != kOCCompareEqualTo) {
        PRINTERROR;
        return false;
    }

    OCRelease(mutableString);

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool octypesTest5(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    // Test OCNumber creation and comparison
    OCNumberRef num1 = OCNumberCreateWithSInt32(10);
    OCNumberRef num2 = OCNumberCreateWithSInt32(20);
    OCNumberRef num3 = OCNumberCreateWithSInt32(10); // Same value as num1

    if (!OCTypeEqual(num1, num3)) {
        PRINTERROR;
        return false;
    }
    if (OCTypeEqual(num1, num2)) {
        PRINTERROR;
        return false;
    }

    OCRelease(num1);
    OCRelease(num2);
    OCRelease(num3);

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

// Add an adapter function for OCStringCompare
static OCComparisonResult OCStringCompareAdapter(const void *a, const void *b, void *context) {
    return OCStringCompare((OCStringRef)a, (OCStringRef)b, 0);
}

bool octypesTest6(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    // Test OCArray sorting
    OCMutableArrayRef array = OCArrayCreateMutable(0, NULL);
    OCStringRef str1 = STR("c");
    OCStringRef str2 = STR("a");
    OCStringRef str3 = STR("b");

    OCArrayAppendValue(array, str1);
    OCArrayAppendValue(array, str2);
    OCArrayAppendValue(array, str3);

    OCArraySortValues(array, OCRangeMake(0, OCArrayGetCount(array)), OCStringCompareAdapter, NULL); // Update OCArraySortValues call to use the adapter function

    if (OCStringCompare(OCArrayGetValueAtIndex(array, 0), str2, 0) != kOCCompareEqualTo ||
        OCStringCompare(OCArrayGetValueAtIndex(array, 1), str3, 0) != kOCCompareEqualTo ||
        OCStringCompare(OCArrayGetValueAtIndex(array, 2), str1, 0) != kOCCompareEqualTo) {
        PRINTERROR;
        return false;
    }

    OCRelease(array);

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}
