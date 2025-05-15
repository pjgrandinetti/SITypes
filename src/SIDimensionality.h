/**
 * @file SIDimensionality.h
 * @brief Declares the SIDimensionality interface for representing and manipulating physical dimensionalities.
 *
 * SIDimensionality provides immutable objects representing the dimensionality of physical quantities,
 * supporting arithmetic operations, parsing from symbols, and mapping to physical quantity names.
 * The API is CoreFoundation-style, with explicit memory management and reference counting.
 *
 * The seven SI base dimensions are supported: length, mass, time, electric current, temperature,
 * amount of substance, and luminous intensity.
 *
 * @author Philip
 * @copyright PhySy Ltd.
 */

#include <stdbool.h>
#include <stdint.h>
#include "OCTypes/OCLibrary.h"

//
//  SIDimensionality.h
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef SIDimensionality_h
#define SIDimensionality_h

/**
 * @defgroup SIDimensionality SIDimensionality
 * @brief Dimensionality types and operations in SITypes.
 * @{
 */

/**
 * @typedef SIDimensionalityRef
 * @brief This is the type of a reference to immutable SIDimensionality.
 * @details A NULL reference represents a dimensionless and underived dimensionality.
 */
typedef const struct __SIDimensionality * SIDimensionalityRef;

/**
 * @enum SIBaseDimensionIndex
 * @brief Index constants for the seven SI base dimensions.
 * @var kSILengthIndex Index for length dimension.
 * @var kSIMassIndex Index for mass dimension.
 * @var kSITimeIndex Index for time dimension.
 * @var kSICurrentIndex Index for current dimension.
 * @var kSITemperatureIndex Index for temperature dimension.
 * @var kSIAmountIndex Index for amount dimension.
 * @var kSILuminousIntensityIndex Index for luminous intensity dimension.
 */
typedef enum {
    kSILengthIndex = 0,
    kSIMassIndex = 1,
    kSITimeIndex = 2,
    kSICurrentIndex = 3,
    kSITemperatureIndex = 4,
    kSIAmountIndex = 5,
    kSILuminousIntensityIndex = 6,
} SIBaseDimensionIndex;

/**
 * @brief Dimensionality constants for various physical quantities.
 */
