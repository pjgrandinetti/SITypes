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




/*! @constant kSIQuantityDimensionless */
#define kSIQuantityDimensionless                        STR("dimensionless")


/*! @constant kSIQuantityLength */
#define kSIQuantityLength                               STR("length")
/*! @constant kSIQuantityInverseLength */
#define kSIQuantityInverseLength                        STR("inverse length")
/*! @constant kSIQuantityWavenumber */
#define kSIQuantityWavenumber                           STR("wavenumber")
/*! @constant kSIQuantityLengthRatio */
#define kSIQuantityLengthRatio                          STR("length ratio")
/*! @constant kSIQuantityPlaneAngle */
#define kSIQuantityPlaneAngle                           STR("plane angle")


/*! @constant kSIQuantityMass */
#define kSIQuantityMass                                 STR("mass")
/*! @constant kSIQuantityInverseMass */
#define kSIQuantityInverseMass                          STR("inverse mass")
/*! @constant kSIQuantityMassRatio */
#define kSIQuantityMassRatio                            STR("mass ratio")


/*! @constant kSIQuantityTime */
#define kSIQuantityTime                                 STR("time")
/*! @constant kSIQuantityInverseTime */
#define kSIQuantityInverseTime                          STR("inverse time")
/*! @constant kSIQuantityFrequency */
#define kSIQuantityFrequency                            STR("frequency")
/*! @constant kSIQuantityRadioactivity */
#define kSIQuantityRadioactivity                        STR("radioactivity")
/*! @constant kSIQuantityTimeRatio */
#define kSIQuantityTimeRatio                            STR("time ratio")
/*! @constant kSIQuantityFrequencyRatio */
#define kSIQuantityFrequencyRatio                       STR("frequency ratio")

/*! @constant kSIQuantityInverseTimeSquared */
#define kSIQuantityInverseTimeSquared                   STR("inverse time squared")


/*! @constant kSIQuantityCurrent */
#define kSIQuantityCurrent                              STR("current")
/*! @constant kSIQuantityInverseCurrent */
#define kSIQuantityInverseCurrent                       STR("inverse current")
/*! @constant kSIQuantityCurrentRatio */
#define kSIQuantityCurrentRatio                         STR("current ratio")


/*! @constant kSIQuantityTemperature */
#define kSIQuantityTemperature                          STR("temperature")
/*! @constant kSIQuantityInverseTemperature */
#define kSIQuantityInverseTemperature                   STR("inverse temperature")
/*! @constant kSIQuantityTemperatureRatio */
#define kSIQuantityTemperatureRatio                     STR("temperature ratio")

/*! @constant kSIQuantityTemperatureGradient */
#define kSIQuantityTemperatureGradient                  STR("temperature gradient")


/*! @constant kSIQuantityAmount */
#define kSIQuantityAmount                               STR("amount")
/*! @constant kSIQuantityInverseAmount */
#define kSIQuantityInverseAmount                        STR("inverse amount")
/*! @constant kSIQuantityAmountRatio */
#define kSIQuantityAmountRatio                          STR("amount ratio")


/*! @constant kSIQuantityLuminousIntensity */
#define kSIQuantityLuminousIntensity                    STR("luminous intensity")
/*! @constant kSIQuantityInverseLuminousIntensity */
#define kSIQuantityInverseLuminousIntensity             STR("inverse luminous intensity")
/*! @constant kSIQuantityLuminousIntensityRatio */
#define kSIQuantityLuminousIntensityRatio               STR("luminous intensity ratio")


/*! @constant kSIQuantityArea */
#define kSIQuantityArea                                 STR("area")
/*! @constant kSIQuantityInverseArea */
#define kSIQuantityInverseArea                          STR("inverse area")
/*! @constant kSIQuantityAreaRatio */
#define kSIQuantityAreaRatio                            STR("area ratio")
/*! @constant kSIQuantitySolidAngle */
#define kSIQuantitySolidAngle                           STR("solid angle")


/*! @constant kSIQuantityVolume */
#define kSIQuantityVolume                               STR("volume")
/*! @constant kSIQuantityInverseVolume */
#define kSIQuantityInverseVolume                        STR("inverse volume")
/*! @constant kSIQuantityVolumeRatio */
#define kSIQuantityVolumeRatio                          STR("volume ratio")


/*! @constant kSIQuantitySpeed */
#define kSIQuantitySpeed                                STR("speed")
/*! @constant kSIQuantityVelocity */
#define kSIQuantityVelocity                             STR("velocity")

