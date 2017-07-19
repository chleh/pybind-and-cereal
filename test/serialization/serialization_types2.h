#include "reflect-lib/cereal.h"

#include "test/test_types.h"

REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((Derived3<std::string, double>),
                                            (Base))
