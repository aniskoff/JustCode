#pragma once
#include <array>


namespace allocators
{
    template <typename T, const size_t slab_size>
    class slab;

    namespace details
    {
        namespace math
        {
            inline size_t round_up_2th_pow(size_t v)
            {
                v--;
                v |= v >> 1;
                v |= v >> 2;
                v |= v >> 4;
                v |= v >> 8;
                v |= v >> 16;
                v |= v >> 32;
                v++;
                return v;
            }

            inline size_t round_down_2th_pow(size_t x)
            {
                x = x | (x >> 1);
                x = x | (x >> 2);
                x = x | (x >> 4);
                x = x | (x >> 8);
                x = x | (x >> 16);
                x = x | (x >> 32);
                return x - (x >> 1);
            }

            inline size_t ilog2(size_t v) // such that ilog2(0) == 0
            {
                const size_t b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000, 0xFFFFFFFF00000000};
                const size_t S[] = {1, 2, 4, 8, 16, 32};

                size_t r = 0;
                for (int i = 5; i >= 0; --i)
                {
                    if (v & b[i]) {
                        v >>= S[i];
                        r |= S[i];
                    }
                }
                return r;
            }

            inline size_t power_of_2(size_t i)
            {
                size_t res = 1;
                return res << i;
            }

            inline size_t safe_multiply(size_t a, size_t b)
            {
                double res = static_cast<double>(a) * static_cast<double>(b);
                if (res > static_cast<double>(SIZE_MAX))
                    return SIZE_MAX;
                return static_cast<size_t>(res);
            }
        }


        class Slab_descriptor;
        inline size_t get_total_descriptor_size(size_t);
        inline Slab_descriptor* find_descriptor(void*, size_t);
        inline Slab_descriptor* find_descriptor_by_ptr(void*, size_t);
        inline void* get_free_chunk_by_offset(Slab_descriptor*, size_t, size_t);
        inline size_t get_cache_ind(size_t);
        inline void move_slab_from_to(Slab_descriptor*, Slab_descriptor*&, Slab_descriptor*&);
        inline void free_slabs(Slab_descriptor*&, size_t);

        class Intrusive_base
        {
        public:
            Intrusive_base* next_chunk;
            Intrusive_base() = default;
            explicit Intrusive_base(void* ptr)
                    :next_chunk(static_cast<Intrusive_base*>(ptr))
            {}
        };

        inline size_t get_allocatable_chunks_num(size_t, size_t);


        class Slab_descriptor
        {
        public:
            size_t offset; //start with 1
            size_t used_chunks_num;
            Intrusive_base*  free_chunks;
            Slab_descriptor* next_slab;
            Slab_descriptor* prev_slab;
            Slab_descriptor()
                    :offset(1)
                    ,used_chunks_num(0)
                    ,free_chunks(nullptr)
                    ,next_slab(nullptr)
                    ,prev_slab(nullptr)
            {}

            bool is_slab_full(size_t slab_size, size_t chunk_size) const
            {
                return used_chunks_num == get_allocatable_chunks_num(slab_size, chunk_size);
            }

            bool has_last_free_chunk(size_t slab_size, size_t chunk_size) const
            {
                return used_chunks_num == get_allocatable_chunks_num(slab_size, chunk_size) - 1;
            }

            bool is_free_chunk_available_by_offset(size_t slab_size, size_t chunk_size) const
            {
                return offset <= get_allocatable_chunks_num(slab_size, chunk_size);
            }

            bool is_only_one_chunk_occupied() const
            {
                return used_chunks_num == 1;
            }

            size_t count_slabs()
            {
                size_t cnt = 0;
                for (details::Slab_descriptor* it = this; it != nullptr; it = it->next_slab)
                    ++cnt;
                return cnt;
            }

            size_t get_capacity(size_t slab_size, size_t cache_alloc_size)
            {
                size_t cnt = 0;
                for (details::Slab_descriptor* it = this; it != nullptr; it = it->next_slab)
                    cnt += details::get_allocatable_chunks_num(slab_size ,cache_alloc_size);
                return cnt;
            }

