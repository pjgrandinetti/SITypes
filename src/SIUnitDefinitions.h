// Derived Constants
#pragma mark Derived Constants
    double hbar = kSIPlanckConstant / (2 * kSIPi);
    // (1/(4•π•ε_0))•q_e^2/(c_0•h_P/(2•π))
    double alpha = (1. / (4. * kSIPi * kSIElectricConstant)) * kSIElementaryCharge * kSIElementaryCharge / (kSISpeedOfLight * kSIPlanckConstant / (2 * kSIPi));
    double lightYear = kSIYear *kSISpeedOfLight;
    // E_h = m_e • (q_e^2/(2•ε_0•h_P))^2
    double E_h = kSIElectronMass * (kSIElementaryCharge * kSIElementaryCharge / (2 * kSIElectricConstant * kSIPlanckConstant)) * (kSIElementaryCharge * kSIElementaryCharge / (2 * kSIElectricConstant * kSIPlanckConstant));
    // a_0 = ε_0•h_P^2/(π*m_e•q_e^2)
    double a_0 = kSIElectricConstant * kSIPlanckConstant * kSIPlanckConstant / (kSIPi * kSIElectronMass * kSIElementaryCharge * kSIElementaryCharge);
    double R_H = kSIElectronMass * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge / (8 * kSIElectricConstant * kSIElectricConstant * kSIPlanckConstant * kSIPlanckConstant * kSIPlanckConstant * kSISpeedOfLight);
    // R_H = m_e•q_e^4/(8•ε_0^2•h_P^3•c_0)
    double Ry = kSIPlanckConstant * kSISpeedOfLight * R_H;
    double Λ_0 = E_h / (kSIElementaryCharge * a_0 * a_0);
    double G_0 = 2 * kSIElementaryCharge * kSIElementaryCharge / kSIPlanckConstant;
    double mu_N = kSIElementaryCharge * hbar / (2 * kSIProtonMass);
    double mu_e = kSIElementaryCharge * hbar / (2 * kSIElectronMass);
    double planckTime = sqrt(hbar * (double)kSIGravitationalConstant / ((double)kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight));
    // sqrt(h_P * &G/(2*π*c_0^3))
    double planckLength = sqrt(hbar * (double)kSIGravitationalConstant / ((double)kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight));
    double planckMass = sqrt(hbar * kSISpeedOfLight / kSIGravitationalConstant);
    double planckTemperature = planckMass * kSISpeedOfLight * kSISpeedOfLight / kSIBoltmannConstant;
    //
    double naturalTimeUnit = hbar / (kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight);
    double naturalMomentumUnit = kSIElectronMass * kSISpeedOfLight;
    double naturalEnergyUnit = kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight;
    double alphaParticleMassEnergy = kSIAlphaParticleMass * kSISpeedOfLight * kSISpeedOfLight;
    double secondRadiationConstant = kSIPlanckConstant * kSISpeedOfLight / kSIBoltmannConstant;
    double vonKlitzingConstant = kSIPlanckConstant / (kSIElementaryCharge * kSIElementaryCharge);
    OCStringRef error = NULL;
    AddToLibPrefixed(kSIQuantityLength, STR("meter"), STR("meters"), STR("m"), 1, &error);
    AddToLibPrefixed(kSIQuantityMass, STR("gram"), STR("grams"), STR("g"), 1, &error);
    AddToLibPrefixed(kSIQuantityTime, STR("second"), STR("seconds"), STR("s"), 1, &error);
    AddToLibPrefixed(kSIQuantityCurrent, STR("ampere"), STR("amperes"), STR("A"), 1, &error);
    AddToLibPrefixed(kSIQuantityTemperature, STR("kelvin"), STR("kelvin"), STR("K"), 1, &error);
    AddToLibPrefixed(kSIQuantityAmount, STR("mole"), STR("moles"), STR("mol"), 1, &error);
    AddToLibPrefixed(kSIQuantityLuminousIntensity, STR("candela"), STR("candelas"), STR("cd"), 1, &error);
#pragma mark kSIQuantityDimensionless
    AddToLib(kSIQuantityDimensionless, STR("dimensionless"), STR("dimensionless"), STR(" "), 1, &error);
    AddToLib(kSIQuantityDimensionless, STR("percent"), STR("percent"), STR("%"), 0.01, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per thousand"), STR("parts per thousand"), STR("‰"), 0.001, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per ten thousand"), STR("parts per ten thousand"), STR("‱"), 0.0001, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per million"), STR("parts per million"), STR("ppm"), 1.e-6, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per billion"), STR("parts per billion"), STR("ppb"), 1.e-9, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per trillion"), STR("parts per trillion"), STR("ppt"), 1.e-12, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per quadrillion"), STR("parts per quadrillion"), STR("ppq"), 1.e-15, &error);
    AddToLib(kSIQuantityDimensionless, STR("euler number"), STR("euler number"), STR("exp(1)"), kSIEulersNumber, &error);
//
#pragma mark kSIQuantityFineStructureConstant
    AddToLib(kSIQuantityFineStructureConstant, STR("fine structure constant"), STR("fine structure constant"), STR("α"), alpha, &error);
    AddToLib(kSIQuantityFineStructureConstant, STR("inverse fine structure constant"), STR("inverse fine structure constant"), STR("(1/α)"), 1 / alpha, &error);
//
#pragma mark kSIQuantityLength
    AddToLib(kSIQuantityLength, STR("astronomical unit"), STR("astronomical units"), STR("ua"), 1.49597870691e11, &error);
    AddToLib(kSIQuantityLength, STR("light year"), STR("light years"), STR("ly"), lightYear, &error);
    AddToLib(kSIQuantityLength, STR("ångström"), STR("ångströms"), STR("Å"), 1.e-10, &error);
    AddToLib(kSIQuantityLength, STR("atomic unit of length"), STR("atomic unit of length"), STR("a_0"), a_0, &error);
    //  AddToLib(kSIQuantityLength, STR("nautical mile"), STR("nautical miles"), STR("M"),1852., &error);
    AddToLib(kSIQuantityLength, STR("fathom"), STR("fathoms"), STR("ftm"), 2 * kSIMile / 1760, &error);
    AddToLib(kSIQuantityLength, STR("inch"), STR("inches"), STR("in"), kSIMile / 63360, &error);
    AddToLib(kSIQuantityLength, STR("foot"), STR("feet"), STR("ft"), kSIMile / 5280, &error);
    AddToLib(kSIQuantityLength, STR("yard"), STR("yards"), STR("yd"), kSIMile / 1760, &error);
    AddToLib(kSIQuantityLength, STR("mile"), STR("miles"), STR("mi"), kSIMile, &error);
    AddToLib(kSIQuantityLength, STR("link"), STR("links"), STR("li"), kSIMile / 5280 * 33 / 50, &error);
    AddToLib(kSIQuantityLength, STR("rod"), STR("rods"), STR("rod"), kSIMile / 5280 * 16.5, &error);
    AddToLib(kSIQuantityLength, STR("chain"), STR("chains"), STR("ch"), kSIMile / 5280 * 16.5 * 4, &error);
    AddToLib(kSIQuantityLength, STR("furlong"), STR("furlongs"), STR("fur"), kSIMile / 5280 * 16.5 * 4 * 10, &error);
    AddToLib(kSIQuantityLength, STR("league"), STR("leagues"), STR("lea"), kSIMile * 3, &error);
    AddToLib(kSIQuantityLength, STR("compton wavelength"), STR("compton wavelengths"), STR("λ_C"), kSIPlanckConstant / (kSIElectronMass * kSISpeedOfLight), &error);
    AddToLib(kSIQuantityLength, STR("natural unit of length"), STR("natural units of length"), STR("ƛ_C"), 386.15926764e-15, &error);
    AddToLib(kSIQuantityLength, STR("planck length"), STR("planck length"), STR("l_P"), planckLength, &error);
//
#pragma mark kSIQuantityWavenumber, kSIQuantityInverseLength
    AddToLib(kSIQuantityWavenumber, STR("inverse meter"), STR("inverse meters"), STR("(1/m)"), 1, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse centimeter"), STR("inverse centimeters"), STR("(1/cm)"), 100, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse ångström"), STR("inverse ångströms"), STR("(1/Å)"), 1.e10, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse mile"), STR("inverse miles"), STR("(1/mi)"), 1. / kSIMile, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse yard"), STR("inverse yards"), STR("(1/yd)"), 1. / (kSIMile / 1760), &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse foot"), STR("inverse feet"), STR("(1/ft)"), 1. / (kSIMile / 5280), &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse inch"), STR("inverse inches"), STR("(1/in)"), 1. / (kSIMile / 63360), &error);
    AddToLib(kSIQuantityWavenumber, STR("rydberg constant"), STR("rydberg constant"), STR("R_∞"), R_H, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse atomic unit of length"), STR("inverse atomic unit of length"), STR("(1/a_0)"), 1 / a_0, &error);
    OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityWavenumber);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityInverseLength, units);
//
#pragma mark kSIQuantityLengthRatio, kSIQuantityPlaneAngle
    AddToLibPrefixed(kSIQuantityLengthRatio, STR("meter per meter"), STR("meters per meter"), STR("m/m"), 1.0, &error);
    AddToLibPrefixed(kSIQuantityLengthRatio, STR("radian"), STR("radians"), STR("rad"), 1.0, &error);
    AddToLibPrefixed(kSIQuantityLengthRatio, STR("degree"), STR("degrees"), STR("°"), kSIPi / 180., &error);
    AddToLib(kSIQuantityLengthRatio, STR("pi"), STR("pi"), STR("π"), kSIPi, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityLengthRatio);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityPlaneAngle, units);
