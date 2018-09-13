//
//  SIDimensionality.h
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef SIDimensionality_h
#define SIDimensionality_h


/*!
 @typedef SIDimensionalityRef
 This is the type of a reference to immutable SIDimensionality.
 A NULL reference represents a dimensionless and underived dimensionality.
 */
typedef const struct __SIDimensionality * SIDimensionalityRef;

/*!
 @enum SIBaseDimensionIndex
 @constant kSILengthIndex index for length dimension.
 @constant kSIMassIndex index for mass dimension.
 @constant kSITimeIndex index for time dimension.
 @constant kSICurrentIndex index for current dimension.
 @constant kSITemperatureIndex index for temperature dimension.
 @constant kSIAmountIndex index for amount dimension.
 @constant kSILuminousIntensityIndex index for luminous intensity dimension.
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




/*! @constant kPSQuantityDimensionless */
#define kPSQuantityDimensionless                        STR("dimensionless")


/*! @constant kPSQuantityLength */
#define kPSQuantityLength                               STR("length")
/*! @constant kPSQuantityInverseLength */
#define kPSQuantityInverseLength                        STR("inverse length")
/*! @constant kPSQuantityWavenumber */
#define kPSQuantityWavenumber                           STR("wavenumber")
/*! @constant kPSQuantityLengthRatio */
#define kPSQuantityLengthRatio                          STR("length ratio")
/*! @constant kPSQuantityPlaneAngle */
#define kPSQuantityPlaneAngle                           STR("plane angle")


/*! @constant kPSQuantityMass */
#define kPSQuantityMass                                 STR("mass")
/*! @constant kPSQuantityInverseMass */
#define kPSQuantityInverseMass                          STR("inverse mass")
/*! @constant kPSQuantityMassRatio */
#define kPSQuantityMassRatio                            STR("mass ratio")


/*! @constant kPSQuantityTime */
#define kPSQuantityTime                                 STR("time")
/*! @constant kPSQuantityInverseTime */
#define kPSQuantityInverseTime                          STR("inverse time")
/*! @constant kPSQuantityFrequency */
#define kPSQuantityFrequency                            STR("frequency")
/*! @constant kPSQuantityRadioactivity */
#define kPSQuantityRadioactivity                        STR("radioactivity")
/*! @constant kPSQuantityTimeRatio */
#define kPSQuantityTimeRatio                            STR("time ratio")
/*! @constant kPSQuantityFrequencyRatio */
#define kPSQuantityFrequencyRatio                       STR("frequency ratio")

/*! @constant kPSQuantityInverseTimeSquared */
#define kPSQuantityInverseTimeSquared                   STR("inverse time squared")


/*! @constant kPSQuantityCurrent */
#define kPSQuantityCurrent                              STR("current")
/*! @constant kPSQuantityInverseCurrent */
#define kPSQuantityInverseCurrent                       STR("inverse current")
/*! @constant kPSQuantityCurrentRatio */
#define kPSQuantityCurrentRatio                         STR("current ratio")


/*! @constant kPSQuantityTemperature */
#define kPSQuantityTemperature                          STR("temperature")
/*! @constant kPSQuantityInverseTemperature */
#define kPSQuantityInverseTemperature                   STR("inverse temperature")
/*! @constant kPSQuantityTemperatureRatio */
#define kPSQuantityTemperatureRatio                     STR("temperature ratio")

/*! @constant kPSQuantityTemperatureGradient */
#define kPSQuantityTemperatureGradient                  STR("temperature gradient")


/*! @constant kPSQuantityAmount */
#define kPSQuantityAmount                               STR("amount")
/*! @constant kPSQuantityInverseAmount */
#define kPSQuantityInverseAmount                        STR("inverse amount")
/*! @constant kPSQuantityAmountRatio */
#define kPSQuantityAmountRatio                          STR("amount ratio")


/*! @constant kPSQuantityLuminousIntensity */
#define kPSQuantityLuminousIntensity                    STR("luminous intensity")
/*! @constant kPSQuantityInverseLuminousIntensity */
#define kPSQuantityInverseLuminousIntensity             STR("inverse luminous intensity")
/*! @constant kPSQuantityLuminousIntensityRatio */
#define kPSQuantityLuminousIntensityRatio               STR("luminous intensity ratio")


/*! @constant kPSQuantityArea */
#define kPSQuantityArea                                 STR("area")
/*! @constant kPSQuantityInverseArea */
#define kPSQuantityInverseArea                          STR("inverse area")
/*! @constant kPSQuantityAreaRatio */
#define kPSQuantityAreaRatio                            STR("area ratio")
/*! @constant kPSQuantitySolidAngle */
#define kPSQuantitySolidAngle                           STR("solid angle")