            size_t get_size()
            {
                size_t cnt = 0;
                for (details::Slab_descriptor* it = this; it != nullptr; it = it->next_slab)
                    cnt += it->used_chunks_num;
                return cnt;
            }
        };


        class Cache
        {
        public:
            Cache(size_t slab_sz, size_t chunk_sz)
                    :empty_slabs(nullptr)
                    ,partial_slabs(nullptr)
                    ,full_slabs(nullptr)
                    ,slab_size(slab_sz)
                    ,chunk_size(chunk_sz)
            {}
            Slab_descriptor* empty_slabs;
            Slab_descriptor* partial_slabs;
            Slab_descriptor* full_slabs;
            size_t slab_size;
            size_t chunk_size;

            void* get_free_chunk_from_partial()
            {
                auto* head_slab_d = partial_slabs;
                if (head_slab_d->is_free_chunk_available_by_offset(slab_size, chunk_size))
                {
                    if (head_slab_d->has_last_free_chunk(slab_size, chunk_size))// if it was the last chunk
                    {
                        move_slab_from_to(head_slab_d, partial_slabs, full_slabs);
                    }

                    ++head_slab_d->used_chunks_num;
                    return get_free_chunk_by_offset(head_slab_d, chunk_size, head_slab_d->offset++);
                }

                auto* free_chunk = head_slab_d->free_chunks;
                head_slab_d->free_chunks = head_slab_d->free_chunks->next_chunk;
                if (head_slab_d->has_last_free_chunk(slab_size, chunk_size)) // if it was the last chunk
                    move_slab_from_to(head_slab_d, partial_slabs, full_slabs);

                ++head_slab_d->used_chunks_num;
                return free_chunk;
            }

            void* get_free_chunk_from_empty()
            {
                auto* head_slab_d = empty_slabs;
                move_slab_from_to(head_slab_d, empty_slabs, partial_slabs);

                ++head_slab_d->used_chunks_num;
                return get_free_chunk_by_offset(head_slab_d, chunk_size, head_slab_d->offset++);
            }

            void create_empty_slab()
            {
                void* new_slab = std::aligned_alloc(slab_size, slab_size);
                auto* slab_d = new(find_descriptor(new_slab, slab_size)) Slab_descriptor;
                empty_slabs = slab_d;
            }

            void free_chunk(void* chunk_ptr)
            {
                auto* slab_d = find_descriptor_by_ptr(chunk_ptr, slab_size);
                if (slab_d->is_slab_full(slab_size, chunk_size))
                {
                    slab_d->free_chunks = new(chunk_ptr) details::Intrusive_base();
                    move_slab_from_to(slab_d, full_slabs, partial_slabs);

                    --slab_d->used_chunks_num;
                    return;
                }

                if (slab_d->is_only_one_chunk_occupied())
                {
                    slab_d->offset = 1;
                    slab_d->free_chunks = nullptr;
                    move_slab_from_to(slab_d, partial_slabs, empty_slabs);

                    --slab_d->used_chunks_num;
                    return;
                }

                slab_d->free_chunks = new(chunk_ptr) details::Intrusive_base(slab_d->free_chunks);

                --slab_d->used_chunks_num;
            }
        };


        template <const size_t slab_size>
        class Cache_list
        {
        public:
            inline static std::array<Cache, 8> caches = {Cache(slab_size,   8), Cache(slab_size,   16),
                                                         Cache(slab_size,  32), Cache(slab_size,   64),
                                                         Cache(slab_size, 128), Cache(slab_size,  256),
                                                         Cache(slab_size, 512), Cache(slab_size, 1024)};
        };


        inline Slab_descriptor* find_descriptor(void* slab, size_t slab_size)
        {
            return reinterpret_cast<Slab_descriptor*>(static_cast<char*>(slab) + slab_size - sizeof(Slab_descriptor));
        }