/*! @constant kSIQuantityLinearMomentum */
#define kSIQuantityLinearMomentum                       STR("linear momentum")

/*! @constant kSIQuantityAngularMomentum */
#define kSIQuantityAngularMomentum                      STR("angular momentum")

/*! @constant kSIQuantityMomentOfInertia */
#define kSIQuantityMomentOfInertia                      STR("moment of inertia")

/*! @constant kSIQuantityAcceleration */
#define kSIQuantityAcceleration                         STR("acceleration")

/*! @constant kSIQuantityMassFlowRate */
#define kSIQuantityMassFlowRate                         STR("mass flow rate")

/*! @constant kSIQuantityMassFlux */
#define kSIQuantityMassFlux                             STR("mass flux")

/*! @constant kSIQuantityDensity */
#define kSIQuantityDensity                              STR("density")

/*! @constant kSIQuantitySpecificGravity */
#define kSIQuantitySpecificGravity                      STR("specific gravity")

/*! @constant kSIQuantitySpecificSurfaceArea */
#define kSIQuantitySpecificSurfaceArea                  STR("specific surface area")

/*! @constant kSIQuantitySurfaceAreaToVolumeRatio */
#define kSIQuantitySurfaceAreaToVolumeRatio             STR("surface area to volume ratio")

/*! @constant kSIQuantitySurfaceDensity */
#define kSIQuantitySurfaceDensity                       STR("surface density")

/*! @constant kSIQuantitySpecificVolume */
#define kSIQuantitySpecificVolume                       STR("specific volume")

/*! @constant kSIQuantityCurrentDensity */
#define kSIQuantityCurrentDensity                       STR("current density")

/*! @constant kSIQuantityMagneticFieldStrength */
#define kSIQuantityMagneticFieldStrength                STR("magnetic field strength")

/*! @constant kSIQuantityLuminance */
#define kSIQuantityLuminance                            STR("luminance")

/*! @constant kSIQuantityRefractiveIndex */
#define kSIQuantityRefractiveIndex                      STR("refractive index")

/*! @constant kSIQuantityFluidity */
#define kSIQuantityFluidity                             STR("fluidity")

/*! @constant kSIQuantityMomentOfForce */
#define kSIQuantityMomentOfForce                        STR("moment of force")

/*! @constant kSIQuantitySurfaceTension */
#define kSIQuantitySurfaceTension                       STR("surface tension")

/*! @constant kSIQuantitySurfaceEnergy */
#define kSIQuantitySurfaceEnergy                        STR("surface energy")

/*! @constant kSIQuantityAngularSpeed */
#define kSIQuantityAngularSpeed                         STR("angular speed")

/*! @constant kSIQuantityAngularVelocity */
#define kSIQuantityAngularVelocity                      STR("angular velocity")

/*! @constant kSIQuantityAngularAcceleration */
#define kSIQuantityAngularAcceleration                  STR("angular acceleration")

/*! @constant kSIQuantityHeatFluxDensity */
#define kSIQuantityHeatFluxDensity                      STR("heat flux density")

/*! @constant kSIQuantityIrradiance */
#define kSIQuantityIrradiance                           STR("irradiance")

/*! @constant kSIQuantitySpectralRadiantFluxDensity */
#define kSIQuantitySpectralRadiantFluxDensity           STR("spectral radiant flux density")

/*! @constant kSIQuantityHeatCapacity */
#define kSIQuantityHeatCapacity                         STR("heat capacity")
/*! @constant kSIQuantityEntropy */
#define kSIQuantityEntropy                              STR("entropy")

/*! @constant kSIQuantitySpecificHeatCapacity */
#define kSIQuantitySpecificHeatCapacity                 STR("specific heat capacity")
/*! @constant kSIQuantitySpecificEntropy */
#define kSIQuantitySpecificEntropy                      STR("specific entropy")

/*! @constant kSIQuantitySpecificEnergy */
#define kSIQuantitySpecificEnergy                       STR("specific energy")

/*! @constant kSIQuantityThermalConductance */
#define kSIQuantityThermalConductance                  STR("thermal conductance")

/*! @constant kSIQuantityThermalConductivity */
#define kSIQuantityThermalConductivity                  STR("thermal conductivity")

/*! @constant kSIQuantityEnergyDensity */
#define kSIQuantityEnergyDensity                        STR("energy density")

