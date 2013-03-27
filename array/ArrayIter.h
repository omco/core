//#####################################################################
// Class ArrayIter
//#####################################################################
#pragma once

#include <othercore/array/forward.h>
#include <othercore/utility/remove_const_reference.h>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/declval.hpp>
#include <iterator>
namespace other {

template<class TArray> class ArrayIter {
  const TArray* array;
  int index;
public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef decltype(boost::declval<TArray>()[0]) reference;
  typedef typename boost::remove_reference<reference>::type value_type;
  typedef int difference_type;
  typedef value_type* pointer;

  ArrayIter(const TArray& array, int index)
    : array(&array), index(index) {}

  ArrayIter& operator++() {
    index++;return *this;
  }

  ArrayIter& operator--() {
    index--;return *this;
  }

  ArrayIter operator++(int post) {
    return ArrayIter(*array,index++);
  }

  ArrayIter operator--(int post) {
    return ArrayIter(*array,index--);
  }

  ArrayIter operator+(int n) const {
    return ArrayIter(*array,index+n);
  }

  ArrayIter operator-(int n) const {
    return ArrayIter(*array,index-n);
  }

  int operator-(ArrayIter other) const {
    return index-other.index; // Assume array==other.array
  }

  bool operator==(ArrayIter other) {
    return index==other.index; // Assume array==other.array
  }

  bool operator!=(ArrayIter other) {
    return index!=other.index; // Assume array==other.array
  }

  bool operator<(ArrayIter other) {
    return index<other.index; // Assume array==other.array
  }

  bool operator<=(ArrayIter other) {
    return index<=other.index; // Assume array==other.array
  }

  reference operator*() const {
    return (*array)[index];
  }
};

}