        inline size_t get_total_descriptor_size(size_t chunk_size)
        {
            return ((sizeof(Slab_descriptor) + chunk_size - 1) / chunk_size) * chunk_size;
        }

        inline size_t get_allocatable_space_size(size_t slab_size, size_t chunk_size)
        {
            return slab_size - get_total_descriptor_size(chunk_size);
        }

        inline size_t get_allocatable_chunks_num(size_t slab_size, size_t chunk_size)
        {
            return get_allocatable_space_size(slab_size, chunk_size) / chunk_size;
        }

        inline void* get_free_chunk_by_offset(Slab_descriptor* slab_decsr, size_t chunk_size, size_t offset)
        {
            return reinterpret_cast<char*>(slab_decsr) -
            (get_total_descriptor_size(chunk_size) - sizeof(Slab_descriptor)) - offset * chunk_size;
        }

        inline size_t get_cache_ind(size_t size)
        {
            size_t cache_min_size = 8;
            size_t cache_min_2th_pow = math::ilog2(cache_min_size);
            size_t cache_size = std::max(cache_min_size, math::round_up_2th_pow(size));
            return math::ilog2(cache_size) - cache_min_2th_pow;
        }

        inline void* find_slab_by_ptr(void* p, size_t slab_size)
        {
            size_t dist_from_beg_to_p = reinterpret_cast<uintptr_t>(p) & (slab_size - 1);
            return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p) - dist_from_beg_to_p);
        }

        inline Slab_descriptor* find_descriptor_by_ptr(void* p, size_t slab_size)
        {
            return find_descriptor(find_slab_by_ptr(p, slab_size), slab_size);
        }

        inline void move_slab_from_to(Slab_descriptor* to_insert_slab_descr, Slab_descriptor*& from, Slab_descriptor*& to)
        {
            if (from == to_insert_slab_descr)
                from = to_insert_slab_descr->next_slab;

            if (to_insert_slab_descr->prev_slab)
                to_insert_slab_descr->prev_slab->next_slab = to_insert_slab_descr->next_slab;

            if (to_insert_slab_descr->next_slab)
                to_insert_slab_descr->next_slab->prev_slab = to_insert_slab_descr->prev_slab;

            Slab_descriptor* tmp = to;
            to = to_insert_slab_descr;
            if (tmp)
            {
                tmp->prev_slab = to_insert_slab_descr;
            }
            to_insert_slab_descr->next_slab = tmp;
            to_insert_slab_descr->prev_slab = nullptr;
        }

        inline void free_slabs(Slab_descriptor*& head, size_t slab_size)
        {
            if (!head)
                return;
            while (head)
            {
                auto* next = head->next_slab;
                free(find_slab_by_ptr(head, slab_size));
                head = next;
            }
        }
    }


    template <typename T, const size_t slab_size = 4096>
    class slab
    {
    public:
        using value_type      = T;
        using pointer         = T*;
        using const_pointer   = const T*;
        using reference       = T&;
        using const_reference = const T&;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;

        inline static const size_t max_cache_alloc_size = 1024;
        inline static const size_t min_cache_alloc_size = 8;
        inline static const size_t num_of_caches        = 8;

        static_assert(slab_size >= 4096 && (slab_size & (slab_size - 1)) == 0,
                          "slab size must be power of 2 and not less than 4Kb");

        template <class Other_t>
        class rebind
        {
        public:
            using other = slab<Other_t, slab_size>;
        };

        slab() = default;

        slab(const slab& ) noexcept = default;

        slab& operator=(slab const &) = delete;

        template <typename U>
        explicit slab(const slab<U, slab_size>&) //constructs such that slab(other) == other
        {}

        slab(slab&&) noexcept = default;

        template <typename U>
        explicit slab(slab<U, slab_size>&&) noexcept //constructs such that slab(other) == other
        {}

        pointer allocate(size_t n);

        void deallocate(pointer p, size_t n);

        void clear();

        size_t allocated_slabs(size_t cache_alloc_size) const;

        size_t capacity(size_t cache_alloc_size) const;

        size_t size(size_t cache_alloc_size) const;

    private:
        std::array<details::Cache, num_of_caches>& caches = details::Cache_list<slab_size>::caches;
    };


    template<typename T, const size_t slab_size>
    typename slab<T, slab_size>::pointer  slab<T, slab_size>::allocate(size_t n)
    {
        size_t size = details::math::safe_multiply(sizeof(T), n);

        if (size == SIZE_MAX)
            throw std::bad_alloc();

        if (size > max_cache_alloc_size)
        {
            size_t alignment = details::math::round_down_2th_pow(size);
            void* p = nullptr;
            posix_memalign(&p, alignment, size);
            return static_cast<pointer>(p);
        }

        size_t ind = details::get_cache_ind(size);
        details::Cache& cache = caches[ind];

        if (cache.partial_slabs)
        {
            return  static_cast<pointer>(cache.get_free_chunk_from_partial());
        }

        if (cache.empty_slabs)
        {
            return  static_cast<pointer>(cache.get_free_chunk_from_empty());
        }

        cache.create_empty_slab();

        return  static_cast<pointer>(cache.get_free_chunk_from_empty());
    }

    template<typename T, const size_t slab_size>
    void slab<T, slab_size>::deallocate(typename slab<T, slab_size>::pointer p, size_t n)
    {
        if (!p)
            return;

        size_t size = details::math::safe_multiply(sizeof(T), n);

        if (size > max_cache_alloc_size)
        {
            free(p);
            return;
        }

        size_t chunk_size = std::max(min_cache_alloc_size, details::math::round_up_2th_pow(size));
        size_t ind = details::get_cache_ind(chunk_size);
        details::Cache& cache = caches[ind];
        cache.free_chunk(p);
    }

    template <typename T, const size_t slab_size>
    void slab<T, slab_size>::clear()
    {
        for (auto& cache: caches)
        {
            free_slabs(cache.empty_slabs, slab_size);
            cache.empty_slabs = nullptr;
        }
    }

    template <typename T, const size_t slab_size>
    size_t slab<T, slab_size>::allocated_slabs(size_t cache_alloc_size) const
    {
        size_t ind = details::get_cache_ind(cache_alloc_size);
        const details::Cache& cache = caches[ind];
        return  cache.empty_slabs->count_slabs()
              + cache.partial_slabs->count_slabs()
              + cache.full_slabs->count_slabs();
    }

    template <typename T, const size_t slab_size>
    size_t slab<T, slab_size>::capacity(size_t cache_alloc_size) const
    {
        size_t ind = details::get_cache_ind(cache_alloc_size);
        const details::Cache& cache = caches[ind];
        return  cache.empty_slabs->get_capacity(slab_size, cache_alloc_size)
              + cache.partial_slabs->get_capacity(slab_size, cache_alloc_size)
              + cache.full_slabs->get_capacity(slab_size, cache_alloc_size);
    }

    template <typename T, const size_t slab_size>
    size_t slab<T, slab_size>::size(size_t cache_alloc_size) const
    {
        if (cache_alloc_size > max_cache_alloc_size)
            return 0;
        size_t ind = details::get_cache_ind(cache_alloc_size);
        const details::Cache& cache = caches[ind];
        return  cache.empty_slabs->get_size()
              + cache.partial_slabs->get_size()
              + cache.full_slabs->get_size();
    }

    template<typename T, typename U, size_t slab_size_1, size_t slab_size_2>
    bool operator==(const slab<T, slab_size_1>&, const slab<U, slab_size_2>&)
    {
        return slab_size_1 == slab_size_2;
    }

    template<typename T, typename U, size_t slab_size_1, size_t slab_size_2>
    bool operator!=(const slab<T, slab_size_1>& lhs, const slab<U, slab_size_2>& rhs)
    {
        return !(lhs == rhs);
    }
}