#define kSIQuantityDimensionless                        STR("dimensionless")
#define kSIQuantityLength                               STR("length")
#define kSIQuantityInverseLength                        STR("inverse length")
#define kSIQuantityWavenumber                           STR("wavenumber")
#define kSIQuantityLengthRatio                          STR("length ratio")
#define kSIQuantityPlaneAngle                           STR("plane angle")
#define kSIQuantityMass                                 STR("mass")
#define kSIQuantityInverseMass                          STR("inverse mass")
#define kSIQuantityMassRatio                            STR("mass ratio")
#define kSIQuantityTime                                 STR("time")
#define kSIQuantityInverseTime                          STR("inverse time")
#define kSIQuantityFrequency                            STR("frequency")
#define kSIQuantityRadioactivity                        STR("radioactivity")
#define kSIQuantityTimeRatio                            STR("time ratio")
#define kSIQuantityFrequencyRatio                       STR("frequency ratio")
#define kSIQuantityInverseTimeSquared                   STR("inverse time squared")
#define kSIQuantityCurrent                              STR("current")
#define kSIQuantityInverseCurrent                       STR("inverse current")
#define kSIQuantityCurrentRatio                         STR("current ratio")
#define kSIQuantityTemperature                          STR("temperature")
#define kSIQuantityInverseTemperature                   STR("inverse temperature")
#define kSIQuantityTemperatureRatio                     STR("temperature ratio")
#define kSIQuantityTemperatureGradient                  STR("temperature gradient")
#define kSIQuantityAmount                               STR("amount")
#define kSIQuantityInverseAmount                        STR("inverse amount")
#define kSIQuantityAmountRatio                          STR("amount ratio")
#define kSIQuantityLuminousIntensity                    STR("luminous intensity")
#define kSIQuantityInverseLuminousIntensity             STR("inverse luminous intensity")
#define kSIQuantityLuminousIntensityRatio               STR("luminous intensity ratio")
#define kSIQuantityArea                                 STR("area")
#define kSIQuantityInverseArea                          STR("inverse area")
#define kSIQuantityAreaRatio                            STR("area ratio")
#define kSIQuantitySolidAngle                           STR("solid angle")
#define kSIQuantityVolume                               STR("volume")
#define kSIQuantityInverseVolume                        STR("inverse volume")
#define kSIQuantityVolumeRatio                          STR("volume ratio")
#define kSIQuantitySpeed                                STR("speed")
#define kSIQuantityVelocity                             STR("velocity")
#define kSIQuantityLinearMomentum                       STR("linear momentum")
#define kSIQuantityAngularMomentum                      STR("angular momentum")
#define kSIQuantityMomentOfInertia                      STR("moment of inertia")
#define kSIQuantityAcceleration                         STR("acceleration")
#define kSIQuantityMassFlowRate                         STR("mass flow rate")
#define kSIQuantityMassFlux                             STR("mass flux")
#define kSIQuantityDensity                              STR("density")
#define kSIQuantitySpecificGravity                      STR("specific gravity")
#define kSIQuantitySpecificSurfaceArea                  STR("specific surface area")
#define kSIQuantitySurfaceAreaToVolumeRatio             STR("surface area to volume ratio")
#define kSIQuantitySurfaceDensity                       STR("surface density")
#define kSIQuantitySpecificVolume                       STR("specific volume")
#define kSIQuantityCurrentDensity                       STR("current density")
#define kSIQuantityMagneticFieldStrength                STR("magnetic field strength")
#define kSIQuantityLuminance                            STR("luminance")
#define kSIQuantityRefractiveIndex                      STR("refractive index")
#define kSIQuantityFluidity                             STR("fluidity")
#define kSIQuantityMomentOfForce                        STR("moment of force")
#define kSIQuantitySurfaceTension                       STR("surface tension")
#define kSIQuantitySurfaceEnergy                        STR("surface energy")
#define kSIQuantityAngularSpeed                         STR("angular speed")
#define kSIQuantityAngularVelocity                      STR("angular velocity")
#define kSIQuantityAngularAcceleration                  STR("angular acceleration")
#define kSIQuantityHeatFluxDensity                      STR("heat flux density")
#define kSIQuantityIrradiance                           STR("irradiance")
#define kSIQuantitySpectralRadiantFluxDensity           STR("spectral radiant flux density")
#define kSIQuantityHeatCapacity                         STR("heat capacity")
#define kSIQuantityEntropy                              STR("entropy")
#define kSIQuantitySpecificHeatCapacity                 STR("specific heat capacity")
#define kSIQuantitySpecificEntropy                      STR("specific entropy")
#define kSIQuantitySpecificEnergy                       STR("specific energy")
#define kSIQuantityThermalConductance                   STR("thermal conductance")
#define kSIQuantityThermalConductivity                  STR("thermal conductivity")
#define kSIQuantityEnergyDensity                        STR("energy density")
#define kSIQuantityElectricFieldStrength                STR("electric field strength")
#define kSIQuantityElectricFieldGradient                STR("electric field gradient")
#define kSIQuantityElectricChargeDensity                STR("electric charge density")
#define kSIQuantitySurfaceChargeDensity                 STR("surface charge density")
#define kSIQuantityElectricFlux                         STR("electric flux")
#define kSIQuantityElectricFluxDensity                  STR("electric flux density")
#define kSIQuantityElectricDisplacement                 STR("electric displacement")
#define kSIQuantityPermittivity                         STR("permittivity")
#define kSIQuantityPermeability                         STR("permeability")
#define kSIQuantityMolarEnergy                          STR("molar energy")
#define kSIQuantityMolarEntropy                         STR("molar entropy")
#define kSIQuantityMolarHeatCapacity                    STR("molar heat capacity")
#define kSIQuantityMolarMass                            STR("molar mass")
#define kSIQuantityMolality                             STR("molality")
#define kSIQuantityDiffusionFlux                        STR("diffusion flux")
#define kSIQuantityMassToChargeRatio                    STR("mass to charge ratio")
#define kSIQuantityChargeToMassRatio                    STR("charge to mass ratio")
#define kSIQuantityRadiationExposure                    STR("radiation exposure")
#define kSIQuantityAbsorbedDoseRate                     STR("absorbed dose rate")
#define kSIQuantityRadiantIntensity                     STR("radiant intensity")
#define kSIQuantitySpectralRadiantIntensity             STR("spectral radiant intensity")
#define kSIQuantityRadiance                             STR("radiance")
#define kSIQuantitySpectralRadiance                     STR("spectral radiance")
#define kSIQuantityPorosity                             STR("porosity")
#define kSIQuantityAngularFrequency                     STR("angular frequency")
#define kSIQuantityForce                                STR("force")
#define kSIQuantityTorque                               STR("torque")
#define kSIQuantityPressure                             STR("pressure")
#define kSIQuantityStress                               STR("stress")
#define kSIQuantityElasticModulus                       STR("elastic modulus")
#define kSIQuantityCompressibility                      STR("compressibility")
#define kSIQuantityStressOpticCoefficient               STR("stress-optic coefficient")
#define kSIQuantityPressureGradient                     STR("pressure gradient")
#define kSIQuantityEnergy                               STR("energy")
#define kSIQuantitySpectralRadiantEnergy                STR("spectral radiant energy")
#define kSIQuantityPower                                STR("power")
#define kSIQuantitySpectralPower                        STR("spectral power")
#define kSIQuantityVolumePowerDensity                   STR("volume power density")
#define kSIQuantitySpecificPower                        STR("specific power")
#define kSIQuantityRadiantFlux                          STR("radiant flux")
#define kSIQuantityElectricCharge                       STR("electric charge")
#define kSIQuantityAmountOfElectricity                  STR("amount of electricity")
#define kSIQuantityElectricPotentialDifference          STR("electric potential difference")
#define kSIQuantityElectromotiveForce                   STR("electromotive force")
#define kSIQuantityElectricPolarizability               STR("electric polarizability")
#define kSIQuantityElectricDipoleMoment                 STR("electric dipole moment")
#define kSIQuantityVoltage                              STR("voltage")
#define kSIQuantityCapacitance                          STR("capacitance")
#define kSIQuantityElectricResistance                   STR("electric resistance")
#define kSIQuantityElectricResistancePerLength          STR("electric resistance per length")
#define kSIQuantityElectricResistivity                  STR("electric resistivity")
#define kSIQuantityElectricConductance                  STR("electric conductance")
#define kSIQuantityElectricConductivity                 STR("electric conductivity")
#define kSIQuantityElectricalMobility                   STR("electrical mobility")
#define kSIQuantityMolarConductivity                    STR("molar conductivity")
#define kSIQuantityMagneticDipoleMoment                 STR("magnetic dipole moment")
#define kSIQuantityMagneticDipoleMomentRatio            STR("magnetic dipole moment ratio")
#define kSIQuantityMagneticFlux                         STR("magnetic flux")
#define kSIQuantityMagneticFluxDensity                  STR("magnetic flux density")
#define kSIQuantityMolarMagneticSusceptibility          STR("molar magnetic susceptibility")
#define kSIQuantityInverseMagneticFluxDensity           STR("inverse magnetic flux density")
#define kSIQuantityMagneticFieldGradient                STR("magnetic field gradient")
#define kSIQuantityInductance                           STR("inductance")
#define kSIQuantityLuminousFlux                         STR("luminous flux")
#define kSIQuantityLuminousFluxDensity                  STR("luminous flux density")
#define kSIQuantityLuminousEnergy                       STR("luminous energy")
#define kSIQuantityIlluminance                          STR("illuminance")
#define kSIQuantityAbsorbedDose                         STR("absorbed dose")
#define kSIQuantityDoseEquivalent                       STR("dose equivalent")
#define kSIQuantityCatalyticActivity                    STR("catalytic activity")
#define kSIQuantityCatalyticActivityConcentration       STR("catalytic activity concentration")
#define kSIQuantityCatalyticActivityContent             STR("catalytic activity content")
#define kSIQuantityAction                               STR("action")
#define kSIQuantityReducedAction                        STR("reduced action")
#define kSIQuantityKinematicViscosity                   STR("kinematic viscosity")
#define kSIQuantityDiffusionCoefficient                 STR("diffusion coefficient")
#define kSIQuantityCirculation                          STR("circulation")
#define kSIQuantityDynamicViscosity                     STR("dynamic viscosity")
#define kSIQuantityAmountConcentration                  STR("amount concentration")
#define kSIQuantityMassConcentration                    STR("mass concentration")
#define kSIQuantityChargePerAmount                      STR("charge per amount")
#define kSIQuantityGravitationalConstant                STR("gravitational constant")
#define kSIQuantityLengthPerVolume                      STR("distance per volume")
#define kSIQuantityVolumePerLength                      STR("volume per length")
#define kSIQuantityVolumetricFlowRate                   STR("volumetric flow rate")
#define kSIQuantityFrequencyPerMagneticFluxDensity      STR("frequency per magnetic flux density")
#define kSIQuantityPowerPerLuminousFlux                 STR("power per luminous flux")
#define kSIQuantityLuminousEfficacy                     STR("luminous efficacy")
#define kSIQuantityRockPermeability                     STR("rock permeability")
#define kSIQuantityGyromagneticRatio                    STR("gyromagnetic ratio")
#define kSIQuantityHeatTransferCoefficient              STR("heat transfer coefficient")
#define kSIQuantityGasPermeance                         STR("gas permeance")
#define kSIQuantityPowerPerAreaPerTemperatureToFourthPower        STR("stefan-boltzmann constant")
#define kSIQuantityFirstHyperPolarizability             STR("first hyperpolarizability")
#define kSIQuantitySecondHyperPolarizability            STR("second hyperpolarizability")
#define kSIQuantityElectricQuadrupoleMoment             STR("electric quadrupole moment")
#define kSIQuantityMagnetizability                      STR("magnetizability")
#define kSIQuantitySecondRadiationConstant              STR("second radiation constant")
#define kSIQuantityWavelengthDisplacementConstant       STR("wavelength displacement constant")
#define kSIQuantityFineStructureConstant                STR("fine structure constant")
#define kSIQuantityRatePerAmountConcentrationPerTime    STR("inverse amount concentration inverse time")