//
#pragma mark kSIQuantityMass
    AddToLib(kSIQuantityMass, STR("microgram"), STR("micrograms"), STR("mcg"), 1e-9, &error);
    AddToLibPrefixed(kSIQuantityMass, STR("tonne"), STR("tonnes"), STR("t"), 1e3, &error);
    AddToLibPrefixed(kSIQuantityMass, STR("dalton"), STR("daltons"), STR("Da"), kSIAtomicMassConstant, &error);
    AddToLib(kSIQuantityMass, STR("atomic mass unit"), STR("atomic mass units"), STR("u"), kSIAtomicMassConstant, &error);
    AddToLib(kSIQuantityMass, STR("atomic mass constant"), STR("atomic mass constant"), STR("m_u"), kSIAtomicMassConstant, &error);
    AddToLib(kSIQuantityMass, STR("electron mass"), STR("electron mass"), STR("m_e"), kSIElectronMass, &error);
    AddToLib(kSIQuantityMass, STR("proton mass"), STR("proton mass"), STR("m_p"), kSIProtonMass, &error);
    AddToLib(kSIQuantityMass, STR("neutron mass"), STR("neutron mass"), STR("m_n"), kSINeutronMass, &error);
    AddToLib(kSIQuantityMass, STR("alpha particle mass"), STR("alpha particle mass"), STR("m_a"), kSIAlphaParticleMass, &error);
    AddToLib(kSIQuantityMass, STR("muon mass"), STR("muon mass"), STR("m_µ"), kSIMuonMass, &error);
    AddToLib(kSIQuantityMass, STR("ton"), STR("tons"), STR("ton"), kSIPound * 2000, &error);
    AddToLib(kSIQuantityMass, STR("hundredweight"), STR("hundredweight"), STR("cwt"), kSIPound * 100, &error);
    AddToLib(kSIQuantityMass, STR("pound"), STR("pounds"), STR("lb"), kSIPound, &error);
    AddToLib(kSIQuantityMass, STR("stone"), STR("stones"), STR("st"), 6.35029318, &error);
    AddToLib(kSIQuantityMass, STR("ounce"), STR("ounces"), STR("oz"), kSIOunce, &error);
    AddToLib(kSIQuantityMass, STR("grain"), STR("grains"), STR("gr"), kSIPound / 7000, &error);
    AddToLib(kSIQuantityMass, STR("dram"), STR("drams"), STR("dr"), kSIPound / 256, &error);
    AddToLib(kSIQuantityMass, STR("tonUK"), STR("tonsUK"), STR("tonUK"), kSIPound * 2240, &error);
    AddToLib(kSIQuantityMass, STR("hundredweightUK"), STR("hundredweightUK"), STR("cwtUK"), kSIPound * 112, &error);
    AddToLib(kSIQuantityMass, STR("planck mass"), STR("planck mass"), STR("m_P"), planckMass, &error);
//
#pragma mark kSIQuantityInverseMass
    AddToLib(kSIQuantityInverseMass, STR("inverse kilogram"), STR("inverse kilograms"), STR("(1/kg)"), 1., &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse tonne"), STR("inverse tonnes"), STR("(1/t)"), 1. / 1e3, &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse ton"), STR("inverse tons"), STR("(1/ton)"), 1. / (kSIPound * 2000), &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse stone"), STR("inverse stones"), STR("(1/st)"), 1. / 6.35029318, &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse pound"), STR("inverse pounds"), STR("(1/lb)"), 1. / kSIPound, &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse ounce"), STR("inverse ounces"), STR("(1/oz)"), 1. / kSIOunce, &error);
//
#pragma mark kSIQuantityMassRatio
    AddToLibPrefixed(kSIQuantityMassRatio, STR("gram per kilogram"), STR("grams per kilogram"), STR("g/kg"), 0.001, &error);
//
#pragma mark kSIQuantityTime
    AddToLib(kSIQuantityTime, STR("minute"), STR("minutes"), STR("min"), kSIMinute, &error);
    AddToLib(kSIQuantityTime, STR("hour"), STR("hours"), STR("h"), kSIHour, &error);
    AddToLib(kSIQuantityTime, STR("day"), STR("days"), STR("d"), kSIDay, &error);
    AddToLib(kSIQuantityTime, STR("week"), STR("weeks"), STR("wk"), kSIWeek, &error);
    AddToLib(kSIQuantityTime, STR("month"), STR("months"), STR("mo"), kSIMonth, &error);
    AddToLib(kSIQuantityTime, STR("year"), STR("years"), STR("yr"), kSIYear, &error);
    AddToLib(kSIQuantityTime, STR("decade"), STR("decades"), STR("dayr"), kSIDecade, &error);
    AddToLib(kSIQuantityTime, STR("century"), STR("centuries"), STR("hyr"), kSICentury, &error);
    AddToLib(kSIQuantityTime, STR("millennium"), STR("millennia"), STR("kyr"), kSIMillennium, &error);
    AddToLib(kSIQuantityTime, STR("atomic unit of time"), STR("atomic units of time"), STR("ℏ/E_h"), hbar / E_h, &error);
    AddToLib(kSIQuantityTime, STR("natural unit of time"), STR("natural units of time"), STR("ℏ/(m_e•c_0^2)"), naturalTimeUnit, &error);
    AddToLib(kSIQuantityTime, STR("planck time"), STR("planck time"), STR("t_P"), planckTime, &error);
//
#pragma mark kSIQuantityInverseTime
    AddToLib(kSIQuantityInverseTime, STR("inverse seconds"), STR("inverse seconds"), STR("(1/s)"), 1., &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse minute"), STR("inverse minutes"), STR("(1/min)"), 1. / kSIMinute, &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse hour"), STR("inverse hours"), STR("(1/h)"), 1. / kSIHour, &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse day"), STR("inverse days"), STR("(1/d)"), 1. / kSIDay, &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse week"), STR("inverse weeks"), STR("(1/wk)"), 1. / (kSIWeek), &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse month"), STR("inverse months"), STR("(1/month)"), 1. / kSIMonth, &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse year"), STR("inverse years"), STR("(1/yr)"), 1. /kSIYear, &error);
//
#pragma mark kSIQuantityFrequency
    AddToLibPrefixed(kSIQuantityFrequency, STR("hertz"), STR("hertz"), STR("Hz"), 1.0, &error);
//
#pragma mark kSIQuantityRadioactivity
    AddToLibPrefixed(kSIQuantityRadioactivity, STR("becquerel"), STR("becquerels"), STR("Bq"), 1.0, &error);
    AddToLibPrefixed(kSIQuantityRadioactivity, STR("curie"), STR("curies"), STR("Ci"), 3.7e10, &error);
//
#pragma mark kSIQuantityFrequencyRatio
    AddToLibPrefixed(kSIQuantityFrequencyRatio, STR("hertz per hertz"), STR("hertz per hertz"), STR("Hz/Hz"), 1., &error);
//
#pragma mark kSIQuantityTimeRatio
    AddToLibPrefixed(kSIQuantityTimeRatio, STR("second per second"), STR("seconds per second"), STR("s/s"), 1., &error);
//
#pragma mark kSIQuantityInverseTimeSquared
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse millisecond squared"), STR("inverse milliseconds squared"), STR("(1/ms^2)"), 1000000., &error);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse second squared"), STR("inverse seconds squared"), STR("(1/s^2)"), 1., &error);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse hour inverse second"), STR("inverse hour inverse seconds"), STR("(1/(h•s))"), 1. / kSIHour, &error);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse hour inverse minute"), STR("inverse hour inverse minutes"), STR("(1/(h•min))"), 1. / kSIHour / kSIMinute, &error);
//
#pragma mark kSIQuantityLinearMomentum
    AddToLib(kSIQuantityLinearMomentum, STR("natural unit of momentum"), STR("natural units of momentum"), STR("m_e•c_0"), naturalMomentumUnit, &error);
    AddToLibPrefixed(kSIQuantityLinearMomentum, STR("gram meter per second"), STR("gram meters per second"), STR("g•m/s"), 0.001, &error);
    AddToLibPrefixed(kSIQuantityLinearMomentum, STR("newton second"), STR("newton seconds"), STR("N•s"), 1, &error);
    AddToLib(kSIQuantityLinearMomentum, STR("atomic unit of momentum"), STR("atomic units of momentum"), STR("ℏ/a_0"), hbar / a_0, &error);
//
#pragma mark kSIQuantityEnergy
    AddToLib(kSIQuantityEnergy, STR("natural unit of energy"), STR("natural units of energy"), STR("m_e•c_0^2"), naturalEnergyUnit, &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("joule"), STR("joules"), STR("J"), 1., &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("watt hour"), STR("watt hour"), STR("W•h"), 3.6e3, &error);
    AddToLib(kSIQuantityEnergy, STR("rydberg"), STR("rydbergs"), STR("Ry"), Ry, &error);
    AddToLib(kSIQuantityEnergy, STR("alpha particle mass energy"), STR("alpha particle mass energy"), STR("m_a•c_0^2"), alphaParticleMassEnergy, &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("electronvolt"), STR("electronvolts"), STR("eV"), kSIElementaryCharge, &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("erg"), STR("ergs"), STR("erg"), 1e-7, &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("calorie"), STR("calories"), STR("cal"), kSICal, &error);
    AddToLib(kSIQuantityEnergy, STR("atomic unit of energy"), STR("atomic unit of energy"), STR("E_h"), E_h, &error);
    AddToLib(kSIQuantityEnergy, STR("british thermal unit"), STR("british thermal units"), STR("Btu"), kSIBtu, &error);
//
#pragma mark kSIQuantityTemperature
    AddToLib(kSIQuantityTemperature, STR("planck temperature"), STR("planck temperature"), STR("T_P"), planckTemperature, &error);
    AddToLib(kSIQuantityTemperature, STR("rankine"), STR("rankines"), STR("°R"), 0.555555555555556, &error);
    AddToLib(kSIQuantityTemperature, STR("fahrenheit"), STR("fahrenheit"), STR("°F"), 0.555555555555556, &error);
    AddToLib(kSIQuantityTemperature, STR("celsius"), STR("celsius"), STR("°C"), 1, &error);
//
#pragma mark kSIQuantityCurrent
    AddToLib(kSIQuantityCurrent, STR("atomic unit of current"), STR("atomic unit of current"), STR("q_e•E_h/ℏ"), kSIElementaryCharge * E_h / hbar, &error);
//
#pragma mark kSIQuantityInverseCurrent
    AddToLib(kSIQuantityInverseCurrent, STR("inverse ampere"), STR("inverse amperes"), STR("(1/A)"), 1., &error);
//
#pragma mark kSIQuantityCurrentRatio
    AddToLibPrefixed(kSIQuantityCurrentRatio, STR("ampere per ampere"), STR("ampere per ampere"), STR("A/A"), 1., &error);
