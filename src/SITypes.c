#include "SITypes.h"
#include "SIDimensionalityPrivate.h"
#include "SIScalarConstants.h"
#include <stdlib.h>

// GCC compatibility: __has_feature is Clang-specific
#ifndef __has_feature
#define __has_feature(x) 0
#endif


OCStringRef SITypesCreateStringRepresentation(OCTypeRef obj) {
    if (!obj) return NULL;
    OCTypeID type = OCGetTypeID(obj);
    if (type == SIScalarGetTypeID()) {
        return SIScalarCreateStringValue((SIScalarRef)obj);
    } 
    else if (type == SIUnitGetTypeID()) {
        return SIUnitCopySymbol((SIUnitRef)obj);
    }
    else if (type == SIDimensionalityGetTypeID()) {
        return SIDimensionalityCopySymbol((SIDimensionalityRef)obj);
    }
    return NULL;
}
OCTypeRef SITypesCreateByReducing(OCTypeRef a)
{
    OCTypeID typeA = OCGetTypeID(a);
    
    // Perform the reduction operation
    OCTypeRef result = NULL;
    if (typeA == SIScalarGetTypeID()) {
        result = (OCTypeRef)SIScalarCreateByReducingUnit((SIScalarRef)a);
    } 
    else if (typeA == SIUnitGetTypeID()) {
        double unit_multiplier = 1.0;
        result = (OCTypeRef)SIUnitByReducing((SIUnitRef)a, &unit_multiplier);
    }
    else if (typeA == SIDimensionalityGetTypeID()) {
        result = (OCTypeRef)SIDimensionalityByReducing((SIDimensionalityRef)a);
    }

    return result;
}

OCTypeRef SITypesCreateByTakingNthRoot(OCTypeRef a, int root, OCStringRef *outError)
{
    if (!a) {
        if (outError) *outError = STR("Invalid argument");
        return NULL;
    }

    OCTypeID typeA = OCGetTypeID(a);
    
    // Perform the nth root operation
    OCTypeRef result = NULL;
    if (typeA == SIScalarGetTypeID()) {
        result = (OCTypeRef)SIScalarCreateByTakingNthRoot((SIScalarRef)a, root, outError);
    } 
    else if (typeA == SIUnitGetTypeID()) {
        double unit_multiplier = 1.0;
        result = (OCTypeRef)SIUnitByTakingNthRoot((SIUnitRef)a, root, &unit_multiplier, outError);
    }
    else if (typeA == SIDimensionalityGetTypeID()) {
        result = (OCTypeRef)SIDimensionalityByTakingNthRoot((SIDimensionalityRef)a, root, outError);
    }
    else {
        if (outError) *outError = STR("Unsupported type for nth root operation");
        return NULL;
    }

    return result;
}
OCTypeRef SITypesCreateByRaisingToPower(OCTypeRef a, int power, OCStringRef *outError)
{
    if (!a) {
        if (outError) *outError = STR("Invalid argument");
        return NULL;
    }

    OCTypeID typeA = OCGetTypeID(a);
    
    // Perform the power arithmetic operation
    OCTypeRef result = NULL;
    if (typeA == SIScalarGetTypeID()) {
        result = (OCTypeRef)SIScalarCreateByRaisingToPower((SIScalarRef)a, power, outError);
    } 
    else if (typeA == SIUnitGetTypeID()) {
        double unit_multiplier = 1.0;
        result = (OCTypeRef)SIUnitByRaisingToPower((SIUnitRef)a, power, &unit_multiplier, outError);
    }
    else if (typeA == SIDimensionalityGetTypeID()) {
        result = (OCTypeRef)SIDimensionalityByRaisingToPower((SIDimensionalityRef)a, power);
    }
    else {
        if (outError) *outError = STR("Unsupported type for power operation");
        return NULL;
    }

    return result;
}