/**
 * @brief Parses the string and returns the dimensionality for the symbol.
 * @param theString The string with the dimensionality symbol.
 * @param error A pointer to a CFError.
 * @return The dimensionality.
 * @details Symbols for the seven base dimensions, length, mass, time, current, temperature, amount, and luminous intensity are L, M, T, I, ϴ, N, and J, respectively.
 * The input symbol can be in the general form:
 * L^l * M^m * T^t * I^i * ϴ^q * N^n * J^j / (L^l' * M^m' * T^t' * I^i' * ϴ^q' * N^n' * J^j'),
 * where the lower case exponents are replaced with integer values and any combination of symbols in the numerator or denominator can be omitted.
 * This method is intelligent enough to handle any valid combination of the base dimension symbols multiplied, divided, and raised to arbitrary signed integer powers.
 * The symbol for temperature, "ϴ", does not exist as a valid ASCII character, so, if needed, the symbol "@" can be substituted for "ϴ" in this parser.
 */
SIDimensionalityRef SIDimensionalityForSymbol(OCStringRef theString, OCStringRef *error);

extern OCMutableDictionaryRef dimensionalityLibrary;
extern OCMutableDictionaryRef dimensionalityQuantitiesLibrary;

#define IF_UNEQUAL_DIMENSIONALITIES(DIM1,DIM2,RESULT) if(!SIDimensionalityEqual(DIM1,DIM2)) { \
fprintf(stderr, "%s : Unequal dimensionalities:  ",__FUNCTION__); \
OCStringShow(SIDimensionalityGetSymbol(DIM1)); \
fprintf(stderr, " and "); \
OCStringShow(SIDimensionalityGetSymbol(DIM2)); \
fprintf(stderr, "\n"); \
return RESULT;}

