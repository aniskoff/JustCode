#pragma once

#include <algorithm>
#include <functional>
#include <type_traits>

namespace smart_ptr
{
    namespace details
    {
        template<class T>
        inline void checked_delete(T* x) noexcept
        {
            typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
            (void) sizeof(type_must_be_complete);
            delete x;
        }
        class linked_node
        {
        public:
            linked_node() noexcept
                    :next_(nullptr)
                    ,prev_(nullptr)

            {}

            linked_node(linked_node& other) noexcept
                    :next_(other.next_)
                    ,prev_(&other)
            {
                update_links();
            }

            const linked_node* get_prev() const noexcept
            {
                return prev_;
            }

            const linked_node* get_next() const noexcept
            {
                return next_;
            }

            void swap(linked_node& other) noexcept
            {
                bool this_left_neighbour  = next_ == &other;
                bool this_right_neighbour = prev_ == &other;
                std::swap(prev_, other.prev_);
                std::swap(next_, other.next_);
                if (this_left_neighbour)
                {
                    other.next_ = this;
                    prev_       = &other;
                }
                if (this_right_neighbour)
                {
                    other.prev_ = this;
                    next_       = &other;
                }
                this->update_links();
                other.update_links();

            }

            void update_links() noexcept
            {
                if (prev_ != nullptr)
                    prev_->next_ = this;
                if (next_ != nullptr)
                    next_->prev_ = this;
            }

            ~linked_node() noexcept
            {
                if (prev_ != nullptr)
                    prev_->next_ = next_;
                if (next_ != nullptr)
                    next_->prev_ = prev_;
                prev_ = next_ = nullptr;
            }

        private:
            linked_node* next_;
            linked_node* prev_;
        };
    }

    template <class T>
    class linked_ptr
    {
    public:

        linked_ptr() noexcept
                :ptr_(nullptr)

        {}

        explicit linked_ptr(T* x) noexcept
                :ptr_(x)
        {}

        template<class U>
        explicit linked_ptr(U* x) noexcept
                :ptr_(x)
        {}

        linked_ptr(const linked_ptr& other) noexcept
                :ptr_(other.ptr_)
                ,node(other.node)
        {}

        template<class U>
        linked_ptr(const linked_ptr<U>& other) noexcept
                :ptr_(other.ptr_)
                ,node(other.node)
        {}

        linked_ptr& operator=(const linked_ptr& other) noexcept
        {
            linked_ptr(other).swap(*this);
            return *this;
        }

        template<class U>
        linked_ptr& operator=(const linked_ptr<U>& other) noexcept
        {
            linked_ptr(other).swap(*this);
            return *this;
        }

        void reset() noexcept
        {
            linked_ptr().swap(*this);
        }

        template<class U>
        void reset(U* x) noexcept
        {
            linked_ptr(x).swap(*this);
        }

        void swap(linked_ptr& other) noexcept
        {
            std::swap(ptr_, other.ptr_);
            node.swap(other.node);
        }

        T* get() const noexcept
        {
            return ptr_;
        }

        bool unique() const noexcept
        {
            if (get() != nullptr)
                return !node.get_prev() && !node.get_next();
            return false;
        }

        T* operator->() const noexcept
        {
            return ptr_;
        }

        T& operator*()  const noexcept
        {
            return *ptr_;
        }

        explicit operator bool() const noexcept
        {
            return ptr_ != nullptr;
        }

        ~linked_ptr() noexcept
        {
            if (node.get_next() == nullptr && node.get_prev() == nullptr)
            {
                details::checked_delete(ptr_);
            }
            ptr_ = nullptr;

        }
        template<class U>
        friend class linked_ptr;

    private:
        T*                           ptr_;
        mutable details::linked_node node;
    };

    template <class T, class U>
    bool operator==(const linked_ptr<T>& lhs, const linked_ptr<U>& rhs) noexcept
    {
        return lhs.get() == rhs.get();
    }

    template <class T, class U>
    bool operator!= (const linked_ptr<T>& lhs, const linked_ptr<U>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <class T, class U>
    bool operator< (const linked_ptr<T>& lhs, const linked_ptr<U>& rhs) noexcept
    {
        using V = typename std::common_type<T*, U*>::type;
        return std::less<V>()(lhs.get(), rhs.get());
    }

    template <class T, class U>
    bool operator> (const linked_ptr<T>& lhs, const linked_ptr<U>& rhs) noexcept
    {
        return rhs < lhs;
    }

    template <class T, class U>
    bool operator<= (const linked_ptr<T>& lhs, const linked_ptr<U>& rhs) noexcept
    {
        return !(rhs < lhs);
    }

    template <class T, class U>
    bool operator>= (const linked_ptr<T>& lhs, const linked_ptr<U>& rhs) noexcept
    {
        return !(lhs < rhs);
    }

    template <class T>
    bool operator== (const linked_ptr<T>& lhs, std::nullptr_t) noexcept
    {
        return !lhs;
    }

    template <class T>
    bool operator== (std::nullptr_t, const linked_ptr<T>& rhs) noexcept
    {
        return !rhs;
    }

    template <class T>
    bool operator!= (const linked_ptr<T>& lhs, std::nullptr_t) noexcept
    {
        return (bool)lhs;
    }

    template <class T>
    bool operator!= (std::nullptr_t, const linked_ptr<T>& rhs) noexcept
    {
        return (bool)rhs;
    }

    template <class T>
    bool operator< (const linked_ptr<T>& lhs, std::nullptr_t) noexcept
    {
        return  std::less<T*>()(lhs.get(), nullptr);
    }

    template <class T>
    bool operator<(std::nullptr_t, const linked_ptr<T>& rhs) noexcept
    {
        return std::less<T*>()(nullptr, rhs.get());
    }

    template <class T>
    bool operator<=(const linked_ptr<T>& lhs, std::nullptr_t) noexcept
    {
        return nullptr < lhs;
    }

    template <class T>
    bool operator<=(std::nullptr_t, const linked_ptr<T>& rhs) noexcept
    {
        return rhs < nullptr;
    }

    template <class T>
    bool operator>(const linked_ptr<T>& lhs, std::nullptr_t) noexcept
    {
        return !(nullptr < lhs);
    }

    template <class T>
    bool operator>(std::nullptr_t, const linked_ptr<T>& rhs) noexcept
    {
        return !(rhs < nullptr);
    }

    template <class T>
    bool operator>=(const linked_ptr<T>& lhs, std::nullptr_t) noexcept
    {
        return !(lhs < nullptr);
    }

    template <class T>
    bool operator>=(std::nullptr_t, const linked_ptr<T>& rhs) noexcept
    {
        return !(nullptr < rhs);
    }
}
