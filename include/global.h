#include <tuple>
#include <string>
using uint = unsigned int;

#define DECL_EQ_DELETE = delete

#define DISABLE_COPY(Class) \
    Class(const Class &) DECL_EQ_DELETE;\
    Class &operator=(const Class &) DECL_EQ_DELETE;

using mto10_param_t = std::tuple<std::string, uint16_t, int, std::string, std::string, int, int, int, int>;