#define IF_INCOMPATIBLE_DIMENSIONALITIES(DIM1,DIM2,RESULT) if(!SIDimensionalityHasSameReducedDimensionality(DIM1,DIM2)) { \
fprintf(stderr, "%s : Incompatible dimensionalities:  ",__FUNCTION__); \
OCStringShow(SIDimensionalityGetSymbol(DIM1)); \
fprintf(stderr, " and "); \
OCStringShow(SIDimensionalityGetSymbol(DIM2)); \
fprintf(stderr, "\n"); \
return RESULT;}

/**
 * @brief SIDimensionality represents the dimensionality of a physical quantity.
 * @details Seven physical quantities serve as fundamental reference quantities from which all other physical quantities can be derived. These reference quantities are:
 * - Length
 * - Mass
 * - Time
 * - Electric current
 * - Thermodynamic temperature (the absolute measure of temperature)
 * - Amount of substance
 * - Luminous intensity
 *
 * @copyright PhySy
 */

#pragma mark Accessors
/**
 * @brief Accessor functions for SIDimensionality.
 */

/**
 * @brief Returns the type ID for SIDimensionality.
 * @return The type ID.
 */
OCTypeID SIDimensionalityGetTypeID(void);

/**
 * @brief Returns the symbol for the dimensionality.
 * @param theDimensionality The dimensionality.
 * @return A string containing the symbol.
 */