//
#pragma mark kSIQuantityInverseTemperature
    AddToLib(kSIQuantityInverseTemperature, STR("inverse kelvin"), STR("inverse kelvin"), STR("(1/K)"), 1., &error);
//
#pragma mark kSIQuantityTemperatureRatio
    AddToLibPrefixed(kSIQuantityTemperatureRatio, STR("kelvin per kelvin"), STR("kelvin per kelvin"), STR("K/K"), 1., &error);
//
#pragma mark kSIQuantityTemperatureGradient
    AddToLibPrefixed(kSIQuantityTemperatureGradient, STR("kelvin per meter"), STR("kelvin per meter"), STR("K/m"), 1., &error);
    AddToLib(kSIQuantityTemperatureGradient, STR("celsius per meter"), STR("celsius per meter"), STR("°C/m"), 1, &error);
    AddToLib(kSIQuantityTemperatureGradient, STR("fahrenheit per foot"), STR("fahrenheit per foot"), STR("°F/ft"), 0.555555555555556 / (kSIMile / 5280), &error);
    AddToLib(kSIQuantityTemperatureGradient, STR("rankine per foot"), STR("rankines per foot"), STR("°R/ft"), 0.555555555555556 / (kSIMile / 5280), &error);
//
#pragma mark kSIQuantityInverseAmount
    AddToLib(kSIQuantityInverseAmount, STR("inverse mole"), STR("inverse moles"), STR("(1/mol)"), 1., &error);
    AddToLib(kSIQuantityInverseAmount, STR("avogadro constant"), STR("avogadro constant"), STR("N_A"), kSIAvogadroConstant, &error);
//
#pragma mark kSIQuantityAmountRatio
    AddToLibPrefixed(kSIQuantityAmountRatio, STR("mole per mole"), STR("moles per mole"), STR("mol/mol"), 1., &error);
//
#pragma mark kSIQuantityInverseLuminousIntensity
    AddToLib(kSIQuantityInverseLuminousIntensity, STR("inverse candela"), STR("inverse candelas"), STR("(1/cd)"), 1., &error);
//
#pragma mark kSIQuantityLuminousIntensityRatio
    AddToLibPrefixed(kSIQuantityLuminousIntensityRatio, STR("candela per candela"), STR("candelas per candela"), STR("cd/cd"), 1., &error);
//
#pragma mark kSIQuantityArea
    AddToLib(kSIQuantityArea, STR("hectare"), STR("hectares"), STR("ha"), 1e4, &error);
    AddToLib(kSIQuantityArea, STR("barn"), STR("barns"), STR("b"), 1.e-28, &error);
    AddToLib(kSIQuantityArea, STR("square meter"), STR("square meters"), STR("m^2"), 1, &error);
    AddToLib(kSIQuantityArea, STR("square centimeter"), STR("square centimeters"), STR("cm^2"), 0.0001, &error);
    AddToLib(kSIQuantityArea, STR("square kilometer"), STR("square kilometers"), STR("km^2"), 1000000, &error);
    AddToLib(kSIQuantityArea, STR("square inch"), STR("square inches"), STR("in^2"), 0.00064516, &error);
    AddToLib(kSIQuantityArea, STR("square foot"), STR("square feet"), STR("ft^2"), 0.09290304, &error);
    AddToLib(kSIQuantityArea, STR("square yard"), STR("square yards"), STR("yd^2"), 0.83612736, &error);
    AddToLib(kSIQuantityArea, STR("square mile"), STR("square miles"), STR("mi^2"), 2589988.110336, &error);
    AddToLib(kSIQuantityArea, STR("square rod"), STR("square rods"), STR("rod^2"), 5.029210 * 5.029210, &error);
    AddToLib(kSIQuantityArea, STR("square chain"), STR("square chains"), STR("ch^2"), 5.029210 * 5.029210 * 16, &error);
    AddToLib(kSIQuantityArea, STR("acre"), STR("acres"), STR("ac"), kSIAcre, &error);
    AddToLib(kSIQuantityArea, STR("township"), STR("townships"), STR("twp"), 2589988.110336 * 36., &error);
//
#pragma mark kSIQuantityInverseArea
    AddToLib(kSIQuantityInverseArea, STR("inverse hectare"), STR("inverse hectares"), STR("(1/ha)"), 1e-4, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse barn"), STR("inverse barns"), STR("(1/b)"), 1.e28, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square kilometer"), STR("inverse square kilometer"), STR("(1/km^2)"), 1. / 1000000, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square centimeter"), STR("inverse square centimeters"), STR("(1/cm^2)"), 1. / 0.0001, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square inch"), STR("inverse square inches"), STR("(1/in^2)"), 1. / 0.00064516, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square foot"), STR("inverse square feet"), STR("(1/ft^2)"), 1. / 0.09290304, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square yard"), STR("inverse square yards"), STR("(1/yd^2)"), 1. / 0.83612736, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse acre"), STR("inverse acres"), STR("(1/ac)"), 1. / kSIAcre, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square mile"), STR("inverse square miles"), STR("(1/mi^2)"), 1. / 2589988.110336, &error);
//
#pragma mark kSIQuantityRockPermeability
    AddToLibPrefixed(kSIQuantityRockPermeability, STR("darcy"), STR("darcys"), STR("Dc"), 9.869233e-13, &error);
//
#pragma mark kSIQuantitySolidAngle
    AddToLib(kSIQuantitySolidAngle, STR("steradian"), STR("steradians"), STR("sr"), 1, &error);
//
#pragma mark kSIQuantityAreaRatio
    AddToLib(kSIQuantityAreaRatio, STR("square meter per square meter"), STR("square meters per square meter"), STR("m^2/m^2"), 1, &error);
//
#pragma mark kSIQuantityVolume
    AddToLibPrefixed(kSIQuantityVolume, STR("liter"), STR("liters"), STR("L"), 1e-3, &error);
    AddToLib(kSIQuantityVolume, STR("cubic inch"), STR("cubic inches"), STR("in^3"),kSIInch * kSIInch * kSIInch, &error);
    AddToLib(kSIQuantityVolume, STR("cubic foot"), STR("cubic feet"), STR("ft^3"), kSIFoot* kSIFoot * kSIFoot, &error);
    AddToLib(kSIQuantityVolume, STR("cubic yard"), STR("cubic yards"), STR("yd^3"), kSIYard* kSIYard*kSIYard, &error);
    AddToLib(kSIQuantityVolume, STR("acre foot"), STR("acre feet"), STR("ac•ft"), kSIAcre*kSIFoot, &error);
    AddToLib(kSIQuantityVolume, STR("oil barrel"), STR("oil barrels"), STR("bbl"), 0.158987295, &error);
    AddToLib(kSIQuantityVolume, STR("thousand oil barrels"), STR("thousand oil barrels"), STR("Mbbl"), 0.158987295e3, &error);
    AddToLib(kSIQuantityVolume, STR("million oil barrels"), STR("million oil barrels"), STR("MMbbl"), 0.158987295e6, &error);
//
#pragma mark kSIQuantityInverseVolume
    AddToLib(kSIQuantityInverseVolume, STR("inverse liter"), STR("inverse liters"), STR("(1/L)"), 1. / 1e-3, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse milliliter"), STR("inverse milliliters"), STR("(1/mL)"), 1. / 1e-6, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic inch"), STR("inverse cubic inches"), STR("(1/in^3)"), 1. /kSIInch/kSIInch/kSIInch, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic foot"), STR("inverse cubic feet"), STR("(1/ft^3)"), 1. /kSIFoot/kSIFoot/kSIFoot, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic yard"), STR("inverse cubic yards"), STR("(1/yd^3)"), 1. / kSIYard/kSIYard/kSIYard, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic centimeter"), STR("inverse cubic centimeters"), STR("(1/cm^3)"), 1000000., &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic millimeter"), STR("inverse cubic millimeters"), STR("(1/mm^3)"), 1000000000., &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic micrometer"), STR("inverse cubic micrometers"), STR("(1/µm^3)"), 1e+18, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic ångström"), STR("inverse cubic ångströms"), STR("(1/Å^3)"), 1e+30, &error);
//
#pragma mark kSIQuantityVolumeRatio
    AddToLib(kSIQuantityVolumeRatio, STR("cubic meter per cubic meter"), STR("cubic meters per cubic meter"), STR("m^3/m^3"), 1, &error);
