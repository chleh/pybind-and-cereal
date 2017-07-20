# What is tested?
# ===============
#
# test_save saves some objects to an XML archive.
# test_load loads the objects from the XML archive.
#
# In addition to test_save, test_load contains code for
# loading instances of Derived3<std::string, double>.
# That shall test if one is able to load archives if
# subsequently new derived types have been introduced
# into the program.

set -e -x
./test_save
./test_load