OCStringRef SIDimensionalityGetSymbol(SIDimensionalityRef theDimensionality);

/**
 * @brief Gets the numerator exponent for the dimension at index.
 * @param theDimensionality The dimensionality.
 * @param index The dimension index constant.
 * @return The integer numerator exponent.
 * @details Base units length, mass, time, current, temperature, amount, and luminous intensity are assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex, kSITemperatureIndex, kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
uint8_t SIDimensionalityGetNumeratorExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index);

/**
 * @brief Gets the denominator exponent for the dimension at index.
 * @param theDimensionality The dimensionality.
 * @param index The dimension index constant.
 * @return The integer denominator exponent.
 * @details Base units length, mass, time, current, temperature, amount, and luminous intensity are assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex, kSITemperatureIndex, kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
uint8_t SIDimensionalityGetDenominatorExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index);

/**
 * @brief Returns the exponent for the dimension at index.
 * @param theDimensionality The dimensionality.
 * @param index The dimension index constant.
 * @return The integer exponent (numerator-denominator).
 * @details Base units length, mass, time, current, temperature, amount, and luminous intensity are assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex, kSITemperatureIndex, kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
int8_t SIDimensionalityReducedExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index);

#pragma mark Tests
/**
 * @brief Test functions for SIDimensionality.
 */

/**
 * @brief Determines if the two dimensionalities are equal.
 * @param theDimensionality1 The first dimensionality.
 * @param theDimensionality2 The second dimensionality.
 * @return True if equal, false otherwise.
 */
bool SIDimensionalityEqual(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2);

/**
 * @brief Determines if the dimensionality is dimensionless.
 * @param theDimensionality The dimensionality.
 * @return True if dimensionless, false otherwise.
 */
bool SIDimensionalityIsDimensionless(SIDimensionalityRef theDimensionality);

/**
 * @brief Determines if the dimensionality is derived from at least one of seven base dimensions.
 * @param theDimensionality The dimensionality.
 * @return True if derived, false otherwise.
 */
bool SIDimensionalityIsDerived(SIDimensionalityRef theDimensionality);

/**
 * @brief Determines if the dimensionality is dimensionless but not derived.
 * @param theDimensionality The dimensionality.
 * @return True if dimensionless and not derived, false otherwise.
 * @details Determines if the dimensionality is dimensionless but not derived, that is, it may be a counting dimensionality.
 */
bool SIDimensionalityIsDimensionlessAndNotDerived(SIDimensionalityRef theDimensionality);

/**
 * @brief Determines if the dimensionality is dimensionless and derived.
 * @param theDimensionality The dimensionality.
 * @return True if dimensionless and derived, false otherwise.
 */
bool SIDimensionalityIsDimensionlessAndDerived(SIDimensionalityRef theDimensionality);

/**
 * @brief Determines if the dimensionality is one of the seven base dimensionalities.
 * @param theDimensionality The dimensionality.
 * @return True if base dimensionality, false otherwise.
 */
bool SIDimensionalityIsBaseDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief Determines if the two dimensionalities have the same reduced dimensionality.
 * @param theDimensionality1 The first dimensionality.
 * @param theDimensionality2 The second dimensionality.
 * @return True if same reduced dimensionality, false otherwise.
 */