/*! @constant kPSQuantityVolume */
#define kPSQuantityVolume                               STR("volume")
/*! @constant kPSQuantityInverseVolume */
#define kPSQuantityInverseVolume                        STR("inverse volume")
/*! @constant kPSQuantityVolumeRatio */
#define kPSQuantityVolumeRatio                          STR("volume ratio")


/*! @constant kPSQuantitySpeed */
#define kPSQuantitySpeed                                STR("speed")
/*! @constant kPSQuantityVelocity */
#define kPSQuantityVelocity                             STR("velocity")

/*! @constant kPSQuantityLinearMomentum */
#define kPSQuantityLinearMomentum                       STR("linear momentum")

/*! @constant kPSQuantityAngularMomentum */
#define kPSQuantityAngularMomentum                      STR("angular momentum")

/*! @constant kPSQuantityMomentOfInertia */
#define kPSQuantityMomentOfInertia                      STR("moment of inertia")

/*! @constant kPSQuantityAcceleration */
#define kPSQuantityAcceleration                         STR("acceleration")

/*! @constant kPSQuantityMassFlowRate */
#define kPSQuantityMassFlowRate                         STR("mass flow rate")

/*! @constant kPSQuantityMassFlux */
#define kPSQuantityMassFlux                             STR("mass flux")

/*! @constant kPSQuantityDensity */
#define kPSQuantityDensity                              STR("density")

/*! @constant kPSQuantitySpecificGravity */
#define kPSQuantitySpecificGravity                      STR("specific gravity")

/*! @constant kPSQuantitySpecificSurfaceArea */
#define kPSQuantitySpecificSurfaceArea                  STR("specific surface area")

/*! @constant kPSQuantitySurfaceAreaToVolumeRatio */
#define kPSQuantitySurfaceAreaToVolumeRatio             STR("surface area to volume ratio")

/*! @constant kPSQuantitySurfaceDensity */
#define kPSQuantitySurfaceDensity                       STR("surface density")

/*! @constant kPSQuantitySpecificVolume */
#define kPSQuantitySpecificVolume                       STR("specific volume")

/*! @constant kPSQuantityCurrentDensity */
#define kPSQuantityCurrentDensity                       STR("current density")

/*! @constant kPSQuantityMagneticFieldStrength */
#define kPSQuantityMagneticFieldStrength                STR("magnetic field strength")

/*! @constant kPSQuantityLuminance */
#define kPSQuantityLuminance                            STR("luminance")

/*! @constant kPSQuantityRefractiveIndex */
#define kPSQuantityRefractiveIndex                      STR("refractive index")

/*! @constant kPSQuantityFluidity */
#define kPSQuantityFluidity                             STR("fluidity")

/*! @constant kPSQuantityMomentOfForce */
#define kPSQuantityMomentOfForce                        STR("moment of force")

/*! @constant kPSQuantitySurfaceTension */
#define kPSQuantitySurfaceTension                       STR("surface tension")

/*! @constant kPSQuantitySurfaceEnergy */
#define kPSQuantitySurfaceEnergy                        STR("surface energy")

/*! @constant kPSQuantityAngularSpeed */
#define kPSQuantityAngularSpeed                         STR("angular speed")

/*! @constant kPSQuantityAngularVelocity */
#define kPSQuantityAngularVelocity                      STR("angular velocity")

/*! @constant kPSQuantityAngularAcceleration */
#define kPSQuantityAngularAcceleration                  STR("angular acceleration")

/*! @constant kPSQuantityHeatFluxDensity */
#define kPSQuantityHeatFluxDensity                      STR("heat flux density")

/*! @constant kPSQuantityIrradiance */
#define kPSQuantityIrradiance                           STR("irradiance")

/*! @constant kPSQuantitySpectralRadiantFluxDensity */
#define kPSQuantitySpectralRadiantFluxDensity           STR("spectral radiant flux density")

/*! @constant kPSQuantityHeatCapacity */
#define kPSQuantityHeatCapacity                         STR("heat capacity")
/*! @constant kPSQuantityEntropy */
#define kPSQuantityEntropy                              STR("entropy")

/*! @constant kPSQuantitySpecificHeatCapacity */
#define kPSQuantitySpecificHeatCapacity                 STR("specific heat capacity")
/*! @constant kPSQuantitySpecificEntropy */
#define kPSQuantitySpecificEntropy                      STR("specific entropy")

/*! @constant kPSQuantitySpecificEnergy */
#define kPSQuantitySpecificEnergy                       STR("specific energy")

/*! @constant kPSQuantityThermalConductance */
#define kPSQuantityThermalConductance                  STR("thermal conductance")

