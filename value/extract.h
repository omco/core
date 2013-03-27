//#####################################################################
// Function extract
//#####################################################################
#pragma once

#include <othercore/value/Compute.h>
#include <othercore/structure/Tuple.h>
#include <othercore/utility/curry.h>
#include <othercore/utility/remove_const_reference.h>
namespace other{

template<int n,class Tuple> static inline auto extract_helper(const ValueRef<Tuple>& value)
  -> decltype(value().template get<n>()) {
  return value().template get<n>();
}

template<class Array> static inline auto extract_helper(const ValueRef<Array>& value, int n)
  -> decltype(value()[n]) {
  auto& array = value();
  OTHER_ASSERT(n<(int)array.size());
  return array[n];
}

// Extract the nth element of a tuple value as a value
template<int n,class Tuple> static inline auto extract(const ValueRef<Tuple>& value)
  -> ValueRef<typename remove_const_reference<decltype(value().template get<n>())>::type> {
  typedef decltype(value().template get<n>()) T;
  return cache(static_cast<T(*)(const ValueRef<Tuple>&)>(extract_helper<n>),value);
}

// Extract the nth element of an array value as a value
template<class Array> static inline auto extract(const ValueRef<Array>& value, int n)
  -> ValueRef<typename remove_const_reference<decltype(value()[0])>::type> {
  OTHER_ASSERT(n>=0);
  return cache(extract_helper<Array>,value,n);
}

}