bool SIDimensionalityHasSameReducedDimensionality(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2);

/**
 * @brief Determines if the dimensionality has the same seven base dimension exponents.
 * @param theDimensionality The dimensionality.
 * @param length_exponent The length exponent.
 * @param mass_exponent The mass exponent.
 * @param time_exponent The time exponent.
 * @param current_exponent The current exponent.
 * @param temperature_exponent The temperature exponent.
 * @param amount_exponent The amount exponent.
 * @param luminous_intensity_exponent The luminous intensity exponent.
 * @return True if same reduced exponents, false otherwise.
 */
bool SIDimensionalityHasReducedExponents(SIDimensionalityRef theDimensionality,
                                         int8_t length_exponent,
                                         int8_t mass_exponent,
                                         int8_t time_exponent,
                                         int8_t current_exponent,
                                         int8_t temperature_exponent,
                                         int8_t amount_exponent,
                                         int8_t luminous_intensity_exponent);

#pragma mark Operations
/**
 * @brief Operations for SIDimensionality.
 */

/**
 * @brief Returns the dimensionality where all exponents are zero.
 * @return The dimensionless dimensionality.
 */
SIDimensionalityRef SIDimensionalityDimensionless(void);

/**
 * @brief Returns the dimensionality associated with the base dimension index.
 * @param index The base dimension index.
 * @return The dimensionality.
 */
SIDimensionalityRef SIDimensionalityForBaseDimensionIndex(SIBaseDimensionIndex index);

/**
 * @brief Returns the dimensionality associated with the base dimension symbol.
 * @param theString The base dimension symbol.
 * @param error A pointer to a CFError.
 * @return The dimensionality.
 * @details Base dimension symbols are L, M, T, I, ϴ, N, and J. The symbol for temperature, "ϴ", does not exist as a valid ASCII character, so, if needed, the symbol "@" can be substituted for "ϴ" in this method.
 */
SIDimensionalityRef SIDimensionalityWithBaseDimensionSymbol(OCStringRef theString, OCStringRef *error);

/**
 * @brief Parses the string and returns the dimensionality for the symbol.
 * @param theString The string with the dimensionality symbol.
 * @param error A pointer to a CFError.
 * @return The dimensionality.
 * @details Symbols for the seven base dimensions, length, mass, time, current, temperature, amount, and luminous intensity are L, M, T, I, ϴ, N, and J, respectively.
 * The input symbol can be in the general form:
 * L^l * M^m * T^t * I^i * ϴ^q * N^n * J^j / (L^l' * M^m' * T^t' * I^i' * ϴ^q' * N^n' * J^j'),
 * where the lower case exponents are replaced with integer values and any combination of symbols in the numerator or denominator can be omitted.
 * This method is intelligent enough to handle any valid combination of the base dimension symbols multiplied, divided, and raised to arbitrary signed integer powers.
 * The symbol for temperature, "ϴ", does not exist as a valid ASCII character, so, if needed, the symbol "@" can be substituted for "ϴ" in this parser.
 */
SIDimensionalityRef SIDimensionalityForSymbol(OCStringRef theString, OCStringRef *error);

/**
 * @brief Returns the dimensionality for the quantity.
 * @param quantity The quantity.
 * @param error A pointer to a CFError.
 * @return The dimensionality.
 */
SIDimensionalityRef SIDimensionalityForQuantity(OCStringRef quantity, OCStringRef *error);

/**
 * @brief Returns the dimensionality by reducing the numerator and denominator exponents to their lowest values.
 * @param theDimensionality The dimensionality.
 * @return The dimensionality with reduced numerator and denominator exponents.
 */
SIDimensionalityRef SIDimensionalityByReducing(SIDimensionalityRef theDimensionality);

/**
 * @brief Returns the dimensionality by dividing the numerator and denominator exponents by an integer.
 * @param theDimensionality The dimensionality.
 * @param root The integer root.
 * @param error A pointer to a CFError.
 * @return The nth root dimensionality.
 * @details The numerator and denominator exponents in a valid dimensionality can only take on integer values.
 * If this function cannot return a valid dimensionality then it will return NULL.
 */