//
#pragma mark kSIQuantitySurfaceAreaToVolumeRatio
    AddToLib(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per cubic meter"), STR("square meters per cubic meter"), STR("m^2/m^3"), 1, &error);
    AddToLib(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per liter"), STR("square meters per liter"), STR("m^2/L"), 1000, &error);
//
#pragma mark kSIQuantitySpeed
    AddToLibPrefixed(kSIQuantitySpeed, STR("meter per second"), STR("meters per second"), STR("m/s"), 1, &error);
    AddToLibPrefixed(kSIQuantitySpeed, STR("meter per minute"), STR("meters per minute"), STR("m/min"), 1. / kSIMinute, &error);
    AddToLibPrefixed(kSIQuantitySpeed, STR("meter per hour"), STR("meters per hour"), STR("m/h"), 1. / kSIHour, &error);
    AddToLib(kSIQuantitySpeed, STR("knot"), STR("knots"), STR("kn"), 0.514444444444444, &error);
    AddToLib(kSIQuantitySpeed, STR("speed of light"), STR("speed of light"), STR("c_0"), kSISpeedOfLight, &error);
    AddToLib(kSIQuantitySpeed, STR("inch per second"), STR("inches per second"), STR("in/s"), kSIInch, &error);
    AddToLib(kSIQuantitySpeed, STR("inch per minute"), STR("inches per minute"), STR("in/min"), kSIInch / kSIMinute, &error);
    AddToLib(kSIQuantitySpeed, STR("inch per hour"), STR("inches per hour"), STR("in/h"), kSIInch / kSIHour, &error);
    AddToLib(kSIQuantitySpeed, STR("foot per second"), STR("feet per second"), STR("ft/s"), kSIFoot, &error);
    AddToLib(kSIQuantitySpeed, STR("foot per minute"), STR("feet per minute"), STR("ft/min"), kSIFoot / kSIMinute, &error);
    AddToLib(kSIQuantitySpeed, STR("foot per hour"), STR("feet per hour"), STR("ft/h"), kSIFoot / kSIHour, &error);
    AddToLib(kSIQuantitySpeed, STR("mile per second"), STR("miles per second"), STR("mi/s"), kSIMile, &error);
    AddToLib(kSIQuantitySpeed, STR("mile per minute"), STR("miles per minute"), STR("mi/min"), kSIMile / kSIMinute, &error);
    AddToLib(kSIQuantitySpeed, STR("mile per hour"), STR("miles per hour"), STR("mi/h"), kSIMile / kSIHour, &error);
    AddToLib(kSIQuantitySpeed, STR("atomic unit of velocity"), STR("atomic units of velocity"), STR("a_0•E_h/ℏ"), a_0 * E_h / hbar, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantitySpeed);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityVelocity, units);
//
#pragma mark kSIQuantityAngularMomentum
    AddToLibPrefixed(kSIQuantityAngularMomentum, STR("joule second"), STR("joules second"), STR("J•s"), 1, &error);
    AddToLibPrefixed(kSIQuantityAngularMomentum, STR("gram square meter per second"), STR("gram square meters per second"), STR("g•m^2/s"), 0.001, &error);
//
#pragma mark kSIQuantityAction
    AddToLib(kSIQuantityAction, STR("planck constant"), STR("planck constant"), STR("h_P"), kSIPlanckConstant, &error);
//
#pragma mark kSIQuantityReducedAction
    AddToLib(kSIQuantityReducedAction, STR("reduced planck constant"), STR("reduced planck constant"), STR("ℏ"), hbar, &error);
//
#pragma mark kSIQuantityCirculation
    AddToLib(kSIQuantityCirculation, STR("quantum of circulation"), STR("quantum of circulation"), STR("h_P/(2•m_e)"), hbar, &error);
//
#pragma mark kSIQuantitySecondRadiationConstant
    AddToLib(kSIQuantitySecondRadiationConstant, STR("second radiation constant"), STR("second radiation constant"), STR("h_P•c_0/k_B"), secondRadiationConstant, &error);
//
#pragma mark kSIQuantityElectricResistance
    AddToLib(kSIQuantityElectricResistance, STR("von klitzing constant"), STR("von klitzing constant"), STR("h_P/(q_e^2)"), vonKlitzingConstant, &error);
    AddToLib(kSIQuantityElectricResistance, STR("ohm"), STR("ohms"), STR("Ω"), 1., &error);
    AddToLib(kSIQuantityElectricResistance, STR("inverse conductance quantum"), STR("inverse conductance quantum"), STR("(1/G_0)"), 1 / G_0, &error);
    AddToLib(kSIQuantityElectricResistance, STR("characteristic impedance of vacuum"), STR("characteristic impedance of vacuum"), STR("Z_0"), 4 * kSIPi * 1.e-7 * kSISpeedOfLight, &error);
//
#pragma mark kSIQuantityAcceleration
    AddToLibPrefixed(kSIQuantityAcceleration, STR("meter per square second"), STR("meters per square second"), STR("m/s^2"), 1, &error);
    AddToLibPrefixed(kSIQuantityAcceleration, STR("meter per hour per second"), STR("meters per hour per second"), STR("m/(h•s)"), 1. / kSIHour, &error);
    AddToLib(kSIQuantityAcceleration, STR("gravity acceleration"), STR("gravity acceleration"), STR("g_0"), kSIGravityAcceleration, &error);
    AddToLib(kSIQuantityAcceleration, STR("mile per square second"), STR("miles per square second"), STR("mi/s^2"), kSIMile, &error);
    AddToLib(kSIQuantityAcceleration, STR("foot per square second"), STR("feet per square second"), STR("ft/s^2"), kSIFoot, &error);
    AddToLib(kSIQuantityAcceleration, STR("inch per square second"), STR("inches per square second"), STR("in/s^2"), kSIInch, &error);
    AddToLib(kSIQuantityAcceleration, STR("mile per square minute"), STR("miles per square minute"), STR("mi/min^2"), kSIMile / kSIMinute / kSIMinute, &error);
    AddToLib(kSIQuantityAcceleration, STR("foot per square minute"), STR("feet per square minute"), STR("ft/min^2"), kSIFoot / kSIMinute / kSIMinute, &error);
    AddToLib(kSIQuantityAcceleration, STR("inch per square minute"), STR("inches per square minute"), STR("in/min^2"), kSIInch / kSIMinute / kSIMinute, &error);
    AddToLib(kSIQuantityAcceleration, STR("mile per hour per second"), STR("miles per hour per second"), STR("mi/(h•s)"), kSIMile / kSIHour, &error);
    AddToLib(kSIQuantityAcceleration, STR("knot per second"), STR("knots per second"), STR("kn/s"), 0.514444444444444, &error);
    AddToLib(kSIQuantityAcceleration, STR("galileo"), STR("galileo"), STR("Gal"), 1e-2, &error);
//
#pragma mark kSIQuantityDensity
    AddToLibPrefixed(kSIQuantityDensity, STR("gram per cubic meter"), STR("grams per cubic meter"), STR("g/m^3"), 1e-3, &error);
//
#pragma mark kSIQuantityMassFlowRate
    AddToLibPrefixed(kSIQuantityMassFlowRate, STR("gram per second"), STR("grams per second"), STR("g/s"), 1e-3, &error);
//
#pragma mark kSIQuantityMassFlux
    AddToLibPrefixed(kSIQuantityMassFlux, STR("gram per square meter per second"), STR("grams per square meter per second"), STR("g/(m^2•s)"), 1e-3, &error);
//
#pragma mark kSIQuantitySurfaceDensity
    AddToLibPrefixed(kSIQuantitySurfaceDensity, STR("gram per square meter"), STR("grams per square meter"), STR("g/m^2"), 1e-3, &error);
//
#pragma mark kSIQuantityCurrentDensity
    AddToLibPrefixed(kSIQuantityCurrentDensity, STR("ampere per square meter"), STR("amperes per square meter"), STR("A/m^2"), 1, &error);
//
#pragma mark kSIQuantityAmountConcentration
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per cubic meter"), STR("moles per cubic meter"), STR("mol/m^3"), 1., &error);
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per liter"), STR("moles per liter"), STR("mol/L"), 1000., &error);
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per milliliter"), STR("moles per milliliter"), STR("mol/mL"), 1000000., &error);
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per microliter"), STR("moles per microliter"), STR("mol/µL"), 1000000000., &error);
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per liter"), STR("moles per liter"), STR("M"), 1000., &error);
//
#pragma mark kSIQuantityMassConcentration
    AddToLibPrefixed(kSIQuantityMassConcentration, STR("gram per liter"), STR("grams per liter"), STR("g/L"), 1, &error);
    AddToLibPrefixed(kSIQuantityMassConcentration, STR("gram per milliliter"), STR("grams per milliliter"), STR("g/mL"), 1e3, &error);
    AddToLibPrefixed(kSIQuantityMassConcentration, STR("gram per cubic centimeter"), STR("grams per cubic centimeter"), STR("g/cm^3"), 1e3, &error);
    AddToLibPrefixed(kSIQuantityMassConcentration, STR("gram per microliter"), STR("grams per microliter"), STR("g/µL"), 1e6, &error);
//
#pragma mark kSIQuantityForce
    AddToLibPrefixed(kSIQuantityForce, STR("newton"), STR("newtons"), STR("N"), 1, &error);
    AddToLib(kSIQuantityForce, STR("atomic unit of force"), STR("atomic units of force"), STR("E_h/a_0"), E_h / a_0, &error);
    AddToLibPrefixed(kSIQuantityForce, STR("dyne"), STR("dynes"), STR("dyn"), 1e-5, &error);
    AddToLib(kSIQuantityForce, STR("pound force"), STR("pounds force"), STR("lbf"), 4.4482216152605, &error);
    AddToLib(kSIQuantityForce, STR("ounce force"), STR("ounces force"), STR("ozf"), 4.4482216152605 / 16., &error);
    AddToLibPrefixed(kSIQuantityForce, STR("gram force"), STR("grams force"), STR("gf"), 0.00980665, &error);
//
#pragma mark kSIQuantityTorque
    AddToLibPrefixed(kSIQuantityTorque, STR("newton meter per radian"), STR("newton meters per radian"), STR("N•m/rad"), 1., &error);
    AddToLibPrefixed(kSIQuantityTorque, STR("joule per radian"), STR("joules per radian"), STR("J/rad"), 1., &error);
    AddToLib(kSIQuantityTorque, STR("pound force foot per radian"), STR("pound force feet per radian"), STR("lbf•ft/rad"), 1.3558179483314, &error);
    AddToLib(kSIQuantityTorque, STR("pound force inch per radian"), STR("pound force inches per radian"), STR("lbf•in/rad"), 1.3558179483314 / 12., &error);
    AddToLibPrefixed(kSIQuantityTorque, STR("gram force meter per radian"), STR("gram force meters per radian"), STR("gf•m/rad"), 0.00980665, &error);
    AddToLibPrefixed(kSIQuantityTorque, STR("gram force centimeter per radian"), STR("gram force centimeters per radian"), STR("gf•cm/rad"), 9.80665e-05, &error);
//
#pragma mark kSIQuantityMomentOfInertia
    AddToLib(kSIQuantityMomentOfInertia, STR("meter squared kilogram"), STR("meters squared kilogram"), STR("m^2•kg"), 1, &error);
    AddToLib(kSIQuantityMomentOfInertia, STR("meter squared gram"), STR("meters squared gram"), STR("m^2•g"), 1.e-3, &error);
    AddToLib(kSIQuantityMomentOfInertia, STR("centimeter squared kilogram"), STR("centimeter squared kilogram"), STR("cm^2•kg"), 0.0001, &error);
    AddToLib(kSIQuantityMomentOfInertia, STR("centimeter squared gram"), STR("centimeter squared gram"), STR("cm^2•g"), 1.e-7, &error);
//
#pragma mark kSIQuantityPressure, kSIQuantityStress, kSIQuantityElasticModulus
    AddToLibPrefixed(kSIQuantityPressure, STR("pascal"), STR("pascals"), STR("Pa"), 1, &error);
    AddToLib(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("lbf/in^2"), kSIPoundsForce/kSIInch/kSIInch, &error);
    AddToLib(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("psi"), kSIPoundsForce/kSIInch/kSIInch, &error);
    AddToLib(kSIQuantityPressure, STR("pound force per square foot"), STR("pounds force per square feet"), STR("lbf/ft^2"), kSIPoundsForce/kSIFoot/kSIFoot, &error);
    AddToLibPrefixed(kSIQuantityPressure, STR("torr"), STR("torrs"), STR("Torr"), 1.01325e5 / 760, &error);
    AddToLibPrefixed(kSIQuantityPressure, STR("bar"), STR("bars"), STR("bar"), 1e5, &error);
    AddToLib(kSIQuantityPressure, STR("millimeter of Hg"), STR("millimeters of Hg"), STR("mmHg"), 133.322, &error);
    AddToLib(kSIQuantityPressure, STR("atmosphere"), STR("atmospheres"), STR("atm"), 1.01325e5, &error);
    AddToLibPrefixed(kSIQuantityPressure, STR("newton per square meter"), STR("newtons per square meter"), STR("N/m^2"), 1, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityPressure);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityStress, units);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityElasticModulus, units);