/*! @constant kPSQuantityThermalConductivity */
#define kPSQuantityThermalConductivity                  STR("thermal conductivity")

/*! @constant kPSQuantityEnergyDensity */
#define kPSQuantityEnergyDensity                        STR("energy density")

/*! @constant kPSQuantityElectricFieldStrength */
#define kPSQuantityElectricFieldStrength                STR("electric field strength")

/*! @constant kPSQuantityElectricFieldGradient */
#define kPSQuantityElectricFieldGradient                STR("electric field gradient")

/*! @constant kPSQuantityElectricChargeDensity */
#define kPSQuantityElectricChargeDensity                STR("electric charge density")

/*! @constant kPSQuantitySurfaceChargeDensity */
#define kPSQuantitySurfaceChargeDensity                 STR("surface charge density")

/*! @constant kPSQuantityElectricFlux */
#define kPSQuantityElectricFlux                         STR("electric flux")

/*! @constant kPSQuantityElectricFluxDensity */
#define kPSQuantityElectricFluxDensity                  STR("electric flux density")

/*! @constant kPSQuantityElectricDisplacement */
#define kPSQuantityElectricDisplacement                 STR("electric displacement")

/*! @constant kPSQuantityPermittivity */
#define kPSQuantityPermittivity                         STR("permittivity")

/*! @constant kPSQuantityPermeability */
#define kPSQuantityPermeability                         STR("permeability")

/*! @constant kPSQuantityMolarEnergy */
#define kPSQuantityMolarEnergy                          STR("molar energy")

/*! @constant kPSQuantityMolarEntropy */
#define kPSQuantityMolarEntropy                         STR("molar entropy")

/*! @constant kPSQuantityMolarHeatCapacity */
#define kPSQuantityMolarHeatCapacity                    STR("molar heat capacity")

/*! @constant kPSQuantityMolarMass */
#define kPSQuantityMolarMass                            STR("molar mass")

/*! @constant kPSQuantityMolality */
#define kPSQuantityMolality                             STR("molality")

/*! @constant kPSQuantityDiffusionFlux */
#define kPSQuantityDiffusionFlux                        STR("diffusion flux")

/*! @constant kPSQuantityMassToChargeRatio */
#define kPSQuantityMassToChargeRatio                    STR("mass to charge ratio")

/*! @constant kPSQuantityChargeToMassRatio */
#define kPSQuantityChargeToMassRatio                    STR("charge to mass ratio")

/*! @constant kPSQuantityRadiationExposure */
#define kPSQuantityRadiationExposure                    STR("radiation exposure")

/*! @constant kPSQuantityAbsorbedDoseRate */
#define kPSQuantityAbsorbedDoseRate                     STR("absorbed dose rate")

/*! @constant kPSQuantityRadiantIntensity */
#define kPSQuantityRadiantIntensity                     STR("radiant intensity")

/*! @constant kPSQuantitySpectralRadiantIntensity */
#define kPSQuantitySpectralRadiantIntensity             STR("spectral radiant intensity")

/*! @constant kPSQuantityRadiance */
#define kPSQuantityRadiance                             STR("radiance")

/*! @constant kPSQuantitySpectralRadiance */
#define kPSQuantitySpectralRadiance                     STR("spectral radiance")

/*! @constant kPSQuantityPorosity */
#define kPSQuantityPorosity                             STR("porosity")

/*! @constant kPSQuantityAngularFrequency */
#define kPSQuantityAngularFrequency                     STR("angular frequency")

/*! @constant kPSQuantityForce */
#define kPSQuantityForce                                STR("force")

/*! @constant kPSQuantityTorque */
#define kPSQuantityTorque                               STR("torque")

/*! @constant kPSQuantityPressure */
#define kPSQuantityPressure                             STR("pressure")
/*! @constant kPSQuantityStress */
#define kPSQuantityStress                               STR("stress")
/*! @constant kPSQuantityElasticModulus */
#define kPSQuantityElasticModulus                       STR("elastic modulus")

/*! @constant kPSQuantityCompressibility */
#define kPSQuantityCompressibility                      STR("compressibility")
/*! @constant kPSQuantityStressOpticCoefficient */
#define kPSQuantityStressOpticCoefficient               STR("stress-optic coefficient")

/*! @constant kPSQuantityPressureGradient */
#define kPSQuantityPressureGradient                     STR("pressure gradient")

/*! @constant kPSQuantityEnergy */
#define kPSQuantityEnergy                               STR("energy")

/*! @constant kPSQuantitySpectralRadiantEnergy */
#define kPSQuantitySpectralRadiantEnergy                STR("spectral radiant energy")