/*! @constant kSIQuantityElectricFieldStrength */
#define kSIQuantityElectricFieldStrength                STR("electric field strength")

/*! @constant kSIQuantityElectricFieldGradient */
#define kSIQuantityElectricFieldGradient                STR("electric field gradient")

/*! @constant kSIQuantityElectricChargeDensity */
#define kSIQuantityElectricChargeDensity                STR("electric charge density")

/*! @constant kSIQuantitySurfaceChargeDensity */
#define kSIQuantitySurfaceChargeDensity                 STR("surface charge density")

/*! @constant kSIQuantityElectricFlux */
#define kSIQuantityElectricFlux                         STR("electric flux")

/*! @constant kSIQuantityElectricFluxDensity */
#define kSIQuantityElectricFluxDensity                  STR("electric flux density")

/*! @constant kSIQuantityElectricDisplacement */
#define kSIQuantityElectricDisplacement                 STR("electric displacement")

/*! @constant kSIQuantityPermittivity */
#define kSIQuantityPermittivity                         STR("permittivity")

/*! @constant kSIQuantityPermeability */
#define kSIQuantityPermeability                         STR("permeability")

/*! @constant kSIQuantityMolarEnergy */
#define kSIQuantityMolarEnergy                          STR("molar energy")

/*! @constant kSIQuantityMolarEntropy */
#define kSIQuantityMolarEntropy                         STR("molar entropy")

/*! @constant kSIQuantityMolarHeatCapacity */
#define kSIQuantityMolarHeatCapacity                    STR("molar heat capacity")

/*! @constant kSIQuantityMolarMass */
#define kSIQuantityMolarMass                            STR("molar mass")

/*! @constant kSIQuantityMolality */
#define kSIQuantityMolality                             STR("molality")

/*! @constant kSIQuantityDiffusionFlux */
#define kSIQuantityDiffusionFlux                        STR("diffusion flux")

/*! @constant kSIQuantityMassToChargeRatio */
#define kSIQuantityMassToChargeRatio                    STR("mass to charge ratio")

/*! @constant kSIQuantityChargeToMassRatio */
#define kSIQuantityChargeToMassRatio                    STR("charge to mass ratio")

/*! @constant kSIQuantityRadiationExposure */
#define kSIQuantityRadiationExposure                    STR("radiation exposure")

/*! @constant kSIQuantityAbsorbedDoseRate */
#define kSIQuantityAbsorbedDoseRate                     STR("absorbed dose rate")

/*! @constant kSIQuantityRadiantIntensity */
#define kSIQuantityRadiantIntensity                     STR("radiant intensity")

/*! @constant kSIQuantitySpectralRadiantIntensity */
#define kSIQuantitySpectralRadiantIntensity             STR("spectral radiant intensity")

/*! @constant kSIQuantityRadiance */
#define kSIQuantityRadiance                             STR("radiance")

/*! @constant kSIQuantitySpectralRadiance */
#define kSIQuantitySpectralRadiance                     STR("spectral radiance")

/*! @constant kSIQuantityPorosity */
#define kSIQuantityPorosity                             STR("porosity")

/*! @constant kSIQuantityAngularFrequency */
#define kSIQuantityAngularFrequency                     STR("angular frequency")

/*! @constant kSIQuantityForce */
#define kSIQuantityForce                                STR("force")

/*! @constant kSIQuantityTorque */
#define kSIQuantityTorque                               STR("torque")

/*! @constant kSIQuantityPressure */
#define kSIQuantityPressure                             STR("pressure")
/*! @constant kSIQuantityStress */
#define kSIQuantityStress                               STR("stress")
/*! @constant kSIQuantityElasticModulus */
#define kSIQuantityElasticModulus                       STR("elastic modulus")

/*! @constant kSIQuantityCompressibility */
#define kSIQuantityCompressibility                      STR("compressibility")
/*! @constant kSIQuantityStressOpticCoefficient */
#define kSIQuantityStressOpticCoefficient               STR("stress-optic coefficient")

/*! @constant kSIQuantityPressureGradient */
#define kSIQuantityPressureGradient                     STR("pressure gradient")

/*! @constant kSIQuantityEnergy */
#define kSIQuantityEnergy                               STR("energy")

/*! @constant kSIQuantitySpectralRadiantEnergy */
#define kSIQuantitySpectralRadiantEnergy                STR("spectral radiant energy")

/*! @constant kSIQuantityPower */
#define kSIQuantityPower                                STR("power")