OCTypeRef SITypesCreateWithBinaryArithmeticOperation(OCTypeRef a, OCTypeRef b, char op, OCStringRef *outError)
{
    if (!a || !b) {
        if (outError) *outError = STR("Invalid arguments");
        return NULL;
    }

    OCTypeID typeA = OCGetTypeID(a);
    OCTypeID typeB = OCGetTypeID(b);
    if(typeA != typeB) {
        if (outError) *outError = STR("Type mismatch");
        return NULL;
    }
    
    // Perform the binary arithmetic operation
    OCTypeRef result = NULL;
    if (op == '+') {
        if(typeA == SIScalarGetTypeID()) {
            result = (OCTypeRef)SIScalarCreateByAdding((SIScalarRef)a, (SIScalarRef)b, outError);
        } 
        else if(typeA == SIUnitGetTypeID()) {
            // Units don't add - this would be an error
            if (outError) *outError = STR("Cannot add units");
            return NULL;
        }
        else if(typeA == SIDimensionalityGetTypeID()) {
            // Dimensionalities don't add - this would be an error
            if (outError) *outError = STR("Cannot add dimensionalities");
            return NULL;
        }
    } else if (op == '-') {
        if(typeA == SIScalarGetTypeID()) {
            result = (OCTypeRef)SIScalarCreateBySubtracting((SIScalarRef)a, (SIScalarRef)b, outError);
        } 
        else if(typeA == SIUnitGetTypeID()) {
            // Units don't subtract - this would be an error
            if (outError) *outError = STR("Cannot subtract units");
            return NULL;
        }
        else if(typeA == SIDimensionalityGetTypeID()) {
            // Dimensionalities don't subtract - this would be an error
            if (outError) *outError = STR("Cannot subtract dimensionalities");
            return NULL;
        }
    } else if (op == '*') {
        if(typeA == SIScalarGetTypeID()) {
            result = (OCTypeRef)SIScalarCreateByMultiplying((SIScalarRef)a, (SIScalarRef)b, outError);
        } 
        else if(typeA == SIUnitGetTypeID()) {
            double unit_multiplier = 1;
            result = (OCTypeRef)SIUnitByMultiplying((SIUnitRef)a, (SIUnitRef)b, &unit_multiplier, outError);
        }
        else if(typeA == SIDimensionalityGetTypeID()) {
            result = (OCTypeRef)SIDimensionalityByMultiplying((SIDimensionalityRef)a, (SIDimensionalityRef)b);
        }
    } else if (op == '/') {
        if(typeA == SIScalarGetTypeID()) {
            result = (OCTypeRef)SIScalarCreateByDividing((SIScalarRef)a, (SIScalarRef)b, outError);
        } 
        else if(typeA == SIUnitGetTypeID()) {
            double unit_multiplier = 1;
            result = (OCTypeRef)SIUnitByDividing((SIUnitRef)a, (SIUnitRef)b, &unit_multiplier, outError);
        }
        else if(typeA == SIDimensionalityGetTypeID()) {
            result = (OCTypeRef)SIDimensionalityByDividing((SIDimensionalityRef)a, (SIDimensionalityRef)b);
        }
    } else {
        if (outError) *outError = STR("Unknown operation");
        return NULL;
    }

    return result;
}

