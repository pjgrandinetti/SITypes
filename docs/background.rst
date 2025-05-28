Background
==========

Physical quantities appear in nearly every aspect of our daily lives. An athlete checks her wristwatch to measure the physical quantity of time elapsed during a race. A doctor employs a thermometer to measure the physical quantity of temperature inside a patient’s mouth. A merchant uses a scale to measure the physical quantity of mass for a piece of cheese. A physical quantity is not just a numerical value but also needs to include another essential attribute: a unit. Additionally, physical quantities can be scalars, vectors, matrices, and higher-dimensional objects. With the inclusion of units, a physical quantity acquires additional behaviors during mathematical operations that a numerical value alone would not have. For example, a physical quantity with a given dimensionality can only be converted between units having the same dimensionality. Likewise, no physical quantities can be added or subtracted unless they share the same dimensionality. Similarly, the arguments of transcendental functions (i.e., :math:`\sin(x)`, :math:`\cos(x)`, :math:`\ln(x)`, :math:`\exp(x)`, ...) must be dimensionless quantities.

An essential characteristic of physical quantities is that any given physical quantity can be derived from other physical quantities through physical laws. For example, the physical quantity of speed is calculated as a ratio of distance traveled to the time elapsed. The volume of a box is calculated as the product of three quantities of length: height, width, and depth. Any physical quantity can be related through physical laws to a smaller set of reference physical quantities. Joseph Fourier originally proposed this idea in his 1822 book *Théorie analytique de la chaleur* (The Analytic Theory of Heat). As the laws of physics become unified, it has been argued that this smaller set can be reduced to simply the Planck length and the speed of light. At the level of theory employed by most scientists and engineers, however, there is a practical agreement that seven physical quantities should serve as fundamental reference quantities from which all other physical quantities can be derived. These reference quantities are:

- length
- mass
- time
- electric current
- thermodynamic temperature [1]_
- amount of substance
- luminous intensity

.. [1] Thermodynamic temperature is an absolute measure of temperature.

Dimensionality
==============

In the International System of Units (SI), seven reference quantities are used to define seven dimensions whose symbols are given in Table 1.

.. table:: Dimension symbols for the seven reference quantities.
   :name: tb:dimensions

   +--------------------------+--------------------+
   | Reference Quantity       | Dimension Symbol   |
   +==========================+====================+
   | length                   | ``L``              |
   +--------------------------+--------------------+
   | mass                     | ``M``              |
   +--------------------------+--------------------+
   | time                     | ``T``              |
   +--------------------------+--------------------+
   | electric current         | ``I``              |
   +--------------------------+--------------------+
   | thermodynamic temperature| ``Θ``              |
   +--------------------------+--------------------+
   | amount of substance      | ``N``              |
   +--------------------------+--------------------+
   | luminous intensity       | ``J``              |
   +--------------------------+--------------------+

The dimensionality of any physical quantity, ``q``, can then be expressed in terms of these seven reference dimensions in the form of a dimensional product:

.. math::

   \dim q = L^{\alpha} \; M^{\beta} \; T^{\gamma}
           \; I^{\delta} \; \Theta^{\epsilon}
           \; N^{\zeta} \; J^{\eta}

where the lower-case Greek symbols represent integers called the dimensional exponents. The dimensionality of any physical quantity can be represented as a point in the space of dimensional exponents ``(\alpha, \beta, \gamma, \delta, \epsilon, \zeta, \eta)``. Physical quantities with different meanings can have the same dimensionality. For example, the thermodynamic quantities *entropy* and *heat capacity* are different physical quantities having the same physical dimensions. Only physical quantities with the same dimensionality can be added. With the operation of multiplication, the physical dimensions form a mathematical group.

There also exists numerous dimensionless quantities of importance, such as *plane angle* which has dimensionality of ``L/L``, or *solid angle* with a dimensionality of ``L^2/L^2``. It is sometimes necessary to distinguish between these different types of dimensionless quantities. Representing dimensionless dimensionalities requires us to split each dimension exponent into numerator and denominator exponents. Thus, we redefine our set of dimensionalities to incorporate numerator and denominator exponents,

.. math::
   :label: eq:dimensionalitySymbol

   \dim q
   = \left(\frac{L^{\alpha_+}}{L^{\alpha_-}}\right)
     \cdot
     \left(\frac{M^{\beta_+}}{M^{\beta_-}}\right)
     \cdot
     \left(\frac{T^{\gamma_+}}{T^{\gamma_-}}\right)
     \cdot
     \left(\frac{I^{\delta_+}}{I^{\delta_-}}\right)
     \cdot
     \left(\frac{\Theta^{\epsilon_+}}{\Theta^{\epsilon_-}}\right)
     \cdot
     \left(\frac{N^{\zeta_+}}{N^{\zeta_-}}\right)
     \cdot
     \left(\frac{J^{\eta_+}}{J^{\eta_-}}\right)

A list of accepted physical quantity names and their corresponding dimensionality symbols are given in Table 2. When no accepted physical quantity name exists for a dimensionality, the symbol obtained from :eq:`eq:dimensionalitySymbol` can be used as the quantity name.


Unit
====

Inherent in measuring any physical quantity is a comparison to a previous measurement. What is most useful is the ratio of the new measurement to a previous measurement. For example, an ancient scientist might have used the cubit, an ancient unit of length, to measure the ratio of a large tree trunk’s circumference to its diameter. Around 3000 BC in Egypt, a cubit was decreed to be the length of a forearm and hand. A scientist could then make a string of one cubit in length using the distance from the back of their elbow to the tip of their middle finger and use that string to measure the ratio

.. math::

   \frac{l_{\text{circumference}}}{l_{\text{diameter}}} \approx 3.14.

While another scientist with longer arms might have cut a longer string to be a cubit, the procedure for finding the ratio of a large tree trunk’s circumference to its diameter is the same, and the result is independent of the absolute length of the string used (i.e., independent of the units of length used).

Thus, we begin by representing a physical quantity, ``q``, using the notation

.. math::

   q = \{q\} \cdot [q]

where ``\{q\}`` is the numerical value and ``[q]`` is the reference unit symbol, i.e., a non-numerical string of characters, usually an abbreviation for the name of the unit.