/*! @constant kSIQuantitySpectralPower */
#define kSIQuantitySpectralPower                        STR("spectral power")

/*! @constant kSIQuantityVolumePowerDensity */
#define kSIQuantityVolumePowerDensity                   STR("volume power density")

/*! @constant kSIQuantitySpecificPower */
#define kSIQuantitySpecificPower                        STR("specific power")

/*! @constant kSIQuantityRadiantFlux */
#define kSIQuantityRadiantFlux                          STR("radiant flux")

/*! @constant kSIQuantityElectricCharge */
#define kSIQuantityElectricCharge                       STR("electric charge")

/*! @constant kSIQuantityAmountOfElectricity */
#define kSIQuantityAmountOfElectricity                  STR("amount of electricity")

/*! @constant kSIQuantityElectricPotentialDifference */
#define kSIQuantityElectricPotentialDifference          STR("electric potential difference")

/*! @constant kSIQuantityElectromotiveForce */
#define kSIQuantityElectromotiveForce                   STR("electromotive force")

/*! @constant kSIQuantityElectricPolarizability */
#define kSIQuantityElectricPolarizability                 STR("electric polarizability")

/*! @constant kSIQuantityElectricDipoleMoment */
#define kSIQuantityElectricDipoleMoment                 STR("electric dipole moment")

/*! @constant kSIQuantityVoltage */
#define kSIQuantityVoltage                              STR("voltage")

/*! @constant kSIQuantityCapacitance */
#define kSIQuantityCapacitance                          STR("capacitance")

/*! @constant kSIQuantityElectricResistance */
#define kSIQuantityElectricResistance                   STR("electric resistance")

/*! @constant kSIQuantityElectricResistancePerLength */
#define kSIQuantityElectricResistancePerLength          STR("electric resistance per length")

/*! @constant kSIQuantityElectricResistivity */
#define kSIQuantityElectricResistivity                  STR("electric resistivity")

/*! @constant kSIQuantityElectricConductance */
#define kSIQuantityElectricConductance                  STR("electric conductance")

/*! @constant kSIQuantityElectricConductivity */
#define kSIQuantityElectricConductivity                 STR("electric conductivity")

/*! @constant kSIQuantityElectricalMobility */
#define kSIQuantityElectricalMobility                   STR("electrical mobility")

/*! @constant kSIQuantityMolarConductivity */
#define kSIQuantityMolarConductivity                    STR("molar conductivity")

/*! @constant kSIQuantityMagneticDipoleMoment */
#define kSIQuantityMagneticDipoleMoment                 STR("magnetic dipole moment")

/*! @constant kSIQuantityMagneticDipoleMomentRatio */
#define kSIQuantityMagneticDipoleMomentRatio            STR("magnetic dipole moment ratio")

/*! @constant kSIQuantityMagneticFlux */
#define kSIQuantityMagneticFlux                         STR("magnetic flux")

/*! @constant kSIQuantityMagneticFluxDensity */
#define kSIQuantityMagneticFluxDensity                  STR("magnetic flux density")

/*! @constant kSIQuantityMolarMagneticSusceptibility */
#define kSIQuantityMolarMagneticSusceptibility          STR("molar magnetic susceptibility")

/*! @constant kSIQuantityInverseMagneticFluxDensity */
#define kSIQuantityInverseMagneticFluxDensity           STR("inverse magnetic flux density")

/*! @constant kSIQuantityMagneticFieldGradient */
#define kSIQuantityMagneticFieldGradient                STR("magnetic field gradient")

/*! @constant kSIQuantityInductance */
#define kSIQuantityInductance                           STR("inductance")

/*! @constant kSIQuantityLuminousFlux */
#define kSIQuantityLuminousFlux                         STR("luminous flux")

/*! @constant kSIQuantityLuminousFluxDensity */
#define kSIQuantityLuminousFluxDensity                  STR("luminous flux density")

/*! @constant kSIQuantityLuminousEnergy */
#define kSIQuantityLuminousEnergy                       STR("luminous energy")

/*! @constant kSIQuantityIlluminance */
#define kSIQuantityIlluminance                          STR("illuminance")

/*! @constant kSIQuantityAbsorbedDose */
#define kSIQuantityAbsorbedDose                         STR("absorbed dose")

/*! @constant kSIQuantityDoseEquivalent */
#define kSIQuantityDoseEquivalent                       STR("dose equivalent")

/*! @constant kSIQuantityCatalyticActivity */
#define kSIQuantityCatalyticActivity                    STR("catalytic activity")

