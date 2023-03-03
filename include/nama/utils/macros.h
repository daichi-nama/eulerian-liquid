#pragma once

#include <nama/utils/logger.h>

#include <cassert>

#ifdef NDEBUG
#    define NAMA_ASSERT(condition, message) \
        { ; }
#else
#    define NAMA_ASSERT(condition, message)                                                 \
        !(condition)                                                                        \
            ? (nama::utils::Logger::error("Assertion", #condition),                         \
               nama::utils::Logger::error("failed in", __FILE__),                           \
               nama::utils::Logger::error("Line", __LINE__), utils::Logger::error(message), \
               std::cerr << message << std::endl, assert(condition), 0)                     \
            : (0)
#endif

#define NAMA_NOT_IMPLEMENTED                                                                      \
    do {                                                                                          \
        throw std::runtime_error(std::string("Function '") + __func__ + "' is not implemented!"); \
    } while (0)
