/*
 * This file is part of AdaptiveCpp, an implementation of SYCL and C++ standard
 * parallelism for CPUs and GPUs.
 *
 * Copyright The AdaptiveCpp Contributors
 *
 * AdaptiveCpp is released under the BSD 2-Clause "Simplified" License.
 * See file LICENSE in the project root for full license details.
 */
// SPDX-License-Identifier: BSD-2-Clause
#ifndef HIPSYCL_EXCEPTION_HPP
#define HIPSYCL_EXCEPTION_HPP

#include <memory>
#include <stdexcept>
#include <exception>
#include <functional>
#include <string>
#include <system_error>

#include "hipSYCL/runtime/error.hpp"
#include "hipSYCL/sycl/info/queue.hpp"
#include "types.hpp"
#include "libkernel/backend.hpp"

namespace hipsycl {  
namespace sycl {
namespace detail {
struct sycl_category : std::error_category {
  const char* name() const noexcept override { return "sycl"; }
  std::string message(int) const override { return "hipSYCL Error"; }
};
} // namespace detail

class context;

enum class errc : unsigned int {
  success = 0,
  runtime,
  kernel,
  accessor,
  nd_range,
  event,
  kernel_argument,
  build,
  invalid,
  memory_allocation,
  platform,
  profiling,
  feature_not_supported,
  kernel_not_supported,
  backend_mismatch
};

inline const std::error_category &sycl_category() noexcept {
  static const detail::sycl_category _sycl_category;
  return _sycl_category;
}
  
inline std::error_code make_error_code(errc e) noexcept {
  return {static_cast<int>(e), sycl_category()};
}
  
using async_handler = std::function<void(sycl::exception_list)>;
  
class exception : public virtual std::exception {
public:
  exception() = default;

  exception(std::error_code ec, const std::string& what_arg)
    : error_code{ec}, _msg{what_arg} {}

  exception(std::error_code ec, const char* what_arg)
    : error_code{ec}, _msg{what_arg} {}

  exception(std::error_code ec)
    : error_code{ec} {}

  exception(int ev, const std::error_category& ecat,
            const std::string& what_arg)
    : error_code{ev, ecat}, _msg{what_arg} {}

  exception(int ev, const std::error_category& ecat, const char* what_arg)
    : error_code{ev, ecat}, _msg{what_arg} {}

  exception(int ev, const std::error_category& ecat)
    : error_code{ev, ecat} {}

  // Defined in context.hpp
  exception(context ctx, std::error_code ec, const std::string& what_arg);
  exception(context ctx, std::error_code ec, const char* what_arg);
  exception(context ctx, std::error_code ec);
  exception(context ctx, int ev, const std::error_category& ecat,
            const std::string& what_arg);
  exception(context ctx, int ev, const std::error_category& ecat,
            const char* what_arg);
  exception(context ctx, int ev, const std::error_category& ecat);

  const std::error_code& code() const noexcept {
    return error_code;
  }
  
  const std::error_category& category() const noexcept {
    return error_code.category();
  }

  const char* what() const noexcept override {
    return _msg.c_str();
  }

  bool has_context() const noexcept {
    return (_context != nullptr);
  }

  // Defined in context.hpp
  context get_context() const;

private:
  std::shared_ptr<context> _context;
  std::error_code error_code;
  string_class _msg;
};

} // namespace sycl
} // namespace hipsycl

namespace std {
template <> struct is_error_code_enum<hipsycl::sycl::errc> : true_type {};
} // namespace std

#endif