// Enhanced JSON serialization for OCDictionary containing SITypes objects
cJSON *SITypesMetadataCopyJSON(OCDictionaryRef dict) {
    if (!dict) return cJSON_CreateNull();
    cJSON *json = cJSON_CreateObject();
    OCArrayRef keys = OCDictionaryCreateArrayWithAllKeys(dict);
    if (!keys) return json;
    for (OCIndex i = 0; i < OCArrayGetCount(keys); ++i) {
        OCStringRef key = (OCStringRef)OCArrayGetValueAtIndex(keys, i);
        OCTypeRef val = OCDictionaryGetValue(dict, key);
        if (!key || !val) continue;
        cJSON *entry = cJSON_CreateObject();
        OCTypeID tid = OCGetTypeID(val);
        if (tid == OCStringGetTypeID()) {
            cJSON_AddStringToObject(entry, "type", "string");
            cJSON_AddStringToObject(entry, "value", OCStringGetCString((OCStringRef)val));
        } else if (tid == OCNumberGetTypeID()) {
            double f = 0.0;
            OCNumberTryGetDouble((OCNumberRef)val, &f);
            cJSON_AddStringToObject(entry, "type", "float64");
            cJSON_AddNumberToObject(entry, "value", f);
        } else if (tid == OCBooleanGetTypeID()) {
            cJSON_AddStringToObject(entry, "type", "bool");
            cJSON_AddBoolToObject(entry, "value", OCBooleanGetValue((OCBooleanRef)val));
        } else if (tid == OCDataGetTypeID()) {
            OCStringRef b64 = OCDataCreateBase64EncodedString((OCDataRef)val, 0);
            cJSON_AddStringToObject(entry, "type", "binary");
            cJSON_AddStringToObject(entry, "value", OCStringGetCString(b64));
            OCRelease(b64);
        } else if (tid == SIScalarGetTypeID()) {
            OCStringRef s = SIScalarCreateStringValue((SIScalarRef)val);
            cJSON_AddStringToObject(entry, "type", "scalar");
            cJSON_AddStringToObject(entry, "value", OCStringGetCString(s));
            OCRelease(s);
        } else if (tid == SIUnitGetTypeID()) {
            OCStringRef s = SIUnitCopySymbol((SIUnitRef)val);
            cJSON_AddStringToObject(entry, "type", "unit");
            cJSON_AddStringToObject(entry, "value", OCStringGetCString(s));
            OCRelease(s);
        } else if (tid == OCDictionaryGetTypeID()) {
            cJSON *nested = SITypesMetadataCopyJSON((OCDictionaryRef)val);
            cJSON_AddStringToObject(entry, "type", "dictionary");
            cJSON_AddItemToObject(entry, "value", nested);
        } else if (tid == OCArrayGetTypeID()) {
            cJSON *arrJSON = cJSON_CreateArray();
            OCArrayRef array = (OCArrayRef)val;
            for (OCIndex j = 0; j < OCArrayGetCount(array); ++j) {
                OCTypeRef elem = OCArrayGetValueAtIndex(array, j);
                OCMutableDictionaryRef singleton = OCDictionaryCreateMutable(0);
                OCDictionarySetValue(singleton, STR("elem"), elem);
                cJSON *wrapped = SITypesMetadataCopyJSON(singleton);
                OCRelease(singleton);
                if (wrapped) {
                    cJSON *e = cJSON_GetObjectItem(wrapped, "elem");
                    if (e) cJSON_AddItemToArray(arrJSON, cJSON_Duplicate(e, 1));
                    cJSON_Delete(wrapped);
                }
            }
            cJSON_AddStringToObject(entry, "type", "array");
            cJSON_AddItemToObject(entry, "value", arrJSON);
        }
        cJSON_AddItemToObject(json, OCStringGetCString(key), entry);
    }
    OCRelease(keys);
    return json;
}

