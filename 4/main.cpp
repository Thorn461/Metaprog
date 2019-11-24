#include <iostream>
#include <string>
/*
 Теортрек, задание 4
Необходимо реализовать Functor.
 Требования к нему есть на фото (как он должен работать).
 Кто-нибудь, выложите в чат, пожалуйста.
Дедлайн 09:00 25 ноября
 */

template <typename ...Tail>
class ArgsPack;

template <>
class ArgsPack<> {
 public:
  template <class Delegate, typename ...Args>
  auto Call(Delegate& delegate, Args&&... args) -> decltype(delegate(args...))
  {
    return delegate(args...);
  }
};

template<typename T>
T* ConvertToPtr(T& t) {
  return &t;
}

template<typename T>
T* ConvertToPtr(T* t) {
  return t;
}

template<typename T>
T& ConvertToRef(T& t) {
  return t;
}

template<typename T>
T& ConvertToRef(T* t) {
  return *t;
}

template<class Head, typename... Tail>
class ArgsPack<Head, Tail...> : ArgsPack<Tail...> {
 private:
  Head m_Param;

 public:
  typedef ArgsPack<Tail...> __base;

  template<typename _Head, typename... _Tail>
  ArgsPack(_Head&& head, _Tail&&... tail): __base(std::forward<_Tail>(tail)...), m_Param(std::forward<_Head>(head)) {}

  template <class Delegate, typename...Args>
  auto Call(const Delegate& delegate, Args... args) ->
  decltype(this->__base::Call(delegate, args..., ConvertToRef(m_Param))) {
    return __base::Call(delegate, args... , ConvertToRef(m_Param));
  }
  
  template <class Delegate, typename...Args>
  auto Call(const Delegate& delegate, Args... args) ->
  decltype(this->__base::Call(delegate, args..., ConvertToPtr(m_Param))) {
    return __base::Call(delegate, args..., ConvertToPtr(m_Param));
  }
};

template <class TObject, typename TCE>
class SimpleDelegate {
 private:
  TObject& m_Object;
  TCE m_CallableEntity;
  
 public:
  SimpleDelegate(TObject& object, const TCE& ptr) 
  : m_Object(object), m_CallableEntity(ptr) {}

  template <typename... Params2>
  auto operator()(Params2... args) const
  -> decltype((m_Object.*m_CallableEntity)(args...)) {
    return (m_Object.*m_CallableEntity)(args...);
  }
};

template <class TCE>
struct CEWrapper {
 private:
  TCE m_CallableEntity;

 public:
  CEWrapper(const TCE& ptr) : m_CallableEntity(ptr) {}

  template <typename... Params2>
  auto operator()(Params2... args) const -> decltype(m_CallableEntity(args...)) {
    return m_CallableEntity(args...);
  }
};

template <class TCallableEntity, typename... Args>
class Functor {
 private:
  ArgsPack<Args...> __base;
  TCallableEntity m_CallableEntity;

 public:
  Functor(const TCallableEntity& delegate, Args&&...args):
    __base(std::forward<Args>(args)...), m_CallableEntity(delegate) {}

  template <class TCE = TCallableEntity>
  auto Call() ->
  decltype(this->__base.Call(CEWrapper<TCE>(m_CallableEntity))) const {
    return __base.Call(m_CallableEntity);
  }

  template <class Object>
  auto Call(Object& object) ->
  decltype(this->__base.Call(SimpleDelegate<Object, TCallableEntity>(object, m_CallableEntity))) const {
    return __base.Call(SimpleDelegate<Object, TCallableEntity>(object, m_CallableEntity));
  }
};

template <class TCallableEntity, typename... Params>
Functor<TCallableEntity, Params...> make_functor(const TCallableEntity& callable_entity,
    Params&&...given_params) {
  return Functor<TCallableEntity, Params...>(callable_entity, std::forward<Params>(given_params)...);
}



int function(int a, float b, short c) {
  return 1+ a + b - c;
}

class Y {
 public:
  int y_ = 2;
};

class X {
 public:
  float process(Y& y) {
    return y.y_;
  }
};

int main() {
// обычные функции
  auto functor1 = make_functor(&function, 1, 2, 3);
  int x1 = functor1.Call();
  std::cout << x1 << std::endl;
// функции-члены
  auto functor2 = make_functor(&X::process, Y());
  X x;
  float y0 = functor2.Call(x);
  std::cout << y0 << std::endl;
// лямбды
  Y y;
  auto functor3 = make_functor([](Y*) {return "test 3: lymbda";}, y);
  std::cout << functor3.Call() << std::endl;
  return 0;
}