/*! @constant kSIQuantityCatalyticActivityConcentration */
#define kSIQuantityCatalyticActivityConcentration       STR("catalytic activity concentration")

/*! @constant kSIQuantityCatalyticActivityContent */
#define kSIQuantityCatalyticActivityContent             STR("catalytic activity content")

/*! @constant kSIQuantityAction */
#define kSIQuantityAction                               STR("action")

/*! @constant kSIQuantityReducedAction */
#define kSIQuantityReducedAction                        STR("reduced action")

/*! @constant kSIQuantityKinematicViscosity */
#define kSIQuantityKinematicViscosity                   STR("kinematic viscosity")

/*! @constant kSIQuantityDiffusionCoefficient */
#define kSIQuantityDiffusionCoefficient                 STR("diffusion coefficient")

/*! @constant kSIQuantityCirculation */
#define kSIQuantityCirculation                          STR("circulation")

/*! @constant kSIQuantityDynamicViscosity */
#define kSIQuantityDynamicViscosity                     STR("dynamic viscosity")

/*! @constant kSIQuantityAmountConcentration */
#define kSIQuantityAmountConcentration                  STR("amount concentration")

/*! @constant kSIQuantityMassConcentration */
#define kSIQuantityMassConcentration                    STR("mass concentration")

/*! @constant kSIQuantityChargePerAmount */
#define kSIQuantityChargePerAmount                      STR("charge per amount")

/*! @constant kSIQuantityGravitationalConstant */
#define kSIQuantityGravitationalConstant                STR("gravitational constant")

/*! @constant kSIQuantityLengthPerVolume */
#define kSIQuantityLengthPerVolume                      STR("distance per volume")

/*! @constant kSIQuantityVolumePerLength */
#define kSIQuantityVolumePerLength                      STR("volume per length")

/*! @constant kSIQuantityVolumetricFlowRate */
#define kSIQuantityVolumetricFlowRate                   STR("volumetric flow rate")

/*! @constant kSIQuantityFrequencyPerMagneticFluxDensity */
#define kSIQuantityFrequencyPerMagneticFluxDensity      STR("frequency per magnetic flux density")

/*! @constant kSIQuantityPowerPerLuminousFlux */
#define kSIQuantityPowerPerLuminousFlux                 STR("power per luminous flux")

/*! @constant kSIQuantityLuminousEfficacy */
#define kSIQuantityLuminousEfficacy                     STR("luminous efficacy")

/*! @constant kSIQuantityRockPermeability */
#define kSIQuantityRockPermeability                     STR("rock permeability")

/*! @constant kSIQuantityGyromagneticRatio */
#define kSIQuantityGyromagneticRatio                    STR("gyromagnetic ratio")

/*! @constant kSIQuantityHeatTransferCoefficient */
#define kSIQuantityHeatTransferCoefficient              STR("heat transfer coefficient")

/*! @constant kSIQuantityGasPermeance */
#define kSIQuantityGasPermeance                         STR("gas permeance")

/*! @constant kSIQuantityPowerPerAreaPerTemperatureToFourthPower */
#define kSIQuantityPowerPerAreaPerTemperatureToFourthPower        STR("stefan-boltzmann constant")

/*! @constant kSIQuantityFirstHyperPolarizability */
#define kSIQuantityFirstHyperPolarizability                 STR("first hyperpolarizability")

/*! @constant kSIQuantitySecondHyperPolarizability */
#define kSIQuantitySecondHyperPolarizability                STR("second hyperpolarizability")

/*! @constant kSIQuantityElectricQuadrupoleMoment */
#define kSIQuantityElectricQuadrupoleMoment                 STR("electric quadrupole moment")

/*! @constant kSIQuantityMagnetizability */
#define kSIQuantityMagnetizability                          STR("magnetizability")

/*! @constant kSIQuantitySecondRadiationConstant */
#define kSIQuantitySecondRadiationConstant                  STR("second radiation constant")

/*! @constant kSIQuantityWavelengthDisplacementConstant */
#define kSIQuantityWavelengthDisplacementConstant           STR("wavelength displacement constant")

/*! @constant kSIQuantityFineStructureConstant */
#define kSIQuantityFineStructureConstant                    STR("fine structure constant")

/*! @constant kSIQuantityRatePerAmountConcentrationPerTime */
#define kSIQuantityRatePerAmountConcentrationPerTime        STR("inverse amount concentration inverse time")


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