/*! @constant kPSQuantityPower */
#define kPSQuantityPower                                STR("power")

/*! @constant kPSQuantitySpectralPower */
#define kPSQuantitySpectralPower                        STR("spectral power")

/*! @constant kPSQuantityVolumePowerDensity */
#define kPSQuantityVolumePowerDensity                   STR("volume power density")

/*! @constant kPSQuantitySpecificPower */
#define kPSQuantitySpecificPower                        STR("specific power")

/*! @constant kPSQuantityRadiantFlux */
#define kPSQuantityRadiantFlux                          STR("radiant flux")

/*! @constant kPSQuantityElectricCharge */
#define kPSQuantityElectricCharge                       STR("electric charge")

/*! @constant kPSQuantityAmountOfElectricity */
#define kPSQuantityAmountOfElectricity                  STR("amount of electricity")

/*! @constant kPSQuantityElectricPotentialDifference */
#define kPSQuantityElectricPotentialDifference          STR("electric potential difference")

/*! @constant kPSQuantityElectromotiveForce */
#define kPSQuantityElectromotiveForce                   STR("electromotive force")

/*! @constant kPSQuantityElectricPolarizability */
#define kPSQuantityElectricPolarizability                 STR("electric polarizability")

/*! @constant kPSQuantityElectricDipoleMoment */
#define kPSQuantityElectricDipoleMoment                 STR("electric dipole moment")

/*! @constant kPSQuantityVoltage */
#define kPSQuantityVoltage                              STR("voltage")

/*! @constant kPSQuantityCapacitance */
#define kPSQuantityCapacitance                          STR("capacitance")

/*! @constant kPSQuantityElectricResistance */
#define kPSQuantityElectricResistance                   STR("electric resistance")

/*! @constant kPSQuantityElectricResistancePerLength */
#define kPSQuantityElectricResistancePerLength          STR("electric resistance per length")

/*! @constant kPSQuantityElectricResistivity */
#define kPSQuantityElectricResistivity                  STR("electric resistivity")

/*! @constant kPSQuantityElectricConductance */
#define kPSQuantityElectricConductance                  STR("electric conductance")

/*! @constant kPSQuantityElectricConductivity */
#define kPSQuantityElectricConductivity                 STR("electric conductivity")

/*! @constant kPSQuantityElectricalMobility */
#define kPSQuantityElectricalMobility                   STR("electrical mobility")

/*! @constant kPSQuantityMolarConductivity */
#define kPSQuantityMolarConductivity                    STR("molar conductivity")

/*! @constant kPSQuantityMagneticDipoleMoment */
#define kPSQuantityMagneticDipoleMoment                 STR("magnetic dipole moment")

/*! @constant kPSQuantityMagneticDipoleMomentRatio */
#define kPSQuantityMagneticDipoleMomentRatio            STR("magnetic dipole moment ratio")

/*! @constant kPSQuantityMagneticFlux */
#define kPSQuantityMagneticFlux                         STR("magnetic flux")

/*! @constant kPSQuantityMagneticFluxDensity */
#define kPSQuantityMagneticFluxDensity                  STR("magnetic flux density")

/*! @constant kPSQuantityMolarMagneticSusceptibility */
#define kPSQuantityMolarMagneticSusceptibility          STR("molar magnetic susceptibility")

/*! @constant kPSQuantityInverseMagneticFluxDensity */
#define kPSQuantityInverseMagneticFluxDensity           STR("inverse magnetic flux density")

/*! @constant kPSQuantityMagneticFieldGradient */
#define kPSQuantityMagneticFieldGradient                STR("magnetic field gradient")

/*! @constant kPSQuantityInductance */
#define kPSQuantityInductance                           STR("inductance")

/*! @constant kPSQuantityLuminousFlux */
#define kPSQuantityLuminousFlux                         STR("luminous flux")

/*! @constant kPSQuantityLuminousFluxDensity */
#define kPSQuantityLuminousFluxDensity                  STR("luminous flux density")

/*! @constant kPSQuantityLuminousEnergy */
#define kPSQuantityLuminousEnergy                       STR("luminous energy")

/*! @constant kPSQuantityIlluminance */
#define kPSQuantityIlluminance                          STR("illuminance")

/*! @constant kPSQuantityAbsorbedDose */
#define kPSQuantityAbsorbedDose                         STR("absorbed dose")

/*! @constant kPSQuantityDoseEquivalent */
#define kPSQuantityDoseEquivalent                       STR("dose equivalent")

/*! @constant kPSQuantityCatalyticActivity */
#define kPSQuantityCatalyticActivity                    STR("catalytic activity")

/*! @constant kPSQuantityCatalyticActivityConcentration */
#define kPSQuantityCatalyticActivityConcentration       STR("catalytic activity concentration")

