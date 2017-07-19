#include "reflect-lib/cereal.h"

#include "test/types/test_types.h"

namespace types_one
{
DEFINE_CEREAL_SAVE_LOAD_FUNCTIONS
}  // namespace types_one

REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((types_one::Derived1),
                                            (types_one::Base))
REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((types_one::Derived2),
                                            (types_one::Base))

REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION((types_one::Derived3<int, double>),
                                            (types_one::Base))
