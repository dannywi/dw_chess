#pragma once

namespace dwc::utils {

template <typename... TS>
struct type_list {};

// basic head, tail, size, at
template <typename HEAD, typename... TAIL>
struct head {
  using type = HEAD;
};

template <typename HEAD, typename... TAIL>
struct tail {
  using type = type_list<TAIL...>;
};

template <typename... TS>
struct size {
  static constexpr size_t value = sizeof...(TS);
};

template <size_t I, typename HEAD, typename... TAIL>
struct at {
  using type = typename at<I - 1, TAIL...>::type;
};

template <typename HEAD, typename... TAIL>
struct at<0, HEAD, TAIL...> {
  using type = HEAD;
};

// specialized versions for type_list
template <typename HEAD, typename... TAIL>
struct head<type_list<HEAD, TAIL...>> {
  using type = HEAD;
};

template <typename HEAD, typename... TAIL>
struct tail<type_list<HEAD, TAIL...>> {
  using type = type_list<TAIL...>;
};

template <typename... TS>
struct size<type_list<TS...>> {
  static constexpr size_t value = sizeof...(TS);
};

template <size_t I, typename HEAD, typename... TAIL>
struct at<I, type_list<HEAD, TAIL...>> {
  using type = typename at<I - 1, TAIL...>::type;
};

template <typename HEAD, typename... TAIL>
struct at<0, type_list<HEAD, TAIL...>> {
  using type = HEAD;
};

template <typename... TS>
using head_t = typename head<TS...>::type;
template <typename... TS>
using tail_t = typename tail<TS...>::type;
template <size_t I, typename... TS>
using at_t = typename at<I, TS...>::type;
template <typename... TS>
static constexpr size_t size_v = size<TS...>::value;

}  // namespace dwc::utils
