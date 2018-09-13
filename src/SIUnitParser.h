//
//  SIUnitParser.h
//
//  Created by PhySy Ltd on 5/3/13.
//  Copyright (c) 2012-2014 PhySy Ltd. All rights reserved.
//

#ifndef SIUnitParser_h
#define SIUnitParser_h

extern OCStringRef unitError;

/*
 @function SIUnitForUnderivedSymbol
 @abstract Returns the unit with an underived symbol, if known.
 @param input The symbol.
 @param error a OCStringRef.
 @result the unit or NULL if unit with symbol is not found.
 */
SIUnitRef SIUnitForParsedSymbol(OCStringRef string, double *unit_multiplier, OCStringRef *error);

#endif
