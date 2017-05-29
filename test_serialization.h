#include "reflect-lib/cereal.h"

#include "test_types.h"

REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(Derived1, Base)
REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(Derived2, Base)

