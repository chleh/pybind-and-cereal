#include "reflect-lib/cereal.h"

#include "test/test_types.h"

REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((Derived1), (Base))
REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((Derived2), (Base))

REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((Derived3<int, double>), (Base))