/*! @constant kPSQuantityCatalyticActivityContent */
#define kPSQuantityCatalyticActivityContent             STR("catalytic activity content")

/*! @constant kPSQuantityAction */
#define kPSQuantityAction                               STR("action")

/*! @constant kPSQuantityReducedAction */
#define kPSQuantityReducedAction                        STR("reduced action")

/*! @constant kPSQuantityKinematicViscosity */
#define kPSQuantityKinematicViscosity                   STR("kinematic viscosity")

/*! @constant kPSQuantityDiffusionCoefficient */
#define kPSQuantityDiffusionCoefficient                 STR("diffusion coefficient")

/*! @constant kPSQuantityCirculation */
#define kPSQuantityCirculation                          STR("circulation")

/*! @constant kPSQuantityDynamicViscosity */
#define kPSQuantityDynamicViscosity                     STR("dynamic viscosity")

/*! @constant kPSQuantityAmountConcentration */
#define kPSQuantityAmountConcentration                  STR("amount concentration")

/*! @constant kPSQuantityMassConcentration */
#define kPSQuantityMassConcentration                    STR("mass concentration")

/*! @constant kPSQuantityChargePerAmount */
#define kPSQuantityChargePerAmount                      STR("charge per amount")

/*! @constant kPSQuantityGravitationalConstant */
#define kPSQuantityGravitationalConstant                STR("gravitational constant")

/*! @constant kPSQuantityLengthPerVolume */
#define kPSQuantityLengthPerVolume                      STR("distance per volume")

/*! @constant kPSQuantityVolumePerLength */
#define kPSQuantityVolumePerLength                      STR("volume per length")

/*! @constant kPSQuantityVolumetricFlowRate */
#define kPSQuantityVolumetricFlowRate                   STR("volumetric flow rate")

/*! @constant kPSQuantityFrequencyPerMagneticFluxDensity */
#define kPSQuantityFrequencyPerMagneticFluxDensity      STR("frequency per magnetic flux density")

/*! @constant kPSQuantityPowerPerLuminousFlux */
#define kPSQuantityPowerPerLuminousFlux                 STR("power per luminous flux")

/*! @constant kPSQuantityLuminousEfficacy */
#define kPSQuantityLuminousEfficacy                     STR("luminous efficacy")

/*! @constant kPSQuantityRockPermeability */
#define kPSQuantityRockPermeability                     STR("rock permeability")

/*! @constant kPSQuantityGyromagneticRatio */
#define kPSQuantityGyromagneticRatio                    STR("gyromagnetic ratio")

/*! @constant kPSQuantityHeatTransferCoefficient */
#define kPSQuantityHeatTransferCoefficient              STR("heat transfer coefficient")

/*! @constant kPSQuantityGasPermeance */
#define kPSQuantityGasPermeance                         STR("gas permeance")

/*! @constant kPSQuantityPowerPerAreaPerTemperatureToFourthPower */
#define kPSQuantityPowerPerAreaPerTemperatureToFourthPower        STR("stefan-boltzmann constant")

/*! @constant kPSQuantityFirstHyperPolarizability */
#define kPSQuantityFirstHyperPolarizability                 STR("first hyperpolarizability")

/*! @constant kPSQuantitySecondHyperPolarizability */
#define kPSQuantitySecondHyperPolarizability                STR("second hyperpolarizability")

/*! @constant kPSQuantityElectricQuadrupoleMoment */
#define kPSQuantityElectricQuadrupoleMoment                 STR("electric quadrupole moment")

/*! @constant kPSQuantityMagnetizability */
#define kPSQuantityMagnetizability                          STR("magnetizability")

/*! @constant kPSQuantitySecondRadiationConstant */
#define kPSQuantitySecondRadiationConstant                  STR("second radiation constant")

/*! @constant kPSQuantityWavelengthDisplacementConstant */
#define kPSQuantityWavelengthDisplacementConstant           STR("wavelength displacement constant")

/*! @constant kPSQuantityFineStructureConstant */
#define kPSQuantityFineStructureConstant                    STR("fine structure constant")

/*! @constant kPSQuantityRatePerAmountConcentrationPerTime */
#define kPSQuantityRatePerAmountConcentrationPerTime        STR("inverse amount concentration inverse time")