//
#pragma mark kSIQuantityCompressibility
    AddToLib(kSIQuantityCompressibility, STR("inverse pascal"), STR("inverse pascals"), STR("1/Pa"), 1, &error);
//
#pragma mark kSIQuantityStressOpticCoefficient
    AddToLib(kSIQuantityStressOpticCoefficient, STR("brewster"), STR("brewsters"), STR("B"), 1.e-12, &error);
//
#pragma mark kSIQuantityPressureGradient
    AddToLibPrefixed(kSIQuantityPressureGradient, STR("pascal per meter"), STR("pascals per meter"), STR("Pa/m"), 1, &error);
    AddToLib(kSIQuantityPressureGradient, STR("pound force per square inch per foot"), STR("pounds force per square inch per foot"), STR("psi/ft"), kSIPoundsForce/kSIInch/kSIInch/kSIFoot, &error);
//
#pragma mark kSIQuantitySpectralRadiantEnergy
    AddToLibPrefixed(kSIQuantitySpectralRadiantEnergy, STR("joule per nanometer"), STR("joules per nanometer"), STR("J/nm"), 1.e9, &error);
//
#pragma mark kSIQuantityPower, kSIQuantityRadiantFlux
    AddToLibPrefixed(kSIQuantityPower, STR("watt"), STR("watts"), STR("W"), 1, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("Joule per second"), STR("Joules per second"), STR("J/s"), 1, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("calorie per second"), STR("calories per second"), STR("cal/s"), kSICal, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("calorie per minute"), STR("calories per minute"), STR("cal/min"), kSICal / kSIMinute, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("calorie per hour"), STR("calories per hour"), STR("cal/h"), kSICal / kSIHour, &error);
    AddToLib(kSIQuantityPower, STR("horsepower"), STR("horsepower"), STR("hp"), kSIhp, &error);
    AddToLib(kSIQuantityPower, STR("british thermal unit per hour"), STR("british thermal unit per hour"), STR("Btu/h"), kSIBtu / kSIHour, &error);
    AddToLib(kSIQuantityPower, STR("british thermal unit per minute"), STR("british thermal unit per minute"), STR("Btu/min"), kSIBtu / kSIMinute, &error);
    AddToLib(kSIQuantityPower, STR("british thermal unit per second"), STR("british thermal unit per second"), STR("Btu/s"), kSIBtu, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("erg per second"), STR("ergs per second"), STR("erg/s"), 1e-7, &error);
    AddToLib(kSIQuantityPower, STR("foot pound force per hour"), STR("feet pound force per hour"), STR("ft•lbf/h"), kSIFoot*kSIPoundsForce / kSIHour, &error);
    AddToLib(kSIQuantityPower, STR("foot pound force per minute"), STR("feet pound force per minute"), STR("ft•lbf/min"), kSIFoot*kSIPoundsForce / kSIMinute, &error);
    AddToLib(kSIQuantityPower, STR("foot pound force per second"), STR("feet pound force per second"), STR("ft•lbf/s"), kSIFoot*kSIPoundsForce, &error);
    AddToLib(kSIQuantityPower, STR("inch pound force per hour"), STR("inches pound force per hour"), STR("in•lbf/h"), kSIInch*kSIPoundsForce / kSIHour, &error);
    AddToLib(kSIQuantityPower, STR("inch pound force per minute"), STR("inches pound force per minute"), STR("in•lbf/min"), kSIInch*kSIPoundsForce / kSIMinute, &error);
    AddToLib(kSIQuantityPower, STR("inch pound force per second"), STR("inches pound force per second"), STR("in•lbf/s"), kSIInch*kSIPoundsForce, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityPower);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityRadiantFlux, units);
//
#pragma mark kSIQuantitySpectralPower
    AddToLibPrefixed(kSIQuantitySpectralPower, STR("watt per nanometer"), STR("watts per nanometer"), STR("W/nm"), 1.e9, &error);
//
#pragma mark kSIQuantityVolumePowerDensity
    AddToLibPrefixed(kSIQuantityVolumePowerDensity, STR("watt per cubic meter"), STR("watts per cubic meter"), STR("W/m^3"), 1, &error);
    AddToLibPrefixed(kSIQuantityVolumePowerDensity, STR("watt per cubic centimeter"), STR("watts per cubic centimeter"), STR("W/cm^3"), 100000., &error);
//
#pragma mark kSIQuantitySpecificPower
    AddToLibPrefixed(kSIQuantitySpecificPower, STR("watt per kilogram"), STR("watts per kilogram"), STR("W/kg"), 1, &error);
    AddToLib(kSIQuantitySpecificPower, STR("horse power per pound"), STR("horse power per pound"), STR("hp/lb"), kSIhp/kSIPound, &error);
    AddToLib(kSIQuantitySpecificPower, STR("horse power per ounce"), STR("horse power per ounce"), STR("hp/oz"), kSIhp/kSIOunce, &error);
//
#pragma mark kSIQuantityElectricCharge, kSIQuantityAmountOfElectricity
    AddToLibPrefixed(kSIQuantityElectricCharge, STR("coulomb"), STR("coulombs"), STR("C"), 1, &error);
    AddToLib(kSIQuantityElectricCharge, STR("elementary charge"), STR("elementary charge"), STR("q_e"), kSIElementaryCharge, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityElectricCharge);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityAmountOfElectricity, units);
//
#pragma mark kSIQuantityElectricPotentialDifference, kSIQuantityElectromotiveForce, kSIQuantityVoltage
    AddToLibPrefixed(kSIQuantityElectricPotentialDifference, STR("volt"), STR("volts"), STR("V"), 1, &error);
    AddToLib(kSIQuantityElectricPotentialDifference, STR("atomic unit of electric potential"), STR("atomic units of electric potential"), STR("E_h/q_e"), E_h / kSIElementaryCharge, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityElectricPotentialDifference);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityElectromotiveForce, units);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityVoltage, units);
//
#pragma mark kSIQuantityElectricFieldGradient
    AddToLibPrefixed(kSIQuantityElectricFieldGradient, STR("volt per square meter"), STR("volts per square meter"), STR("V/m^2"), 1, &error);
    AddToLib(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("Λ_0"), Λ_0, &error);
    AddToLib(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("E_h/(q_e•a_0^2)"), Λ_0, &error);
//
#pragma mark kSIQuantityCapacitance
    AddToLibPrefixed(kSIQuantityCapacitance, STR("farad"), STR("farads"), STR("F"), 1, &error);
