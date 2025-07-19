SITypes
=======

A lightweight library for working with physical quantities in C.

A physical quantity is not just a numerical value, but also includes another essential attribute: a unit. Surprisingly, many scientific software libraries ignore this essential attribute, while others simply relegate it into meta-data, leaving the burden or tracking units and dimensionality to the programmer.

SITypes is a C library for the physical sciences designed from the ground up with intelligent SI unit and dimensionality types accompanying the numerical value of every physical quantity.  This is critically important in today's society where physical quantities must also be transmitted between software packages and across networks to various types of electronic devices without losing any of their essential attributes.  While there is enormous diversity in how quantities are used in the physical sciences, there is a core set of requirements needed to handle the majority of these applications, and it is for this majority that the SITypes library is designed. 

.. toctree::
   :maxdepth: 1
   :caption: Background

   background

.. toctree::
   :maxdepth: 1
   :caption: API Reference

   api/SIScalar
   api/SIScalarConstants
   api/SIQuantity
   api/SIUnit
   api/SIDimensionality

Indices and Tables
==================

* :ref:`genindex`
* :ref:`search`
