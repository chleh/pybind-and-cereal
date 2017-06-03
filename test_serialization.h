#include "reflect-lib/cereal.h"

#include "test_types.h"

REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((Derived1), (Base))
REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((Derived2), (Base))

// using D3id = Derived3<int, double>;
// REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((D3id), (Base))
REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((Derived3<int, double>), (Base))

