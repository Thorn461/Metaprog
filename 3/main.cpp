#include <iostream>
/*
Необходимо реализовать получение значения поля
 при использовании GenScatterHierarchy по атомарному типу.
Усложненное задание:
 то же самое, но значение поля надо уметь получать по индексу типа в TypeList ,
 использовавшемуся при генерации иерархии
*/

struct NullType {};

template<class H, class T>
struct TypeList  {
  using head = H;
  using tail = T;

};

template<class T>
struct Holder {
  T value;
};

template<class TList, template<class> class Unit>
class GenScatterHierarchy;

template<class T1, class T2, template<class> class Unit>
class GenScatterHierarchy<TypeList <T1, T2>, Unit>
    : public GenScatterHierarchy<T1, Unit>
        , public GenScatterHierarchy<T2, Unit> {
 public:
  using TList = TypeList <T1, T2>;
  using LeftBase = GenScatterHierarchy<T1, Unit>;
  using RightBase = GenScatterHierarchy<T2, Unit>;

  template<class T>
  struct Rebind {
    using Result = Unit<T>;
  };
};

template<class AtomicType, template<class> class Unit>
class GenScatterHierarchy : public Unit<AtomicType> {
 public:
  using LeftBase = Unit<AtomicType>;

  template<class T>
  struct Rebind {
    using Result = Unit<T>;
  };
};

template<template<class> class Unit>
class GenScatterHierarchy<NullType, Unit> {
  template<class T>
  struct Rebind {
    using Result = Unit<T>;
  };
};

template <int id, class T>
struct TypeId  {
  typedef typename TypeId <id - 1, typename T::tail >::type type;
};

template <class T1, class T2>
struct TypeId <0, TypeList <T1, T2>> {
  typedef T1 type;
};

template <class T1>
struct TypeId <0, T1> {
  typedef T1 type;
};

template<class T, class H>
typename H::template Rebind<T>::Result &Field(H &obj) {
  return obj;
}

template<int id, class H>
typename H::template Rebind<typename TypeId <id, typename H::TList>::type>::Result &Field(H &obj) {
  return obj;
}

using WidgetInfo = GenScatterHierarchy<TypeList <int, TypeList <std::string, double>>, Holder>;

int main() {
  WidgetInfo hierarchy ;

  Field<int>(hierarchy ).value = 1;
  Field<std::string>(hierarchy ).value = "str1";
  Field<double>(hierarchy ).value = 1.1;

  std::cout << Field<int>(hierarchy ).value << std::endl;
  std::cout << Field<std::string>(hierarchy ).value << std::endl;
  std::cout << Field<double>(hierarchy ).value << std::endl;

  Field<0>(hierarchy ).value = 2;
  Field<1>(hierarchy ).value = "str2";
  Field<2>(hierarchy ).value = 2.2;

  std::cout << Field<0>(hierarchy ).value << std::endl;
  std::cout << Field<1>(hierarchy ).value << std::endl;
  std::cout << Field<2>(hierarchy ).value << std::endl;

  std::cout << Field<int>(hierarchy ).value << std::endl;
  std::cout << Field<std::string>(hierarchy ).value << std::endl;
  std::cout << Field<double>(hierarchy ).value << std::endl;

  return 0;
}