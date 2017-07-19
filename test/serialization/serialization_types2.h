#include "reflect-lib/cereal.h"

#include "test/types/test_types.h"

#if 0
namespace types_one
{
DEFINE_CEREAL_SAVE_LOAD_FUNCTIONS
}  // namespace types_one
#endif

REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(
    (types_one::Derived3<std::string, double>), (types_one::Base))
