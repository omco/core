//#####################################################################
// Class Box<Vector<T,d>>
//#####################################################################
//
// An axis aligned d-dimensional box.  Set are operations are (conservatively) exact,
// but arithmetic (sum, scalar multiply, etc.) is not.
//
//#####################################################################
#pragma once

#include <other/core/array/forward.h>
#include <other/core/geometry/forward.h>
#include <other/core/math/clamp.h>
#include <other/core/math/Zero.h>
#include <other/core/vector/Vector.h>
#include <limits>
namespace other {

using std::numeric_limits;

template<class T,int d> PyObject* to_python(const Box<Vector<T,d>>& box) OTHER_EXPORT;
template<class T,int d> struct FromPython<Box<Vector<T,d>>>{OTHER_EXPORT static Box<Vector<T,d>> convert(PyObject* object);};

template<class T,int d_> class Box<Vector<T,d_>> {
  typedef Vector<T,d_> TV;
public:
  typedef T Scalar;
  typedef TV VectorT;
  enum Workaround {d=d_};

  TV min, max;

  Box()
    : min(numeric_limits<T>::max()*TV::ones()), max((T)-numeric_limits<T>::max()*TV::ones()) {}

  Box(const T xmin, const T xmax)
    : min(xmin), max(xmax) {
    BOOST_STATIC_ASSERT(d==1);
  }

  Box(const T xmin, const T xmax, const T ymin, const T ymax)
    : min(xmin,ymin), max(xmax,ymax) {
    BOOST_STATIC_ASSERT(d==2);
  }

  Box(const T xmin, const T xmax, const T ymin, const T ymax, const T zmin, const T zmax)
    : min(xmin,ymin,zmin), max(xmax,ymax,zmax) {
    BOOST_STATIC_ASSERT(d==3);
  }

  Box(const TV& min, const TV& max)
    : min(min), max(max) {}

  template<class T2> explicit Box(const Box<T2>& box)
    : min(TV(box.min)), max(TV(box.max)) {}

  explicit Box(const TV& point)
    : min(point), max(point) {}

  Box(const Box<TV>& box, const Frame<Vector<T,1> >& frame) // Allow 1d boxes to be used as oriented boxes
    : min(box.min), max(box.max) {
    BOOST_STATIC_ASSERT(d==1);
  }

  Box<TV> axis_aligned_bounding_box() const {
    return *this;
  }

  static Box<TV> unit_box() {
    return Box<TV>(TV(),TV::ones());
  }

  static Box<TV> zero_box() {
    return Box<TV>(TV(),TV());
  }

  static Box<TV> empty_box() {
    return Box<TV>(numeric_limits<T>::max()*TV::ones(),(T)-numeric_limits<T>::max()*TV::ones());
  }

  static Box<TV> full_box() {
    return Box<TV>((T)-numeric_limits<T>::max()*TV::ones(),numeric_limits<T>::max()*TV::ones());
  }

  bool empty() const {
    return !min.all_less_equal(max);
  }

  bool operator==(const Box<TV>& r) const {
    return min==r.min && max==r.max;
  }

  bool operator!=(const Box<TV>& r) const {
    return !(*this==r);
  }

  Box<TV> operator-() const {
    return Box<TV>(-max,-min);
  }

  Box<TV>& operator+=(const Box<TV>& r) {
    min+=r.min;max+=r.max;return *this;
  }

  Box<TV>& operator-=(const Box<TV>& r) {
    min-=r.max;max-=r.min;return *this;
  }

  Box<TV> operator+(const Box<TV>& r) const {
    return Box<TV>(min+r.min,max+r.max);
  }

  Box<TV> operator-(const Box<TV>& r) const {
    return Box<TV>(min-r.max,max-r.min);
  }

  Box<TV> operator*(const T a) const {
    return a>=0?Box<TV>(min*a,max*a):Box<TV>(max*a,min*a);
  }

  Box<TV>& operator*=(const T a) {
    return *this = *this*a;
  }

  Box<TV> operator/(const T a) const {
    assert(a!=0);return *this*inverse(a);
  }

  Box<TV>& operator/=(const T a) {
    return *this = *this/a;
  }

  TV sizes() const {
    return max-min;
  }

  TV center() const {
    return (T).5*(min+max);
  }

  void corners(Array<TV>& corners) const {
    BOOST_STATIC_ASSERT(d==1);
    corners.resize(2);
    corners[0] = min;
    corners[1] = max;
  }

  void corners(Array<TV,2>& corners) const {
    BOOST_STATIC_ASSERT(d==2);
    corners.resize(2,2);
    for (int i=0;i<=1;i++) for (int j=0;j<=1;j++)
      corners(i,j) = TV(i?max.x:min.x,j?max.y:min.y);
  }

  void corners(Array<TV,3>& corners) const {
    BOOST_STATIC_ASSERT(d==3);
    corners.resize(2,2,2);
    for (int i=0;i<=1;i++) for (int j=0;j<=1;j++) for (int k=0;k<=1;k++)
      corners(i,j,k) = TV(i?max.x:min.x,j?max.y:min.y,k?max.z:min.z);
  }

  T volume() const {
    return empty()?(T)0:sizes().product();
  }

  T surface_area() const {
    BOOST_STATIC_ASSERT(d==3);
    Vector<T,3> size(sizes());
    return 2*(size.x*(size.y+size.z)+size.y*size.z);
  }

  void enlarge(const TV& point) {
    min = TV::componentwise_min(min,point);
    max = TV::componentwise_max(max,point);
  }

  void enlarge_nonempty(const TV& point) {
    assert(!empty());
    for (int i=0;i<d;i++) {
      if (point[i]<min[i]) min[i] = point[i];
      else if (point[i]>max[i]) max[i]=point[i];
    }
  }

  void enlarge_nonempty(const TV& p1, const TV& p2) {
    enlarge_nonempty(p1);
    enlarge_nonempty(p2);
  }

  void enlarge_nonempty(const TV& p1, const TV& p2, const TV& p3) {
    enlarge_nonempty(p1);
    enlarge_nonempty(p2);
    enlarge_nonempty(p3);
  }

  template<class TArray> void enlarge_nonempty(const TArray& points) {
    STATIC_ASSERT_SAME(typename TArray::Element,TV);
    for (int i=0;i<points.size();i++)
      enlarge_nonempty(points[i]);
  }

  void enlarge(const Box<TV>& box) {
    min = TV::componentwise_min(min,box.min);
    max = TV::componentwise_max(max,box.max);
  }

  void enlarge_nonempty(const Box<TV>& box) {
    enlarge(box);
  }

  void change_size(const T delta) {
    min-=delta;max+=delta;
  }

  void change_size(const TV& delta) {
    min-=delta;max+=delta;
  }

  Box<TV> thickened(const T half_thickness) const {
    return Box<TV>(min-half_thickness,max+half_thickness);
  }

  Box<TV> thickened(const TV& half_thickness) const {
    return Box<TV>(min-half_thickness,max+half_thickness);
  }

  static Box<TV> combine(const Box<TV>& box1, const Box<TV>& box2) {
    return Box<TV>(TV::componentwise_min(box1.min,box2.min),TV::componentwise_max(box1.max,box2.max));
  }

  static Box<TV> intersect(const Box<TV>& box1, const Box<TV>& box2) {
    return Box<TV>(TV::componentwise_max(box1.min,box2.min),TV::componentwise_min(box1.max,box2.max));
  }

  void scale_about_center(const T factor) {
    TV center = (T).5*(min+max),
       half_length = factor*(T).5*(max-min);
    min = center-half_length;
    max = center+half_length;
  }

  void scale_about_center(const TV factor) {
    TV center = (T).5*(min+max),
       half_length = factor*(T).5*(max-min);
    min = center-half_length;
    max = center+half_length;
  }

  bool lazy_inside(const TV& location) const {
    return location.all_greater_equal(min) && location.all_less_equal(max);
  }

  bool lazy_inside_half_open(const TV& location) const {
    return location.all_greater_equal(min) && location.all_less(max);
  }

  bool inside(const TV& location, const T half_thickness) const {
    return thickened(-half_thickness).lazy_inside(location);
  }

  bool inside(const TV& location, const Zero half_thickness) const {
    return lazy_inside(location);
  }

  bool lazy_outside(const TV& location) const {
    return !lazy_inside(location);
  }

  bool outside(const TV& location, const T half_thickness) const {
    return thickened(half_thickness).lazy_outside(location);
  }

  bool outside(const TV& location, const Zero half_thickness) const {
    return lazy_outside(location);
  }

  bool boundary(const TV& location, const T half_thickness) const {
    bool strict_inside=location.all_greater(min+half_thickness) && location.all_less(max-half_thickness);
    return !strict_inside && !outside(location,half_thickness);
  }

  TV clamp(const TV& location) const {
    return other::clamp(location,min,max);
  }

  T clamp(const T& location) const {
    BOOST_STATIC_ASSERT(d==1);
    return clamp(TV(location)).x;
  }

  void project_points_onto_line(const TV& direction, T& line_min, T& line_max) const {
    line_min = line_max = dot(direction,min);
    TV e = direction*(max-min);
    
    for (int i=0;i<d;i++) {
      (e[i]>0?line_max:line_min) += e[i]; // do not remove these parenthese, even if you think you know what you're doing.
    }
  }

  TV point_from_normalized_coordinates(const TV& weights) const {
    return min+weights*(max-min);
  }

  bool contains(const Box<TV>& box) const {
    return min.all_less_equal(box.min) && max.all_greater_equal(box.max);
  }

  bool lazy_intersects(const Box<TV>& box) const {
    return min.all_less_equal(box.max) && max.all_greater_equal(box.min);
  }

  bool intersects(const Box<TV>& box, const T half_thickness) const {
    return thickened(half_thickness).lazy_intersects(box);
  }

  bool intersects(const Box<TV>& box, const Zero half_thickness) const {
    return lazy_intersects(box);
  }

  bool intersects(const Box<TV>& box) const {
    return lazy_intersects(box);
  }

  Box<Vector<T,d-1> > horizontal_box() const {
    return Box<Vector<T,d-1> >(min.horizontal_vector(),max.horizontal_vector());
  }

  Box<Vector<T,d-1> > vertical_box() const {
    BOOST_STATIC_ASSERT(d==2);
    return Box<Vector<T,d-1> >(min.vertical_vector(),max.vertical_vector());
  }

  Box<Vector<T,d-1> > remove_dimension(int dimension) const {
    return Box<Vector<T,d-1> >(min.remove_index(dimension),max.remove_index(dimension));
  }

  const Box<TV>& bounding_box() const { // for templatization purposes
    return *this;
  }

  Vector<T,TV::dimension-1> principal_curvatures(const TV& X) const {
    return Vector<T,TV::dimension-1>();
  }

  T sqr_distance_bound(const TV& X) const {
    return sqr_magnitude(X-clamp(X));
  }

  TV normal(const TV& X) const;
  TV surface(const TV& X) const;
  T phi(const TV& X) const;
  static std::string name();
  string repr() const;
  bool lazy_intersects(const Ray<TV>& ray,T box_enlargement) const;
};

}
