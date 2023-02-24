#pragma once

namespace dwc::utils {

template <typename... TS>
struct type_list {};

// basic head, tail, size, at
template <typename HEAD, typename... REST>
struct head {
  using type = HEAD;
};

template <typename HEAD, typename... REST>
struct tail {
  using type = type_list<REST...>;
};

template <typename... TS>
struct size {
  static constexpr size_t value = sizeof...(TS);
};

template <size_t I, typename HEAD, typename... REST>
struct at {
  using type = typename at<I - 1, REST...>::type;
};

template <typename HEAD, typename... REST>
struct at<0, HEAD, REST...> {
  using type = HEAD;
};

// specialized versions for type_list
template <typename HEAD, typename... REST>
struct head<type_list<HEAD, REST...>> {
  using type = HEAD;
};

template <typename HEAD, typename... REST>
struct tail<type_list<HEAD, REST...>> {
  using type = type_list<REST...>;
};

template <typename... TS>
struct size<type_list<TS...>> {
  static constexpr size_t value = sizeof...(TS);
};

template <size_t I, typename HEAD, typename... REST>
struct at<I, type_list<HEAD, REST...>> {
  using type = typename at<I - 1, REST...>::type;
};

template <typename HEAD, typename... REST>
struct at<0, type_list<HEAD, REST...>> {
  using type = HEAD;
};

}  // namespace dwc::utils
