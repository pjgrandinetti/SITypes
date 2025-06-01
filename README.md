# SITypes

Physical quantities appear in nearly every aspect of our daily lives. An athlete checks her wristwatch to measure the physical quantity of time elapsed during a race. A doctor measures the physical quantity of temperature inside a patient's mouth and the physical quantities of pressure: systolic and diastolic. A merchant uses a scale to measure the physical quantity of mass for a piece of cheese.

A physical quantity is not just a numerical value, but also includes another essential attribute: a unit. Surprisingly, many scientific software libraries ignore this essential attribute, while others simply relegate it into meta-data, leaving the burden or tracking units and dimensionality to the programmer.

SITypes is a C library for the physical sciences designed from the ground up with intelligent SI unit and dimensionality types accompanying the numerical value of every physical quantity.  This is critically important in today's society where physical quantities must also be transmitted between software packages and across networks to various types of electronic devices without losing any of their essential attributes.  While there is enormous diversity in how quantities are used in the physical sciences, there is a core set of requirements needed to handle the majority of these applications, and it is for this majority that the SITypes library is designed.

## Release v0.1.0 (Alpha)

> **WARNING: This is an early, alpha-quality release.**
> Use with caution—APIs may change in future versions. Feedback and contributions are very welcome!

### What's new in v0.1.0

- Initial proof-of-concept SI-unit library for C, built on top of OCTypes
- Core types:
  - `SIScalar` (immutable/mutable)
  - `SIUnit` and `SIDimensionality`
  - `SIQuantity` for pairing scalars + units
- Basic arithmetic: add, subtract, multiply, divide, exponentiation
- Human-readable parser for quantities (e.g. “9.8 m/s^2”, “4.3 eV”)
- Dimensionality checks and unit reduction
- Complex-value support for real + imaginary scalars
- OCType-based memory management (`OCRelease`, `OCRetain`)
- Example Makefile and CMake integration