SIDimensionalityRef SIDimensionalityByTakingNthRoot(SIDimensionalityRef theDimensionality, uint8_t root, OCStringRef *error);

/**
 * @brief Returns the dimensionality after multiplying two dimensionalities and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 * @param theDimensionality1 The first dimensionality.
 * @param theDimensionality2 The second dimensionality.
 * @param error A pointer to a CFError.
 * @return The new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByMultiplying(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2, OCStringRef *error);

/**
 * @brief Returns the dimensionality after multiplying two dimensionalities.
 * @param theDimensionality1 The first dimensionality.
 * @param theDimensionality2 The second dimensionality.
 * @param error A pointer to a CFError.
 * @return The new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByMultiplyingWithoutReducing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2, OCStringRef *error);

/**
 * @brief Returns the dimensionality after dividing theDimensionality1 by theDimensionality2 and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 * @param theDimensionality1 The first dimensionality.
 * @param theDimensionality2 The second dimensionality.
 * @return The new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByDividing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2);

/**
 * @brief Returns the dimensionality after dividing theDimensionality1 by theDimensionality2.
 * @param theDimensionality1 The first dimensionality.
 * @param theDimensionality2 The second dimensionality.
 * @return The new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByDividingWithoutReducing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2);

/**
 * @brief Returns the dimensionality after raising a dimensionality to a power and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 * @param theDimensionality The dimensionality.
 * @param power The power.
 * @param error A pointer to a CFError.
 * @return The new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByRaisingToAPower(SIDimensionalityRef theDimensionality, double power, OCStringRef *error);

/**
 * @brief Returns the dimensionality after raising a dimensionality to a power.
 * @param theDimensionality The dimensionality.
 * @param power The power.
 * @param error A pointer to a CFError.
 * @return The new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByRaisingToAPowerWithoutReducing(SIDimensionalityRef theDimensionality, double power, OCStringRef *error);

/**
 * @brief Creates an array of physical quantity names for the dimensionality.
 * @param theDimensionality The dimensionality.
 * @return An OCArray of strings with all the physical quantity names having this dimensionality.
 */
OCArrayRef SIDimensionalityCreateArrayOfQuantities(SIDimensionalityRef theDimensionality);

/**
 * @brief Creates an array of physical quantity names with the same reduced dimensionality.
 * @param theDimensionality The dimensionality.
 * @return An OCArray of strings with all the physical quantity names having the same reduced dimensionality.
 */
OCArrayRef SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief Creates an array of dimensionalities with the same reduced dimensionality.
 * @param theDimensionality The dimensionality.
 * @return An OCArray of dimensionalities with all dimensionalities having the same reduced dimensionality as input.
 * @details The routine returns all the dimensionalities starting with the largest exponent (numerator or denominator) down to the reduced dimensionality.
 */
OCArrayRef SIDimensionalityCreateArrayWithSameReducedDimensionality(SIDimensionalityRef theDimensionality);

#pragma mark Strings and Archiving
/**
 * @brief String and archiving functions for SIDimensionality.
 */

/**
 * @brief Shows a short descriptor of the dimensionality.
 * @param theDimensionality The dimensionality.
 */
void SIDimensionalityShow(SIDimensionalityRef theDimensionality);

/**
 * @brief Shows a long descriptor of the dimensionality.
 * @param theDimensionality The dimensionality.
 */
void SIDimensionalityShowFull(SIDimensionalityRef theDimensionality);

#pragma mark Library
/**
 * @brief Library functions for SIDimensionality.
 */

/**
 * @brief Gets a copy of the library of dimensionalities.
 * @return An OCMutableDictionary containing the dimensionalities.
 */
OCMutableDictionaryRef SIDimensionalityGetLibrary(void);

/**
 * @brief Creates an alphabetically sorted array of all quantity names.
 * @return An OCArray of all quantity names.
 */
OCArrayRef SIDimensionalityLibraryCreateArrayOfAllQuantities(void);

/**
 * @brief Sets the library of dimensionalities.
 * @param newDimensionalityLibrary The new dimensionality library.
 */
void SIDimensionalitySetLibrary(OCMutableDictionaryRef newDimensionalityLibrary);

#endif /* SIDimensionality_h */