/*!
 @function SIDimensionalityForSymbol
 @abstract Parses the string and returns the dimensionality for the symbol
 @param theString the string with the dimensionality symbol.
 @param error a pointer to a CFError.
 @result the dimensionality.
 @discussion symbols for the seven base dimensions, length, mass, time, current, temperature, amount, and luminous intensity are L, M, T, I, ϴ, N, and J, respectively.
 The input symbol can be in the general form
 
 L^l * M^m * T^t * I^i * ϴ^q * N^n * J^j / (L^l' * M^m' * T^t' * I^i' * ϴ^q' *• N^n' • J^j'),
 
 where the lower case exponents are replaced with integer values and any combination of symbols in the numerator or denominator can be omitted.  This method is intelligent enough to handle any valid combination of the base dimension symbols multiplied, divided, and raised to arbitrary signed integer powers.   The symbol for temperature, "ϴ", does not exist as a valid ascii character, so, if needed, the symbol "@" can be substituted for "ϴ" in this parser.
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

/*!
 @header SIDimensionality
 SIDimensionality represents the dimensionality of a physical quantity. Seven
 physical quantities serve as fundamental reference quantities from which
 all other physical quantities can be derived.  These reference quantities are
 (1) length,
 (2) mass,
 (3) time,
 (4) electric current,
 (5) thermodynamic temperature (the absolute measure of temperature)
 (6) amount of substance,
 (7) luminous intensity.
 
 @copyright PhySy
 */

/*!  @unsorted */


#pragma mark Accessors
/*!
 @functiongroup Accessors
 */

OCTypeID SIDimensionalityGetTypeID(void);

/*!
 @function SIDimensionalityGetSymbol
 @abstract Returns the symbol for the dimensionality.
 @param theDimensionality the dimensionality.
 @result a string containing the symbol.
 */
OCStringRef SIDimensionalityGetSymbol(SIDimensionalityRef theDimensionality);

/*
 @function SIDimensionalityGetNumeratorExponentAtIndex
 @abstract Gets the numerator exponent for the dimension at index.
 @param thedimensionality the dimensionality.
 @param index the dimension index constant.
 @result the integer numerator exponent.
 @discussion base units length, mass, time, current, temperature, amount, and luminous intensity are
 assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex,  kSITemperatureIndex,
 kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
uint8_t SIDimensionalityGetNumeratorExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index);

/*
 @function SIDimensionalityGetDenominatorExponentAtIndex
 @abstract Gets the denominator exponent for the dimension at index.
 @param theDimensionality the dimensionality.
 @result the integer denominator exponent.
 @discussion base units length, mass, time, current, temperature, amount, and luminous intensity are
 assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex,  kSITemperatureIndex,
 kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
uint8_t SIDimensionalityGetDenominatorExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index);

/*
 @function SIDimensionalityReducedExponentAtIndex
 @abstract Returns the exponent for the dimension at Index.
 @param theDimensionality the dimensionality.
 @result the integer exponent (numerator-denominator).
 @discussion base units length, mass, time, current, temperature, amount, and luminous intensity are
 assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex,  kSITemperatureIndex,
 kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
int8_t SIDimensionalityReducedExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index);


#pragma mark Tests
/*!
 @functiongroup Tests
 */

/*!
 @function SIDimensionalityEqual
 @abstract Determines if the two dimensionalities are equal.
 @param theDimensionality1 The first dimensionality.
 @param theDimensionality2 The second dimensionality.
 @result true or false.
 */
bool SIDimensionalityEqual(SIDimensionalityRef theDimensionality1,SIDimensionalityRef theDimensionality2);

/*!
 @function SIDimensionalityIsDimensionless
 @abstract Determines if the dimensionality is dimensionless.
 @param theDimensionality the dimensionality.
 @result true or false.
 */
bool SIDimensionalityIsDimensionless(SIDimensionalityRef theDimensionality);

/*
 @function SIDimensionalityIsDerived
 @abstract Determines if the dimensionality is derived from at least one of seven base dimensions.
 @param theDimensionality the dimensionality.
 @result true or false.
 */
bool SIDimensionalityIsDerived(SIDimensionalityRef theDimensionality);

/*!
 @function SIDimensionalityIsDimensionlessAndNotDerived
 @abstract Determines if the dimensionality is dimensionless but not derived.
 @param theDimensionality The dimensionality.
 @result true or false.
 @discussion Determines if the Dimensionality is dimensionless but not derived, that is,
 it may be a counting Dimensionality.
 */
bool SIDimensionalityIsDimensionlessAndNotDerived(SIDimensionalityRef theDimensionality);

/*!
 @function SIDimensionalityIsDimensionlessAndDerived
 @abstract Determines if the dimensionality is dimensionless and derived.
 @param theDimensionality The dimensionality.
 @result true or false.
 */
bool SIDimensionalityIsDimensionlessAndDerived(SIDimensionalityRef theDimensionality);

