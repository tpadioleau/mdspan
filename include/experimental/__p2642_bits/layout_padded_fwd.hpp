//@HEADER
// ************************************************************************
//
//                        Kokkos v. 4.0
//       Copyright (2022) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Part of Kokkos, under the Apache License v2.0 with LLVM Exceptions.
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//@HEADER
#pragma once

#include <cassert>
#include "../__p0009_bits/dynamic_extent.hpp"

namespace MDSPAN_IMPL_STANDARD_NAMESPACE {
namespace MDSPAN_IMPL_PROPOSED_NAMESPACE {

template <size_t padding_stride = dynamic_extent>
struct layout_left_padded {
  template <class _Extents>
  class mapping;
};

template <size_t padding_stride = dynamic_extent>
struct layout_right_padded {
  template <class _Extents>
  class mapping;
};

namespace detail {

template <class _Layout, class _ExtentsType>
struct __layout_padded_constants;

template <class _ExtentsType, size_t _PaddingStride>
struct __layout_padded_constants<layout_left_padded<_PaddingStride>, _ExtentsType>
{
  using rank_type = typename _ExtentsType::rank_type;
  static constexpr rank_type __padding_stride_idx = 1;
  static constexpr rank_type __extent_to_pad_idx = 0;
};

template <class _ExtentsType, size_t _PaddingStride>
struct __layout_padded_constants<layout_right_padded<_PaddingStride>, _ExtentsType>
{
  using rank_type = typename _ExtentsType::rank_type;
  static constexpr rank_type __padding_stride_idx = _ExtentsType::rank() - 2;
  static constexpr rank_type __extent_to_pad_idx = _ExtentsType::rank() - 1;
};

template <class _Layout>
struct __is_layout_left_padded : std::false_type {};

template <size_t _PaddingStride>
struct __is_layout_left_padded<layout_left_padded<_PaddingStride>> : std::true_type {};

template <class _Mapping, class _Enabled = void>
struct __is_layout_left_padded_mapping : std::false_type {};

template <class _Mapping>
struct __is_layout_left_padded_mapping<_Mapping,
  std::enable_if_t<std::is_same_v<_Mapping, typename layout_left_padded<_Mapping::padding_stride>::template mapping<typename _Mapping::extents_type>>>>
    : std::true_type {};

template <class _Layout>
struct __is_layout_right_padded : std::false_type {};

template <size_t _PaddingStride>
struct __is_layout_right_padded<layout_right_padded<_PaddingStride>> : std::true_type {};

template <class _Mapping, class _Enabled = void>
struct __is_layout_right_padded_mapping : std::false_type {};

template <class _Mapping>
struct __is_layout_right_padded_mapping<_Mapping,
  std::enable_if_t<std::is_same_v<_Mapping, typename layout_right_padded<_Mapping::padding_stride>::template mapping<typename _Mapping::extents_type>>>>
    : std::true_type {};

template <class _LayoutExtentsType, class _PaddedLayoutMappingType>
constexpr void __check_padded_layout_converting_constructor_mandates()
{
  using __extents_type = typename _PaddedLayoutMappingType::extents_type;
  constexpr auto __padding_stride = _PaddedLayoutMappingType::padding_stride;
  constexpr auto __idx = __layout_padded_constants<typename _PaddedLayoutMappingType::layout_type, _LayoutExtentsType >::__extent_to_pad_idx;
  if constexpr ((_LayoutExtentsType::rank() > 1) &&
                (_LayoutExtentsType::static_extent(__idx) != dynamic_extent) &&
                (__extents_type::static_extent(__idx) != dynamic_extent) &&
                (__padding_stride != dynamic_extent)) {
    if constexpr (__padding_stride == 0) {
      static_assert(_LayoutExtentsType::static_extent(__idx) == 0);
    } else {
      static_assert(
          _LayoutExtentsType::static_extent(__idx) % __padding_stride == 0);
    }
  }
}

template <typename _ExtentsType, typename _OtherMapping>
constexpr void __check_padded_layout_converting_constructor_preconditions(const _OtherMapping &__other_mapping) {
  constexpr auto __padding_stride_idx = __layout_padded_constants<typename _OtherMapping::layout_type, _ExtentsType>::__padding_stride_idx;
  constexpr auto __extent_to_pad_idx = __layout_padded_constants<typename _OtherMapping::layout_type, _ExtentsType>::__extent_to_pad_idx;
  if constexpr (_ExtentsType::rank() > 1) {
    assert(__other_mapping.stride(__padding_stride_idx) == __other_mapping.extents().extent(__extent_to_pad_idx));
  }
}
}
}
}