OCDictionaryRef SITypesMetadataCreateFromJSON(cJSON *json, OCStringRef *outError) {
    if (outError) *outError = NULL;
    if (!json || !cJSON_IsObject(json)) {
        if (outError) *outError = STR("Expected JSON object for metadata");
        return NULL;
    }
    OCMutableDictionaryRef dict = OCDictionaryCreateMutable(0);
    cJSON *entry = NULL;
    cJSON_ArrayForEach(entry, json) {
        const char *key = entry->string;
        if (!cJSON_IsObject(entry)) continue;
        cJSON *type = cJSON_GetObjectItem(entry, "type");
        cJSON *value = cJSON_GetObjectItem(entry, "value");
        if (!cJSON_IsString(type) || !value) continue;
        OCStringRef k = OCStringCreateWithCString(key);
        OCTypeRef v = NULL;
        if (strcmp(type->valuestring, "string") == 0 && cJSON_IsString(value)) {
            v = (OCTypeRef)OCStringCreateWithCString(value->valuestring);
        } else if (strcmp(type->valuestring, "float64") == 0 && cJSON_IsNumber(value)) {
            v = (OCTypeRef)OCNumberCreateWithDouble(value->valuedouble);
        } else if (strcmp(type->valuestring, "bool") == 0 && cJSON_IsBool(value)) {
            v = (OCTypeRef)OCBooleanGetWithBool(cJSON_IsTrue(value));
        } else if (strcmp(type->valuestring, "binary") == 0 && cJSON_IsString(value)) {
            OCStringRef b64 = OCStringCreateWithCString(value->valuestring);
            v = (OCTypeRef)OCDataCreateFromBase64EncodedString(b64);
            OCRelease(b64);
        } else if (strcmp(type->valuestring, "scalar") == 0 && cJSON_IsString(value)) {
            OCStringRef s = OCStringCreateWithCString(value->valuestring);
            v = (OCTypeRef)SIScalarCreateFromExpression(s, outError);
            OCRelease(s);
            if (!v) {
                OCRelease(k);
                OCRelease(dict);
                return NULL;
            }
        } else if (strcmp(type->valuestring, "unit") == 0 && cJSON_IsString(value)) {
            OCStringRef s = OCStringCreateWithCString(value->valuestring);
            SIScalarRef tmpScalar = SIScalarCreateFromExpression(s, outError);
            OCRelease(s);
            if (!tmpScalar) {
                OCRelease(k);
                OCRelease(dict);
                return NULL;
            }
            v = (OCTypeRef)SIQuantityGetUnit((SIQuantityRef)tmpScalar);
            v = (OCTypeRef)OCRetain(v);
            OCRelease(tmpScalar);
        } else if (strcmp(type->valuestring, "dictionary") == 0 && cJSON_IsObject(value)) {
            v = (OCTypeRef)SITypesMetadataCreateFromJSON(value, outError);
            if (!v) {
                OCRelease(k);
                OCRelease(dict);
                return NULL;
            }
        } else if (strcmp(type->valuestring, "array") == 0 && cJSON_IsArray(value)) {
            OCMutableArrayRef arr = OCArrayCreateMutable(cJSON_GetArraySize(value), &kOCTypeArrayCallBacks);
            cJSON *elem = NULL;
            cJSON_ArrayForEach(elem, value) {
                cJSON *wrap = cJSON_CreateObject();
                cJSON_AddItemToObject(wrap, "elem", cJSON_Duplicate(elem, 1));
                OCDictionaryRef tmp = SITypesMetadataCreateFromJSON(wrap, outError);
                cJSON_Delete(wrap);
                if (!tmp) {
                    OCRelease(arr);
                    OCRelease(k);
                    OCRelease(dict);
                    return NULL;
                }
                OCTypeRef item = OCDictionaryGetValue(tmp, STR("elem"));
                if (item) OCArrayAppendValue(arr, item);
                OCRelease(tmp);
            }
            v = (OCTypeRef)arr;
        }
        if (v) {
            OCDictionarySetValue(dict, k, v);
            OCRelease(v);
            OCRelease(k);
        } else {
            // no valid branch matched
            OCRelease(k);
            OCRelease(dict);
            return NULL;
        }
    }
    return dict;
}

static bool siTypesShutdownCalled = false;
void SITypesShutdown(void) {
    if (siTypesShutdownCalled) return;
    siTypesShutdownCalled = true;
    cleanupScalarConstantsLibraries();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForTypeDetailed(SIScalarGetTypeID());
#endif
    SIUnitLibrariesShutdown();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForTypeDetailed(SIUnitGetTypeID());
#endif
    cleanupDimensionalityLibraries();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForTypeDetailed(SIDimensionalityGetTypeID());
#endif
    // Clean up the underlying OCTypes layer that SITypes depends on
    OCTypesShutdown();
}
// // Automatically run at program exit or library unload
// __attribute__((destructor(500)))
// static void SITypes_AutoCleanup(void) {
//     SITypesShutdown();
//     OCTypesShutdown();  // if you have a symmetric OCTypes layer
// }