/*!
 @function SIDimensionalityIsBaseDimensionality
 @abstract Determines if the dimensionality is one of the seven base dimensionalities.
 @param theDimensionality The dimensionality.
 @result true or false.
 */
bool SIDimensionalityIsBaseDimensionality(SIDimensionalityRef theDimensionality);

/*!
 @function SIDimensionalityHasSameReducedDimensionality
 @abstract Determines if the two dimensionalities have the same reduced dimensionality,
 @param theDimensionality1 The first dimensionality.
 @param theDimensionality2 The second dimensionality.
 @result true or false.
 */
bool SIDimensionalityHasSameReducedDimensionality(SIDimensionalityRef theDimensionality1,SIDimensionalityRef theDimensionality2);

/*
 @function SIDimensionalityHasReducedExponents
 @abstract Determines if the dimensionality has the same seven base dimension exponents,
 @param theDimensionality the dimensionality.
 @param length_exponent The length exponent.
 @param mass_exponent The mass exponent.
 @param time_exponent The time exponent.
 @param current_exponent The current exponent.
 @param temperature_exponent The temperature exponent.
 @param amount_exponent The amount exponent.
 @param luminous_intensity_exponent The luminous intensity exponent.
 @result true or false.
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
/*!
 @functiongroup Operations
 */

/*
 @function SIDimensionalityDimensionless
 @abstract Returns the dimensionality where all exponents are zero.
 @result theDimensionality the dimensionality.
 */
SIDimensionalityRef SIDimensionalityDimensionless(void);

/*
 @function SIDimensionalityForBaseDimensionIndex
 @abstract Returns the dimensionality associated with the base dimension index .
 @result theDimensionality the dimensionality.
 */
SIDimensionalityRef SIDimensionalityForBaseDimensionIndex(SIBaseDimensionIndex index);

/*
 @function SIDimensionalityWithBaseDimensionSymbol
 @abstract Returns the dimensionality associated with the base dimension symbol.  Base dimemension symbols are L, M, T, I, ϴ, N, and J.  The symbol for temperature, "ϴ", does not exist as a valid ascii character, so, if needed, the symbol "@" can be substituted for "ϴ" in this method.
 @param error a pointer to a CFError.
 @result theDimensionality the dimensionality.
 */
SIDimensionalityRef SIDimensionalityWithBaseDimensionSymbol(OCStringRef theString, OCStringRef *error);

/*!
 @function SIDimensionalityForSymbol
 @abstract Parses the string and returns the dimensionality for the symbol
 @param theString the string with the dimensionality symbol.
 @param error a pointer to a CFError.
 @result the dimensionality.
 @discussion symbols for the seven base dimensions, length, mass, time, current, temperature, amount, and luminous intensity are L, M, T, I, ϴ, N, and J, respectively.
 The input symbol can be in the general form
 
 L^l * M^m * T^t * I^i * ϴ^q * N^n * J^j / (L^l' * M^m' * T^t' * I^i' * ϴ^q' *• N^n' • J^j'),
 
 where the lower case exponents are replaced with integer values and any combination of symbols in the numerator or denominator can be omitted.  This method is intelligent enough to handle any valid combination of the base dimension symbols multiplied, divided, and raised to arbitrary signed integer powers.   The symbol for temperature, "ϴ", does not exist as a valid ascii character, so, if needed, the symbol "@" can be substituted for "ϴ" in this parser.
 */
SIDimensionalityRef SIDimensionalityForSymbol(OCStringRef theString, OCStringRef *error);

/*!
 @function SIDimensionalityForQuantity
 @abstract Returns the dimensionality for the quantity
 @param quantity The quantity.
 @result the dimensionality.
 */
SIDimensionalityRef SIDimensionalityForQuantity(OCStringRef quantity, OCStringRef *error);

/*!
 @function SIDimensionalityByReducing
 @abstract Returns the dimensionality by reducing the numerator and denominator exponents to their lowest values.
 @param theDimensionality the dimensionality.
 @result the dimensionality with reduced numerator and denominator exponents
 */
SIDimensionalityRef SIDimensionalityByReducing(SIDimensionalityRef theDimensionality);

/*!
 @function SIDimensionalityByTakingNthRoot
 @abstract Returns the dimensionality by dividing the numerator and denominator exponents by an integer.
 @param theDimensionality the dimensionality.
 @param root the integer root.
 @param error an OCString describing the error.
 @result the nth root dimensionality
 @discussion The numerator and denominator exponents in a valid dimensionality can only take on integer values.
 If this function cannot return a valid dimensionality then it will return NULL.
 */
SIDimensionalityRef SIDimensionalityByTakingNthRoot(SIDimensionalityRef theDimensionality, uint8_t root, OCStringRef *error);

