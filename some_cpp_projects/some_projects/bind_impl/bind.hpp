#pragma once

#include <tuple>

namespace fn
{
    using namespace std::placeholders;
    using std::bad_function_call;


    /*************** check if ref wrapper *****************/
    template <typename T>
    class is_ref_wrapper : public std::false_type
    {};


    template<typename T>
    class is_ref_wrapper<std::reference_wrapper<T>> : public std::true_type
    {};


    template<typename T>
    constexpr bool is_ref_wrapper_v = is_ref_wrapper<T>::value;


    /*************** deducing arg_types to pass them to functor *****************/
    template <typename Arg_dt, typename OpArgsTuple_t, bool is_ref_wrapper, bool is_placeholder>
    class processed_arg_base
    {};


    template <typename Arg_dt, typename OpArgsTuple_t>
    class processed_arg_base<Arg_dt, OpArgsTuple_t, false, false>
    {
    public:
        using type = Arg_dt&;
    };


    template <typename Arg_dt, typename OpArgsTuple_t>
    class processed_arg_base<Arg_dt, OpArgsTuple_t, true, false>
    {
    public:
        using type = typename Arg_dt::type&;
    };


    template <typename Arg_dt, typename OpArgsTuple_t>
    class processed_arg_base<Arg_dt, OpArgsTuple_t, false, true>
    {
    public:
        using type = std::tuple_element_t <std::is_placeholder_v<Arg_dt> - 1, OpArgsTuple_t>&&;
    };


    template <typename Arg_dt, typename OpArgsTuple_t>
    class processed_arg : public processed_arg_base<
            Arg_dt,
            OpArgsTuple_t,
            is_ref_wrapper_v<Arg_dt>,
            (std::is_placeholder_v<Arg_dt> > 0) && (std::is_placeholder_v<Arg_dt> <= std::tuple_size_v<OpArgsTuple_t>)>
    {};


    template <typename Arg_dt, typename OpArgsTuple_t>
    using processed_arg_t = typename processed_arg<Arg_dt, OpArgsTuple_t>::type;

    
    template <typename Arg_dt, typename OpArgsTuple_t>
    inline Arg_dt& process_arg(std::reference_wrapper<Arg_dt> ref_arg, OpArgsTuple_t& )
    {
        return ref_arg.get();
    }


    template <typename  Arg_t, typename OpArgsTuple_t, bool is_placeholder>
    struct processed_arg_helper {};

    
    template <typename  Arg_t, typename OpArgsTuple_t>
    struct processed_arg_helper<Arg_t, OpArgsTuple_t, true>
    {
        using type = std::tuple_element_t<std::is_placeholder_v<Arg_t> - 1, OpArgsTuple_t>;
    };

    
    template <typename Arg_t, typename OpArgsTuple_t>
    using processed_arg_helper_t = typename processed_arg_helper<Arg_t, OpArgsTuple_t, 0 < std::is_placeholder_v<Arg_t>>::type;

    
    template <typename Arg_dt, typename OpArgsTuple_t>
    std::enable_if_t <
            0 < std::is_placeholder_v<Arg_dt>,  processed_arg_helper_t<Arg_dt, OpArgsTuple_t>>
    process_arg(Arg_dt&, OpArgsTuple_t& opArgs_tuple)
    {
        const size_t ind = std::is_placeholder_v<Arg_dt> - 1;
        return std::forward<std::tuple_element_t<ind, OpArgsTuple_t>>(std::get<ind>(opArgs_tuple));
    }

    template <typename Arg_dt, typename OpArgsTuple_t>
    typename std::enable_if_t
            <
                    std::is_placeholder_v<Arg_dt> == 0 &&
                    !is_ref_wrapper_v<Arg_dt>,
                    Arg_dt&
            >
    process_arg(Arg_dt& argDt, OpArgsTuple_t&)
    {
        return argDt;
    }


    template<typename Func_dt, typename OpArgsTuple_t, typename BoundArgs_tuple_t, size_t ...I>
    decltype(auto)
    apply_func_impl(Func_dt& funcDt, BoundArgs_tuple_t& boundArgsTuple, OpArgsTuple_t&& opArgsTuple,
                    std::index_sequence<I...>)
    {
        return funcDt(process_arg(std::get<I>(boundArgsTuple), opArgsTuple)...);
    }


    template <typename Func_dt, typename BoundArgs_tuple_t, typename OpArgsTuple_t>
    decltype(auto)
    apply_func(Func_dt& funcDt, BoundArgs_tuple_t& boundArgsTuple, OpArgsTuple_t&& opArgsTuple)
    {
        return apply_func_impl(funcDt, boundArgsTuple, std::forward<OpArgsTuple_t>(opArgsTuple),
                std::make_index_sequence<std::tuple_size_v<BoundArgs_tuple_t>>{});
    }


    /*************** check that enough arguments bound *****************/
    template <typename  R, typename ...Args>
    class args_num_aware
    {
    public:
        static constexpr size_t n_args = sizeof...(Args);
    };


    template <typename  R, typename ...Args>
    auto constexpr make_args_num_aware(R (Args...) )
    {
        return args_num_aware<R, Args...>();
    }


    template<typename Func_t, typename ...BoundArgsPack>
    class binder
    {
        using Func_dt = std::decay_t<Func_t>;
        using BoundArgs_tuple_t = std::tuple<std::decay_t<BoundArgsPack> ...>;
    public:
        explicit binder(Func_t&& f, BoundArgsPack&&... bound_args)
                :func_(std::forward<Func_t>(f))
                ,boundArgsTuple_(std::make_tuple(std::forward<BoundArgsPack>(bound_args)...))
        {
            static_assert(decltype(make_args_num_aware(func_))::n_args == sizeof...(BoundArgsPack));
        }
        template <typename ...OpArgsPack>
        decltype(auto) operator()(OpArgsPack&&... op_args)
        {
            return apply_func(func_, boundArgsTuple_, std::tuple<OpArgsPack&&...>(std::forward<OpArgsPack>(op_args)...));
        }

    private:
        Func_dt func_;
        BoundArgs_tuple_t boundArgsTuple_;
    };

    // Замечание: копирование, мув, присваивание у биндера будут тривиальными. То есть компилятор сам их благополучно
    // сгенерит

    template <typename Func_t, typename ...ArgsPack>
    binder<Func_t, ArgsPack...> bind(Func_t&& func, ArgsPack&& ...args)
    {
        using returning_type = binder<Func_t, ArgsPack...>;
        return returning_type(std::forward<Func_t>(func), std::forward<ArgsPack>(args)...);
    }
}