//
#pragma mark kSIQuantityElectricResistancePerLength
    AddToLibPrefixed(kSIQuantityElectricResistancePerLength, STR("ohm per meter"), STR("ohms per meter"), STR("Ω/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityElectricResistancePerLength, STR("ohm per feet"), STR("ohms per feet"), STR("Ω/ft"), 1. / (kSIMile / 5280), &error);
//
#pragma mark kSIQuantityElectricResistivity
    AddToLibPrefixed(kSIQuantityElectricResistivity, STR("ohm meter"), STR("ohms meter"), STR("Ω•m"), 1, &error);
    AddToLibPrefixed(kSIQuantityElectricResistivity, STR("ohm centimeter"), STR("ohms centimeter"), STR("Ω•cm"), 0.01, &error);
//
#pragma mark kSIQuantityElectricConductance
    AddToLibPrefixed(kSIQuantityElectricConductance, STR("siemen"), STR("siemens"), STR("S"), 1, &error);
    AddToLib(kSIQuantityElectricConductance, STR("conductance quantum"), STR("conductance quantum"), STR("G_0"), G_0, &error);
//
#pragma mark kSIQuantityElectricConductivity
    AddToLibPrefixed(kSIQuantityElectricConductivity, STR("siemen per meter"), STR("siemens per meter"), STR("S/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityElectricConductivity, STR("siemen per centimeter"), STR("siemens per centimeter"), STR("S/cm"), 100., &error);
//
#pragma mark kSIQuantityMolarConductivity
    AddToLibPrefixed(kSIQuantityMolarConductivity, STR("siemen meter squared per mole"), STR("siemens meter squared per mole"), STR("S•m^2/mol"), 1, &error);
    AddToLibPrefixed(kSIQuantityMolarConductivity, STR("siemen centimeter squared per mole"), STR("siemens centimeter squared per mole"), STR("S•cm^2/mol"), 0.0001, &error);
//
#pragma mark kSIQuantityGyromagneticRatio
    AddToLib(kSIQuantityGyromagneticRatio, STR("radian per second per tesla"), STR("radians per second per tesla"), STR("rad/(s•T)"), 1, &error);
//
#pragma mark kSIQuantityMagneticDipoleMoment
    AddToLibPrefixed(kSIQuantityMagneticDipoleMoment, STR("ampere square meter"), STR("ampere square meters"), STR("A•m^2"), 1, &error);
    AddToLibPrefixed(kSIQuantityMagneticDipoleMoment, STR("joule per tesla"), STR("joules per tesla"), STR("J/T"), 1, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("nuclear magneton"), STR("nuclear magnetons"), STR("µ_N"), mu_N, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("bohr magneton"), STR("bohr magnetons"), STR("µ_B"), mu_e, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("proton magnetic moment"), STR("proton magnetic moment"), STR("µ_p"), kSIProtonMagneticMoment, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("neutron magnetic moment"), STR("neutron magnetic moment"), STR("µ_n"), kSINeutronMagneticMoment, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("electron magnetic moment"), STR("electron magnetic moment"), STR("µ_e"), kSIElectronMagneticMoment, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("muon magnetic moment"), STR("muon magnetic moment"), STR("µ_µ"), kSIMuonMagneticMoment, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("atomic unit of magnetic dipole moment"), STR("atomic units of magnetic dipole moment"), STR("ℏ•q_e/m_e"), hbar * kSIElementaryCharge / kSIElectronMass, &error);
//
#pragma mark kSIQuantityMagneticDipoleMomentRatio
    AddToLib(kSIQuantityMagneticDipoleMomentRatio, STR("proton g factor"), STR("proton g factor"), STR("g_p"), kSIProtonGFactor, &error);
    AddToLib(kSIQuantityMagneticDipoleMomentRatio, STR("neutron g factor"), STR("neutron g factor"), STR("g_n"), kSINeutronGFactor, &error);
    AddToLib(kSIQuantityMagneticDipoleMomentRatio, STR("electron g factor"), STR("electron g factor"), STR("g_e"), kSIElectronGFactor, &error);
    AddToLib(kSIQuantityMagneticDipoleMomentRatio, STR("muon g factor"), STR("muon g factor"), STR("g_µ"), kSIMuonGFactor, &error);
//
#pragma mark kSIQuantityMagneticFlux
    AddToLibPrefixed(kSIQuantityMagneticFlux, STR("weber"), STR("webers"), STR("Wb"), 1, &error);
    AddToLib(kSIQuantityMagneticFlux, STR("magnetic flux quantum"), STR("magnetic flux quantum"), STR("Φ_0"), kSIPlanckConstant / (2 * kSIElementaryCharge), &error);
    AddToLib(kSIQuantityMagneticFlux, STR("maxwell"), STR("maxwells"), STR("Mx"), 1e-8, &error);
//
#pragma mark kSIQuantityMagneticFluxDensity
    AddToLibPrefixed(kSIQuantityMagneticFluxDensity, STR("tesla"), STR("tesla"), STR("T"), 1, &error);
    AddToLib(kSIQuantityMagneticFluxDensity, STR("atomic unit of magnetic flux density"), STR("atomic units of magnetic flux density"), STR("ℏ/(q_e•a_0^2)"), hbar / (kSIElementaryCharge * a_0 * a_0), &error);
    AddToLibPrefixed(kSIQuantityMagneticFluxDensity, STR("gauss"), STR("gauss"), STR("G"), 1e-4, &error);
//
#pragma mark kSIQuantityMagneticFieldGradient
    AddToLibPrefixed(kSIQuantityMagneticFieldGradient, STR("tesla per meter"), STR("tesla per meter"), STR("T/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityMagneticFieldGradient, STR("tesla per centimeter"), STR("tesla per centimeter"), STR("T/cm"), 100., &error);
    AddToLibPrefixed(kSIQuantityMagneticFieldGradient, STR("gauss per centimeter"), STR("gauss per centimeter"), STR("G/cm"), 0.01, &error);
//
#pragma mark kSIQuantityMolarMagneticSusceptibility
    AddToLib(kSIQuantityMolarMagneticSusceptibility, STR("cubic meter per mole"), STR("cubic meters per mole"), STR("m^3/mol"), 1., &error);
    AddToLib(kSIQuantityMolarMagneticSusceptibility, STR("cubic centimeter per mole"), STR("cubic centimeters per mole"), STR("cm^3/mol"), 1e-06, &error);
//
#pragma mark kSIQuantityInductance
    AddToLibPrefixed(kSIQuantityInductance, STR("henry"), STR("henries"), STR("H"), 1, &error);
//
#pragma mark kSIQuantityLuminousFlux
    AddToLibPrefixed(kSIQuantityLuminousFlux, STR("lumen"), STR("lumens"), STR("lm"), 1., &error);
    AddToLibPrefixed(kSIQuantityLuminousFlux, STR("candela steradian"), STR("candela steradians"), STR("cd•sr"), 1, &error);
//
#pragma mark kSIQuantityLuminousFluxDensity
    AddToLibPrefixed(kSIQuantityLuminousFluxDensity, STR("lumen per square meter"), STR("lumens per square meter"), STR("lm/m^2"), 1., &error);
    AddToLibPrefixed(kSIQuantityLuminousFluxDensity, STR("lumen per square foot"), STR("lumens per square foot"), STR("lm/ft^2"), 10.76391041670972, &error);
//
#pragma mark kSIQuantityLuminousEnergy
    AddToLibPrefixed(kSIQuantityLuminousEnergy, STR("lumen second"), STR("lumen seconds"), STR("lm•s"), 1, &error);
//
#pragma mark kSIQuantityIlluminance
    AddToLibPrefixed(kSIQuantityIlluminance, STR("lux"), STR("lux"), STR("lx"), 1, &error);
    AddToLibPrefixed(kSIQuantityIlluminance, STR("phot"), STR("phots"), STR("ph"), 1e4, &error);
//
#pragma mark kSIQuantityAbsorbedDose
    AddToLibPrefixed(kSIQuantityAbsorbedDose, STR("gray"), STR("grays"), STR("Gy"), 1, &error);
//
#pragma mark kSIQuantityDoseEquivalent
    AddToLibPrefixed(kSIQuantityDoseEquivalent, STR("sievert"), STR("sieverts"), STR("Sv"), 1, &error);
//
#pragma mark kSIQuantityCatalyticActivity
    AddToLibPrefixed(kSIQuantityCatalyticActivity, STR("mole per second"), STR("moles per second"), STR("mol/s"), 1, &error);
    AddToLibPrefixed(kSIQuantityCatalyticActivity, STR("katal"), STR("katals"), STR("kat"), 1, &error);
    AddToLibPrefixed(kSIQuantityCatalyticActivity, STR("mole per minute"), STR("moles per minute"), STR("mol/min"), 1. / kSIMinute, &error);
//
#pragma mark kSIQuantityCatalyticActivityConcentration
    AddToLibPrefixed(kSIQuantityCatalyticActivityConcentration, STR("katal per cubic meter"), STR("katals per cubic meter"), STR("kat/m^3"), 1, &error);
    AddToLibPrefixed(kSIQuantityCatalyticActivityConcentration, STR("katal per liter"), STR("katals per liter"), STR("kat/L"), 1000., &error);
//
#pragma mark kSIQuantityCatalyticActivityContent
    AddToLibPrefixed(kSIQuantityCatalyticActivityContent, STR("katal per kilogram"), STR("katals per kilogram"), STR("kat/kg"), 1, &error);
//
#pragma mark kSIQuantityRatePerAmountConcentrationPerTime
    AddToLibPrefixed(kSIQuantityRatePerAmountConcentrationPerTime, STR("liter per mole per second"), STR("liter per mole per second"), STR("L/(mol•s)"), 0.001, &error);
//
#pragma mark kSIQuantityRefractiveIndex
    AddToLibPrefixed(kSIQuantityRefractiveIndex, STR("meter second per meter second"), STR("meter seconds per meter second"), STR("m•s/(m•s)"), 1, &error);
//
#pragma mark kSIQuantityElectricQuadrupoleMoment
    AddToLib(kSIQuantityElectricQuadrupoleMoment, STR("atomic unit of electric quadrupole moment"), STR("atomic units of electric quadrupole moment"), STR("q_e•a_0^2"), kSIElementaryCharge * a_0 * a_0, &error);
//
#pragma mark kSIQuantityMagnetizability
    AddToLib(kSIQuantityMagnetizability, STR("atomic unit of magnetizability"), STR("atomic units of magnetizability"), STR("q_e•a_0^2/m_e"), kSIElementaryCharge * a_0 * a_0 / kSIElectronMass, &error);
//
#pragma mark kSIQuantityPermittivity
    AddToLib(kSIQuantityPermittivity, STR("atomic unit of permittivity"), STR("atomic units of permittivity"), STR("q_e^2/(a_0•E_h)"), kSIElementaryCharge * kSIElementaryCharge / (a_0 * E_h), &error);
    AddToLibPrefixed(kSIQuantityPermittivity, STR("farad per meter"), STR("farads per meter"), STR("F/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityPermittivity, STR("coulomb per volt meter"), STR("coulombs per volt meter"), STR("C/(V•m)"), 1, &error);
    AddToLib(kSIQuantityPermittivity, STR("electric constant"), STR("electric constant"), STR("ε_0"), kSIElectricConstant, &error);
//
#pragma mark kSIQuantityElectricPolarizability
    AddToLib(kSIQuantityElectricPolarizability, STR("atomic unit of electric polarizability"), STR("atomic units of electric polarizability"), STR("q_e^2•a_0^2/E_h"), kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 / (E_h), &error);
//
#pragma mark kSIQuantityFirstHyperPolarizability
    AddToLib(kSIQuantityFirstHyperPolarizability, STR("atomic unit of 1st polarizability"), STR("atomic units of 1st polarizability"), STR("q_e^3•a_0^3/E_h^2"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 / (E_h * E_h), &error);
//
#pragma mark kSIQuantitySecondHyperPolarizability
    AddToLib(kSIQuantitySecondHyperPolarizability, STR("atomic unit of 2nd polarizability"), STR("atomic units of 2nd polarizability"), STR("q_e^4•a_0^4/E_h^3"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 * a_0 / (E_h * E_h * E_h), &error);
//
#pragma mark kSIQuantitySpecificVolume
    AddToLib(kSIQuantitySpecificVolume, STR("cubic meter per kilogram"), STR("cubic meters per kilogram"), STR("m^3/kg"), 1., &error);
//
#pragma mark kSIQuantityMassToChargeRatio
    AddToLib(kSIQuantityMassToChargeRatio, STR("thomson"), STR("thomson"), STR("Th"), kSIAtomicMassConstant / kSIElementaryCharge, &error);
//
#pragma mark kSIQuantityChargeToMassRatio
    AddToLib(kSIQuantityChargeToMassRatio, STR("inverse thomson"), STR("inverse thomson"), STR("(1/Th)"), kSIElementaryCharge / kSIAtomicMassConstant, &error);
//
#pragma mark kSIQuantityDynamicViscosity
    AddToLibPrefixed(kSIQuantityDynamicViscosity, STR("poise"), STR("poises"), STR("P"), 0.1, &error);
    AddToLib(kSIQuantityDynamicViscosity, STR("pascal second"), STR("pascal seconds"), STR("Pa•s"), 1, &error);
    AddToLib(kSIQuantityDynamicViscosity, STR("newton second per square meter"), STR("newton seconds per square meter"), STR("N•s/m^2"), 1, &error);
//
#pragma mark kSIQuantityKinematicViscosity
    AddToLibPrefixed(kSIQuantityKinematicViscosity, STR("stokes"), STR("stokes"), STR("St"), 1e-4, &error);
//
#pragma mark kSIQuantityDiffusionCoefficient
    AddToLib(kSIQuantityDiffusionCoefficient, STR("square meter per second"), STR("square meters per second"), STR("m^2/s"), 1, &error);
    AddToLib(kSIQuantityDiffusionCoefficient, STR("square centimeter per second"), STR("square centimeters per second"), STR("cm^2/s"), 0.0001, &error);
    AddToLib(kSIQuantityDiffusionCoefficient, STR("square millimeter per second"), STR("square millimeters per second"), STR("mm^2/s"), 1e-6, &error);
//
#pragma mark kSIQuantityLuminance
    AddToLibPrefixed(kSIQuantityLuminance, STR("stilb"), STR("stilbs"), STR("sb"), 1e4, &error);
    AddToLibPrefixed(kSIQuantityLuminance, STR("lumen per square meter per steradian"), STR("lumens per square meter per steradian"), STR("lm/(m^2•sr)"), 1, &error);
//
#pragma mark kSIQuantityInverseMagneticFluxDensity
    AddToLib(kSIQuantityInverseMagneticFluxDensity, STR("inverse gauss"), STR("inverse gauss"), STR("(1/G)"), 1., &error);
//
#pragma mark kSIQuantityMagneticFieldStrength
    AddToLibPrefixed(kSIQuantityMagneticFieldStrength, STR("ampere per meter"), STR("amperes per meter"), STR("A/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityMagneticFieldStrength, STR("œrsted"), STR("œrsteds"), STR("Oe"), 79.577471545947674, &error);
//
#pragma mark kSIQuantityMomentOfForce
    AddToLibPrefixed(kSIQuantityMomentOfForce, STR("newton meter"), STR("newton meters"), STR("N•m"), 1, &error);
    AddToLib(kSIQuantityMomentOfForce, STR("foot pound force"), STR("feet pound force"), STR("ft•lbf"), 1.3558179483314, &error);
    AddToLib(kSIQuantityMomentOfForce, STR("inch pound force"), STR("inch pound force"), STR("in•lbf"), 1.3558179483314 / 12., &error);
    AddToLib(kSIQuantityMomentOfForce, STR("inch ounce force"), STR("inch ounce force"), STR("in•ozf"), 1.3558179483314 / 12. / 16., &error);
    AddToLib(kSIQuantityMomentOfForce, STR("pound force foot"), STR("pound force feet"), STR("lbf•ft"), 1.3558179483314, &error);
    AddToLib(kSIQuantityMomentOfForce, STR("pound force inch"), STR("pound force inches"), STR("lbf•in"), 1.3558179483314 / 12., &error);
    AddToLib(kSIQuantityMomentOfForce, STR("ounce force inch"), STR("ounce force inches"), STR("ozf•in"), 1.3558179483314 / 12. / 16., &error);
//
#pragma mark kSIQuantitySurfaceTension
    AddToLibPrefixed(kSIQuantitySurfaceTension, STR("newton per meter"), STR("newtons per meter"), STR("N/m"), 1, &error);
    AddToLibPrefixed(kSIQuantitySurfaceTension, STR("dyne per centimeter"), STR("dynes per centimeter"), STR("dyn/cm"), 0.001, &error);
//
#pragma mark kSIQuantitySurfaceEnergy
    AddToLibPrefixed(kSIQuantitySurfaceEnergy, STR("joule per square meter"), STR("joules per square meter"), STR("J/m^2"), 1, &error);
    AddToLibPrefixed(kSIQuantitySurfaceEnergy, STR("dyne per square centimeter"), STR("dynes per square centimeter"), STR("dyn/cm^2"), 0.1, &error);
//
#pragma mark kSIQuantityAngularSpeed, kSIQuantityAngularVelocity
    AddToLib(kSIQuantityAngularSpeed, STR("radian per second"), STR("radians per second"), STR("rad/s"), 1, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityAngularSpeed);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityAngularVelocity, units);
//
#pragma mark kSIQuantityAngularAcceleration
    AddToLib(kSIQuantityAngularAcceleration, STR("radian per square second"), STR("radians per square second"), STR("rad/s^2"), 1, &error);
//
#pragma mark kSIQuantityHeatFluxDensity, kSIQuantityIrradiance
    AddToLibPrefixed(kSIQuantityHeatFluxDensity, STR("watt per square meter"), STR("watts per square meter"), STR("W/m^2"), 1, &error);
    AddToLibPrefixed(kSIQuantityHeatFluxDensity, STR("watt per square centimeter"), STR("watts per square centimeter"), STR("W/cm^2"), 10000., &error);
    AddToLibPrefixed(kSIQuantityHeatFluxDensity, STR("watt per square foot"), STR("watts per square foot"), STR("W/ft^2"), 10.76391041670972, &error);
    AddToLibPrefixed(kSIQuantityHeatFluxDensity, STR("watt per square inch"), STR("watts per square inch"), STR("W/in^2"), 10.76391041670972 / 12., &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityHeatFluxDensity);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityIrradiance, units);
//
#pragma mark kSIQuantitySpectralRadiantFluxDensity
    AddToLibPrefixed(kSIQuantitySpectralRadiantFluxDensity, STR("watt per square meter per nanometer"), STR("watts per square meter per nanometer"), STR("W/(m^2•nm)"), 1.e9, &error);
//
#pragma mark kSIQuantityEntropy, kSIQuantityHeatCapacity
    AddToLibPrefixed(kSIQuantityEntropy, STR("joule per kelvin"), STR("joules per kelvin"), STR("J/K"), 1, &error);
    AddToLib(kSIQuantityEntropy, STR("boltzmann constant"), STR("boltzmann constant"), STR("k_B"), kSIBoltmannConstant, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityEntropy);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityHeatCapacity, units);
//
#pragma mark kSIQuantitySpecificHeatCapacity, kSIQuantitySpecificEntropy
    AddToLibPrefixed(kSIQuantitySpecificHeatCapacity, STR("joule per kilogram kelvin"), STR("joules per kilogram kelvin"), STR("J/(kg•K)"), 1, &error);
    AddToLibPrefixed(kSIQuantitySpecificHeatCapacity, STR("joule per gram kelvin"), STR("joules per gram kelvin"), STR("J/(g•K)"), 1000., &error);
    AddToLibPrefixed(kSIQuantitySpecificHeatCapacity, STR("calorie per gram per kelvin"), STR("calories per gram per kelvin"), STR("cal/(g•K)"), 4186.8, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantitySpecificHeatCapacity);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantitySpecificEntropy, units);
//
#pragma mark kSIQuantityMolarMass
    AddToLibPrefixed(kSIQuantityMolarMass, STR("gram per mole"), STR("grams per mole"), STR("g/mol"), 1e-3, &error);
//
#pragma mark kSIQuantityMolality
    AddToLibPrefixed(kSIQuantityMolality, STR("mole per kilogram"), STR("moles per kilogram"), STR("mol/kg"), 1, &error);
//
#pragma mark kSIQuantitySpecificEnergy
    AddToLibPrefixed(kSIQuantitySpecificEnergy, STR("joule per kilogram"), STR("joules per kilogram"), STR("J/kg"), 1, &error);
    AddToLibPrefixed(kSIQuantitySpecificEnergy, STR("joule per gram"), STR("joules per gram"), STR("J/g"), 1e3, &error);
//
#pragma mark kSIQuantityThermalConductance
    AddToLibPrefixed(kSIQuantityThermalConductance, STR("watt per kelvin"), STR("watts per kelvin"), STR("W/K"), 1, &error);
    AddToLib(kSIQuantityThermalConductance, STR("Btu per hour per rankine"), STR("Btus per hour per rankine"), STR("Btu/(h•°R)"), 0.5275279262867396, &error);
    AddToLibPrefixed(kSIQuantityThermalConductance, STR("calorie per hour per kelvin"), STR("calories per hour per kelvin"), STR("cal/(h•K)"), 1.163e-3, &error);
//
#pragma mark kSIQuantityThermalConductivity
    AddToLibPrefixed(kSIQuantityThermalConductivity, STR("watt per meter kelvin"), STR("watts per meter kelvin"), STR("W/(m•K)"), 1, &error);
    AddToLib(kSIQuantityThermalConductivity, STR("Btu per hour per foot per rankine"), STR("Btus per hour per foot per rankine"), STR("Btu/(h•ft•°R)"), 1.730734666295077, &error);
    AddToLibPrefixed(kSIQuantityThermalConductivity, STR("calorie per hour per meter per kelvin"), STR("calories per hour per meter per kelvin"), STR("cal/(h•m•K)"), 1.163e-3, &error);
//
#pragma mark kSIQuantityEnergyDensity
    AddToLibPrefixed(kSIQuantityEnergyDensity, STR("joule per cubic meter"), STR("joules per cubic meter"), STR("J/m^3"), 1, &error);
    AddToLibPrefixed(kSIQuantityEnergyDensity, STR("joule per liter"), STR("joules per liter"), STR("J/L"), 1000., &error);
//
#pragma mark kSIQuantityElectricDipoleMoment
    AddToLibPrefixed(kSIQuantityElectricDipoleMoment, STR("coulomb meter"), STR("coulomb meters"), STR("C•m"), 1, &error);
    AddToLib(kSIQuantityElectricDipoleMoment, STR("debye"), STR("debyes"), STR("D"), 3.335640951816991e-30, &error);
    AddToLib(kSIQuantityElectricDipoleMoment, STR("atomic unit of electric dipole moment"), STR("atomic unit of electric dipole moment"), STR("q_e•a_0"), kSIElementaryCharge * a_0, &error);
//
#pragma mark kSIQuantityElectricFieldStrength
    AddToLibPrefixed(kSIQuantityElectricFieldStrength, STR("volt per meter"), STR("volts per meter"), STR("V/m"), 1, &error);
    AddToLib(kSIQuantityElectricFieldStrength, STR("atomic unit of electric field"), STR("atomic unit of electric field"), STR("E_h/(q_e•a_0)"), E_h / (kSIElementaryCharge * a_0), &error);
    AddToLibPrefixed(kSIQuantityElectricFieldStrength, STR("volt per centimeter"), STR("volts per centimeter"), STR("V/cm"), 1, &error);
    AddToLibPrefixed(kSIQuantityElectricFieldStrength, STR("newton per coulomb"), STR("newtons per coulomb"), STR("N/C"), 1, &error);
//
#pragma mark kSIQuantityElectricFlux
    AddToLibPrefixed(kSIQuantityElectricFlux, STR("volt meter"), STR("volts meter"), STR("V•m"), 1, &error);
//
#pragma mark kSIQuantityElectricChargeDensity
    AddToLibPrefixed(kSIQuantityElectricChargeDensity, STR("coulomb per cubic meter"), STR("coulombs per cubic meter"), STR("C/m^3"), 1, &error);
    AddToLib(kSIQuantityElectricChargeDensity, STR("charge density"), STR("charge density"), STR("A•h/L"), 3600000, &error);
    AddToLib(kSIQuantityElectricChargeDensity, STR("atomic unit of charge density"), STR("atomic unit of charge density"), STR("q_e/a_0^3"), kSIElementaryCharge / (a_0 * a_0 * a_0), &error);
//
#pragma mark kSIQuantitySurfaceChargeDensity, kSIQuantityElectricFluxDensity, kSIQuantityElectricDisplacement
    AddToLibPrefixed(kSIQuantitySurfaceChargeDensity, STR("coulomb per square meter"), STR("coulombs per square meter"), STR("C/m^2"), 1, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantitySurfaceChargeDensity);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityElectricFluxDensity, units);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityElectricDisplacement, units);
//
#pragma mark kSIQuantityPermeability
    AddToLibPrefixed(kSIQuantityPermeability, STR("henry per meter"), STR("henries per meter"), STR("H/m"), 1., &error);
    AddToLibPrefixed(kSIQuantityPermeability, STR("newton per square ampere"), STR("newtons per square ampere"), STR("N/A^2"), 1, &error);
    AddToLibPrefixed(kSIQuantityPermeability, STR("tesla meter per ampere"), STR("tesla meter per ampere"), STR("T•m/A"), 1, &error);
    AddToLibPrefixed(kSIQuantityPermeability, STR("weber per ampere meter"), STR("webers per ampere meter"), STR("Wb/(A•m)"), 1, &error);
    AddToLib(kSIQuantityPermeability, STR("magnetic constant"), STR("magnetic constant"), STR("µ_0"), 4 * kSIPi * 1.e-7, &error);
//
#pragma mark kSIQuantityMolarEntropy, kSIQuantityMolarHeatCapacity
    AddToLibPrefixed(kSIQuantityMolarEntropy, STR("joule per mole kelvin"), STR("joules per mole kelvin"), STR("J/(mol•K)"), 1, &error);
    AddToLib(kSIQuantityMolarEntropy, STR("gas constant"), STR("gas constant"), STR("R"), kSIBoltmannConstant * kSIAvogadroConstant, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityMolarEntropy);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityMolarHeatCapacity, units);
//
#pragma mark kSIQuantityMolarEnergy
    AddToLibPrefixed(kSIQuantityMolarEnergy, STR("joule per mole"), STR("joules per mole"), STR("J/mol"), 1, &error);
    AddToLibPrefixed(kSIQuantityMolarEnergy, STR("calorie per mole"), STR("calories per mole"), STR("cal/mol"), kSICal, &error);
//
#pragma mark kSIQuantityRadiationExposure
    AddToLibPrefixed(kSIQuantityRadiationExposure, STR("coulomb per kilogram"), STR("coulombs per kilogram"), STR("C/kg"), 1, &error);
//
#pragma mark kSIQuantityAbsorbedDoseRate
    AddToLibPrefixed(kSIQuantityAbsorbedDoseRate, STR("gray per second"), STR("grays per second"), STR("Gy/s"), 1, &error);
//
#pragma mark kSIQuantityRadiantIntensity
    AddToLibPrefixed(kSIQuantityRadiantIntensity, STR("watt per steradian"), STR("watts per steradian"), STR("W/sr"), 1, &error);
//
#pragma mark kSIQuantitySpectralRadiantIntensity
    AddToLibPrefixed(kSIQuantitySpectralRadiantIntensity, STR("watt per steradian per nanometer"), STR("watts per steradian per nanometer"), STR("W/(sr•nm)"), 1.e9, &error);
//
#pragma mark kSIQuantityRadiance
    AddToLibPrefixed(kSIQuantityRadiance, STR("watt per square meter per steradian"), STR("watts per square meter per steradian"), STR("W/(m^2•sr)"), 1, &error);
//
#pragma mark kSIQuantitySpectralRadiance
    AddToLibPrefixed(kSIQuantitySpectralRadiance, STR("watt per square meter per steradian per nanometer"), STR("watts per square meter steradian per nanometer"), STR("W/(m^2•sr•nm)"), 1.e9, &error);
//
#pragma mark kSIQuantityFrequencyPerMagneticFluxDensity
    AddToLibPrefixed(kSIQuantityFrequencyPerMagneticFluxDensity, STR("hertz per tesla"), STR("hertz per tesla"), STR("Hz/T"), 1, &error);
//
#pragma mark kSIQuantityLengthPerVolume
    AddToLibPrefixed(kSIQuantityLengthPerVolume, STR("meter per liter"), STR("meters per liter"), STR("m/L"), 1. / 1.e-3, &error);
//
#pragma mark kSIQuantityPowerPerLuminousFlux
    AddToLibPrefixed(kSIQuantityPowerPerLuminousFlux, STR("watt per lumen"), STR("watts per lumen"), STR("W/lm"), 1, &error);
//
#pragma mark kSIQuantityLuminousEfficacy
    AddToLibPrefixed(kSIQuantityLuminousEfficacy, STR("lumen per watt"), STR("lumens per watt"), STR("lm/W"), 1, &error);
//
#pragma mark kSIQuantityHeatTransferCoefficient
    AddToLibPrefixed(kSIQuantityHeatTransferCoefficient, STR("watt per square meter per kelvin"), STR("watts per square meter per kelvin"), STR("W/(m^2•K)"), 1, &error);
    AddToLib(kSIQuantityHeatTransferCoefficient, STR("Btu per hour per square foot per rankine"), STR("Btus per hour per square foot per rankine"), STR("Btu/(h•ft^2•°R)"), 5.678263340863113, &error);
    AddToLibPrefixed(kSIQuantityHeatTransferCoefficient, STR("calorie per hour per square meter per kelvin"), STR("calories per hour per square meter per kelvin"), STR("cal/(h•m^2•K)"), 1.163e-3, &error);
//
#pragma mark kSIQuantityChargePerAmount
    AddToLib(kSIQuantityChargePerAmount, STR("faraday constant"), STR("faraday constant"), STR("&F"), kSIElementaryCharge * kSIAvogadroConstant, &error);
    AddToLibPrefixed(kSIQuantityChargePerAmount, STR("coulomb per mole"), STR("coulombs per mole"), STR("C/mol"), 1.0, &error);
//
#pragma mark kSIQuantityGravitationalConstant
    AddToLib(kSIQuantityGravitationalConstant, STR("gravitational constant"), STR("gravitational constant"), STR("&G"), kSIGravitationalConstant, &error);
//
#pragma mark kSIQuantityVolumePerLength
    AddToLibPrefixed(kSIQuantityVolumePerLength, STR("liter per 100 kilometers"), STR("liters per 100 kilometers"), STR("L/(100 km)"), 1e-3 / 100000., &error);
//
#pragma mark kSIQuantityVolumetricFlowRate
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic meter per hour"), STR("cubic meters per hour"), STR("m^3/h"), 1. / kSIHour, &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic meter per minute"), STR("cubic meters per minute"), STR("m^3/min"), 1. / kSIMinute, &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic meter per second"), STR("cubic meters per second"), STR("m^3/s"), 1., &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per hour"), STR("cubic centimeters per hour"), STR("cm^3/h"), 1e-6 / kSIHour, &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per minute"), STR("cubic centimeters per minute"), STR("cm^3/min"), 1e-6 / kSIMinute, &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per second"), STR("cubic centimeters per second"), STR("cm^3/s"), 1e-6, &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic foot per hour"), STR("cubic feet per hour"), STR("ft^3/h"), (kSIMile / 5280) * (kSIMile / 5280) * (kSIMile / 5280) / kSIHour, &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic foot per minute"), STR("cubic feet per minute"), STR("ft^3/min"), (kSIMile / 5280) * (kSIMile / 5280) * (kSIMile / 5280) / kSIMinute, &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic foot per second"), STR("cubic feet per second"), STR("ft^3/s"), (kSIMile / 5280) * (kSIMile / 5280) * (kSIMile / 5280), &error);
//
#pragma mark kSIQuantityPowerPerAreaPerTemperatureToFourthPower
    AddToLib(kSIQuantityPowerPerAreaPerTemperatureToFourthPower, STR("stefan-boltzmann constant"), STR("stefan-boltzmann constant"), STR("σ"), kSIStefanBoltzmannConstant, &error);
//
#pragma mark kSIQuantityWavelengthDisplacementConstant
    AddToLib(kSIQuantityWavelengthDisplacementConstant, STR("wien wavelength displacement constant"), STR("wien wavelength displacement constant"), STR("b_λ"), kSIWeinDisplacementConstant, &error);
//
#pragma mark kSIQuantityGasPermeance
    AddToLib(kSIQuantityGasPermeance, STR("gas permeance unit"), STR("gas permeance unit"), STR("GPU"), 3.35e-10, &error);