/*!
 @function SIDimensionalityByMultiplying
 @abstract Returns the dimensionality after multiplying two dimensionalities and reducing the dimensionality numerator and denominator exponents to their lowest integer values
 @param theDimensionality1 The first dimensionality.
 @param theDimensionality2 The second dimensionality.
 @param error an OCString describing the error.
 @result the new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByMultiplying(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2, OCStringRef *error);

/*!
 @function SIDimensionalityByMultiplyingWithoutReducing
 @abstract Returns the dimensionality after multiplying two dimensionalities
 @param theDimensionality1 The first dimensionality.
 @param theDimensionality2 The second dimensionality.
 @param error an OCString describing the error.
 @result the new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByMultiplyingWithoutReducing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2, OCStringRef *error);

/*!
 @function SIDimensionalityByDividing
 @abstract Returns the dimensionality after dividing theDimensionality1 by theDimensionality2 and reducing the dimensionality numerator and denominator exponents to their lowest integer values
 @param theDimensionality1 The first dimensionality.
 @param theDimensionality2 The second dimensionality.
 @result the new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByDividing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2);

/*!
 @function SIDimensionalityByDividingWithoutReducing
 @abstract Returns the dimensionality after dividing theDimensionality1 by theDimensionality2
 @param theDimensionality1 The first dimensionality.
 @param theDimensionality2 The second dimensionality.
 @result the new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByDividingWithoutReducing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2);

/*!
 @function SIDimensionalityByRaisingToAPower
 @abstract Returns the dimensionality after raising a dimensionality to a power and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 @param theDimensionality the dimensionality.
 @result the new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByRaisingToAPower(SIDimensionalityRef theDimensionality, double power, OCStringRef *error);

/*!
 @function SIDimensionalityByRaisingToAPowerWithoutReducing
 @abstract Returns the dimensionality after by raising a dimensionality to a power.
 @param theDimensionality the dimensionality.
 @param power the power.
 @param error a pointer to a CFError.
 @result the new dimensionality.
 */
SIDimensionalityRef SIDimensionalityByRaisingToAPowerWithoutReducing(SIDimensionalityRef theDimensionality, double power, OCStringRef *error);

/*!
 @function SIDimensionalityCreateArrayOfQuantities
 @abstract Creates an array of physical quantity names for the dimensionality.
 @param theDimensionality the dimensionality.
 @result a OCArray of strings with all the physical quantity names having this dimensionality.
 */
OCArrayRef SIDimensionalityCreateArrayOfQuantities(SIDimensionalityRef theDimensionality);

/*!
 @function SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality
 @abstract Creates an array of physical quantity names for with the same reduced dimensionality.
 @param theDimensionality the dimensionality.
 @result a OCArray of strings with all the physical quantity names having the same reduced dimensionality.
 */
OCArrayRef SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIDimensionalityRef theDimensionality);

/*!
 @function SIDimensionalityCreateArrayWithSameReducedDimensionality
 @abstract Creates an array of dimenstionalities with the same dimensionality.
 @param theDimensionality the dimensionality.
 @result a OCArray of dimenstionalities with all dimensionalities having the same reduced dimensionality as input.
 @discussion The routine returns all the dimensionalities starting with the largest exponent (numerator or denominator) down to the reduced dimensionality.
 */
OCArrayRef SIDimensionalityCreateArrayWithSameReducedDimensionality(SIDimensionalityRef theDimensionality);

#pragma mark Strings and Archiving
/*!
 @functiongroup Strings and Archiving
 */

/*
 @function SIDimensionalityShow
 @abstract Shows a short descriptor of the dimensionality
 @param theDimensionality the dimensionality.
 */
void SIDimensionalityShow(SIDimensionalityRef theDimensionality);

/*
 @function SIDimensionalityShowFull
 @abstract Shows a long descriptor of the dimensionality
 @param theDimensionality the dimensionality.
 */
void SIDimensionalityShowFull(SIDimensionalityRef theDimensionality);

#pragma mark Library
/*!
 @functiongroup Library
 */

/*!
 @function SIDimensionalityGetLibrary
 @abstract Gets a copy of the library of dimensionalities
 @result a OCMutableDictionary containing the dimensionalities.
 */
OCMutableDictionaryRef SIDimensionalityGetLibrary(void);

/*!
 @function SIDimensionalityLibraryCreateArrayOfAllQuantities
 @abstract Creates a alphabetical sorted array of all quantity names
 */
OCArrayRef SIDimensionalityLibraryCreateArrayOfAllQuantities(void);


void SIDimensionalitySetLibrary(OCMutableDictionaryRef newDimensionalityLibrary);
#endif /* SIDimensionality_h */
