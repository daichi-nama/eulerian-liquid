#pragma once

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <vector>

#include "constants.h"

namespace nama {

template <typename T, typename I, int N>
class ArrayBase : private std::vector<T> {
    static_assert(N == 1 || N == 2 || N == 3, "unsupported dimension");
};

template <typename T, typename I>
class ArrayBase<T, I, 1> : private std::vector<T> {
   public:
    using reference              = typename std::vector<T>::reference;
    using const_reference        = typename std::vector<T>::const_reference;
    using iterator               = typename std::vector<T>::iterator;
    using const_iterator         = typename std::vector<T>::const_iterator;
    using size_type              = typename std::vector<T>::size_type;
    using difference_type        = typename std::vector<T>::difference_type;
    using value_type             = typename std::vector<T>::value_type;
    using allocator_type         = typename std::vector<T>::allocator_type;
    using pointer                = typename std::vector<T>::pointer;
    using const_pointer          = typename std::vector<T>::const_pointer;
    using reverse_iterator       = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

    using index_type = I;
    static_assert(std::is_signed<index_type>::value, "index_type should be signed.");

    inline static constexpr int dim = 1;

    using std::vector<T>::begin;
    using std::vector<T>::end;
    using std::vector<T>::cbegin;
    using std::vector<T>::cend;
    using std::vector<T>::crbegin;
    using std::vector<T>::crend;

    using std::vector<T>::size;
    using std::vector<T>::max_size;
    using std::vector<T>::resize;
    using std::vector<T>::capacity;
    using std::vector<T>::empty;
    using std::vector<T>::reserve;
    using std::vector<T>::shrink_to_fit;

    using std::vector<T>::operator[];
    using std::vector<T>::at;
    using std::vector<T>::data;
    using std::vector<T>::front;
    using std::vector<T>::back;

    ArrayBase()                            = default;
    ArrayBase(const ArrayBase&)            = default;
    ArrayBase& operator=(const ArrayBase&) = default;
    ArrayBase(ArrayBase&&)                 = default;
    ArrayBase& operator=(ArrayBase&&)      = default;
    virtual ~ArrayBase()                   = default;

    ArrayBase(index_type ni, const value_type& value = value_type{}) {
        std::vector<T>::resize(ni, value);
    }

    index_type ni() const {
        return std::vector<T>::size();
    }

    index_type calc_index(index_type i) const {
        assert(i >= index_type{} && i < static_cast<index_type>(std::vector<T>::size()));
        return i;
    }

    reference operator()(index_type i) {
        return (*this)[calc_index(i)];
    }

    const_reference operator()(index_type i) const {
        return (*this)[calc_index(i)];
    }
};

template <typename T, typename I>
class ArrayBase<T, I, 2> : private std::vector<T> {
   public:
    using reference              = typename std::vector<T>::reference;
    using const_reference        = typename std::vector<T>::const_reference;
    using iterator               = typename std::vector<T>::iterator;
    using const_iterator         = typename std::vector<T>::const_iterator;
    using size_type              = typename std::vector<T>::size_type;
    using difference_type        = typename std::vector<T>::difference_type;
    using value_type             = typename std::vector<T>::value_type;
    using allocator_type         = typename std::vector<T>::allocator_type;
    using pointer                = typename std::vector<T>::pointer;
    using const_pointer          = typename std::vector<T>::const_pointer;
    using reverse_iterator       = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

    using index_type = I;
    static_assert(std::is_signed<index_type>::value, "index_type should be signed.");

    inline static constexpr int dim = 2;

    using std::vector<T>::begin;
    using std::vector<T>::end;
    using std::vector<T>::cbegin;
    using std::vector<T>::cend;
    using std::vector<T>::crbegin;
    using std::vector<T>::crend;

    using std::vector<T>::size;
    using std::vector<T>::max_size;
    // using std::vector<T>::resize;
    using std::vector<T>::capacity;
    using std::vector<T>::empty;
    using std::vector<T>::reserve;
    using std::vector<T>::shrink_to_fit;

    using std::vector<T>::operator[];
    using std::vector<T>::at;
    using std::vector<T>::data;
    using std::vector<T>::front;
    using std::vector<T>::back;

    ArrayBase()                            = default;
    ArrayBase(const ArrayBase&)            = default;
    ArrayBase& operator=(const ArrayBase&) = default;
    ArrayBase(ArrayBase&&)                 = default;
    ArrayBase& operator=(ArrayBase&&)      = default;
    virtual ~ArrayBase()                   = default;

    ArrayBase(index_type ni, index_type nj, const value_type& value = value_type{})
        : m_ni(ni), m_nj(nj) {
        check_n();
        std::vector<T>::resize(m_ni * m_nj, value);
    }

    void resize(index_type ni, index_type nj, const value_type& value = value_type{}) {
        m_ni = ni;
        m_nj = nj;
        check_n();
        std::vector<T>::resize(m_ni * m_nj, value);
    }

    index_type ni() const {
        return m_ni;
    }

    index_type nj() const {
        return m_nj;
    }

    index_type calc_index(index_type i, index_type j) const {
        assert(i >= index_type{} && i < m_ni);
        assert(j >= index_type{} && j < m_nj);
        return i + m_ni * j;
    }

    reference operator()(index_type i, index_type j) {
        return (*this)[calc_index(i, j)];
    }

    const_reference operator()(index_type i, index_type j) const {
        return (*this)[calc_index(i, j)];
    }

    reference at(index_type i, index_type j) {
        return (*this).at(calc_index(i, j));
    }

    const_reference at(index_type i, index_type j) const {
        return (*this).at(calc_index(i, j));
    }

    void clear() {
        m_ni = {};
        m_nj = {};
        std::vector<T>::clear();
    }

   private:
    void check_n() {
        if (m_ni < index_type{}) {
            throw std::length_error("negative size detected");
        }
        if (m_nj < index_type{}) {
            throw std::length_error("negative size detected");
        }
        if (static_cast<size_type>(m_ni) * m_nj
            >= static_cast<size_type>(std::numeric_limits<index_type>::max())) {
            throw std::length_error("Overflow will occur.");
        }
    }

    index_type m_ni{}, m_nj{};
};

template <typename T, typename I>
class ArrayBase<T, I, 3> : private std::vector<T> {
   public:
    using reference              = typename std::vector<T>::reference;
    using const_reference        = typename std::vector<T>::const_reference;
    using iterator               = typename std::vector<T>::iterator;
    using const_iterator         = typename std::vector<T>::const_iterator;
    using size_type              = typename std::vector<T>::size_type;
    using difference_type        = typename std::vector<T>::difference_type;
    using value_type             = typename std::vector<T>::value_type;
    using allocator_type         = typename std::vector<T>::allocator_type;
    using pointer                = typename std::vector<T>::pointer;
    using const_pointer          = typename std::vector<T>::const_pointer;
    using reverse_iterator       = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

    using index_type = I;
    static_assert(std::is_signed<index_type>::value, "index_type should be signed.");

    inline static constexpr int dim = 3;

    using std::vector<T>::begin;
    using std::vector<T>::end;
    using std::vector<T>::cbegin;
    using std::vector<T>::cend;
    using std::vector<T>::crbegin;
    using std::vector<T>::crend;

    using std::vector<T>::size;
    using std::vector<T>::max_size;
    // using std::vector<T>::resize;
    using std::vector<T>::capacity;
    using std::vector<T>::empty;
    using std::vector<T>::reserve;
    using std::vector<T>::shrink_to_fit;

    using std::vector<T>::operator[];
    using std::vector<T>::at;
    using std::vector<T>::data;
    using std::vector<T>::front;
    using std::vector<T>::back;

    ArrayBase()                            = default;
    ArrayBase(const ArrayBase&)            = default;
    ArrayBase& operator=(const ArrayBase&) = default;
    ArrayBase(ArrayBase&&)                 = default;
    ArrayBase& operator=(ArrayBase&&)      = default;
    virtual ~ArrayBase()                   = default;

    ArrayBase(index_type ni, index_type nj, index_type nk, const value_type& value = value_type{})
        : m_ni(ni), m_nj(nj), m_nk(nk) {
        check_n();
        std::vector<T>::resize(m_ni * m_nj * m_nk, value);
    }

    void resize(index_type ni, index_type nj, index_type nk,
                const value_type& value = value_type{}) {
        m_ni = ni;
        m_nj = nj;
        m_nk = nk;
        check_n();
        std::vector<T>::resize(m_ni * m_nj * m_nk, value);
    }

    index_type ni() const {
        return m_ni;
    }

    index_type nj() const {
        return m_nj;
    }

    index_type nk() const {
        return m_nk;
    }

    index_type calc_index(index_type i, index_type j, index_type k) const {
        assert(i >= index_type{} && i < m_ni);
        assert(j >= index_type{} && j < m_nj);
        assert(k >= index_type{} && k < m_nk);
        return i + m_ni * (j + m_nj * k);
    }

    reference operator()(index_type i, index_type j, index_type k) {
        return (*this)[calc_index(i, j, k)];
    }

    const_reference operator()(index_type i, index_type j, index_type k) const {
        return (*this)[calc_index(i, j, k)];
    }

    reference at(index_type i, index_type j, index_type k) {
        return (*this).at(calc_index(i, j, k));
    }

    const_reference at(index_type i, index_type j, index_type k) const {
        return (*this).at(calc_index(i, j, k));
    }

    void clear() {
        m_ni = {};
        m_nj = {};
        m_nk = {};
        std::vector<T>::clear();
    }

   private:
    void check_n() {
        if (m_ni < index_type{}) {
            throw std::length_error("negative size detected");
        }
        if (m_nj < index_type{}) {
            throw std::length_error("negative size detected");
        }
        if (m_nk < index_type{}) {
            throw std::length_error("negative size detected");
        }
        if (static_cast<size_type>(m_ni) * m_nj * m_nk
            >= static_cast<size_type>(std::numeric_limits<index_type>::max())) {
            throw std::length_error("Overflow will occur.");
        }
    }

    index_type m_ni{}, m_nj{}, m_nk{};
};

template <typename T>
using Array1  = ArrayBase<T, int, 1>;
using Array1i = ArrayBase<int, int, 1>;
using Array1f = ArrayBase<float, int, 1>;
using Array1d = ArrayBase<double, int, 1>;
using Array1c = ArrayBase<char, int, 1>;

template <typename T>
using Array2  = ArrayBase<T, int, 2>;
using Array2i = Array2<int>;
using Array2f = Array2<float>;
using Array2d = Array2<double>;
using Array2c = Array2<char>;

template <typename T>
using Array3  = ArrayBase<T, int, 3>;
using Array3i = Array3<int>;
using Array3f = Array3<float>;
using Array3d = Array3<double>;
using Array3c = Array3<char>;

template <typename T, typename I, typename F>
void serial_loop(const ArrayBase<T, I, 1>& arr, F f) {
    using index_type = typename ArrayBase<T, I, 1>::index_type;
    for (index_type i{}; i < arr.ni(); ++i) {
        f(i);
    }
}

template <typename I, typename F>
void serial_loop(const std::pair<I, I>& i_range, F f) {
    for (I i = i_range.first; i < i_range.second; ++i) {
        f(i);
    }
}

template <typename T, typename I, typename F>
void parallel_loop(const ArrayBase<T, I, 1>& arr, F f) {
    using index_type = typename ArrayBase<T, I, 1>::index_type;
#pragma omp parallel for
    for (index_type i = {}; i < arr.ni(); ++i) {
        f(i);
    }
}

template <typename I, typename F>
void parallel_loop(const std::pair<I, I>& i_range, F f) {
#pragma omp parallel for
    for (I i = i_range.first; i < i_range.second; ++i) {
        f(i);
    }
}

template <typename T, typename I, typename F>
void serial_loop(const ArrayBase<T, I, 2>& arr, F f) {
    using index_type = typename ArrayBase<T, I, 2>::index_type;
    for (index_type j{}; j < arr.nj(); ++j) {
        for (index_type i{}; i < arr.ni(); ++i) {
            f(i, j);
        }
    }
}

template <typename I, typename F>
void serial_loop(const std::pair<I, I>& i_range, const std::pair<I, I>& j_range, F f) {
    for (I j = j_range.first; j < j_range.second; ++j) {
        for (I i = i_range.first; i < i_range.second; ++i) {
            f(i, j);
        }
    }
}

template <typename T, typename I, typename F>
void parallel_loop(const ArrayBase<T, I, 2>& arr, F f) {
    using index_type = typename ArrayBase<T, I, 2>::index_type;
#pragma omp parallel for collapse(2)
    for (index_type j = {}; j < arr.nj(); ++j) {
        for (index_type i = {}; i < arr.ni(); ++i) {
            f(i, j);
        }
    }
}

template <typename I, typename F>
void parallel_loop(const std::pair<I, I>& i_range, const std::pair<I, I>& j_range, F f) {
#pragma omp parallel for collapse(2)
    for (I j = j_range.first; j < j_range.second; ++j) {
        for (I i = i_range.first; i < i_range.second; ++i) {
            f(i, j);
        }
    }
}

template <typename T, typename I, typename F>
void serial_loop(const ArrayBase<T, I, 3>& arr, F f) {
    using index_type = typename ArrayBase<T, I, 3>::index_type;
    for (index_type k{}; k < arr.nk(); ++k) {
        for (index_type j{}; j < arr.nj(); ++j) {
            for (index_type i{}; i < arr.ni(); ++i) {
                f(i, j, k);
            }
        }
    }
}

template <typename I, typename F>
void serial_loop(const std::pair<I, I>& i_range, const std::pair<I, I>& j_range,
                 const std::pair<I, I>& k_range, F f) {
    for (I k = k_range.first; k < k_range.second; ++k) {
        for (I j = j_range.first; j < j_range.second; ++j) {
            for (I i = i_range.first; i < i_range.second; ++i) {
                f(i, j, k);
            }
        }
    }
}

template <typename T, typename I, typename F>
void parallel_loop(const ArrayBase<T, I, 3>& arr, F f) {
    using index_type = typename ArrayBase<T, I, 3>::index_type;
#pragma omp parallel for collapse(3)
    for (index_type k = {}; k < arr.nk(); ++k) {
        for (index_type j = {}; j < arr.nj(); ++j) {
            for (index_type i = {}; i < arr.ni(); ++i) {
                f(i, j, k);
            }
        }
    }
}

template <typename I, typename F>
void parallel_loop(const std::pair<I, I>& i_range, const std::pair<I, I>& j_range,
                   const std::pair<I, I>& k_range, F f) {
#pragma omp parallel for collapse(3)
    for (I k = k_range.first; k < k_range.second; ++k) {
        for (I j = j_range.first; j < j_range.second; ++j) {
            for (I i = i_range.first; i < i_range.second; ++i) {
                f(i, j, k);
            }
        }
    }
}

}  // namespace nama

template <typename T, typename I>
inline std::ostream& operator<<(std::ostream& os, const nama::ArrayBase<T, I, 1>& arr) {
    using namespace nama;
    using namespace nama::internal;
    using index_type = typename ArrayBase<T, I, 1>::index_type;
    for (index_type i{}; i < arr.ni(); ++i) {
        if constexpr (std::is_same<T, char>::value) {
            os << std::right << std::setw(WIDTH) << std::setprecision(PRECISION)
               << static_cast<int>(arr(i));
        } else {
            os << std::right << std::setw(WIDTH) << std::setprecision(PRECISION) << arr(i);
        }
    }
    os << std::endl;
    return os;
}

template <typename T, typename I>
inline std::ostream& operator<<(std::ostream& os, const nama::ArrayBase<T, I, 2>& arr) {
    using namespace nama;
    using namespace nama::internal;
    using index_type = typename ArrayBase<T, I, 2>::index_type;
    for (index_type j = arr.nj() - static_cast<index_type>(1); j >= index_type{}; --j) {
        for (index_type i{}; i < arr.ni(); ++i) {
            if constexpr (std::is_same<T, char>::value) {
                os << std::right << std::setw(WIDTH) << std::setprecision(PRECISION)
                   << static_cast<int>(arr(i, j));
            } else {
                os << std::right << std::setw(WIDTH) << std::setprecision(PRECISION) << arr(i, j);
            }
        }
        os << std::endl;
    }
    return os;
}

template <typename T, typename I>
inline std::ostream& operator<<(std::ostream& os, const nama::ArrayBase<T, I, 3>& arr) {
    using namespace nama;
    using namespace nama::internal;
    using index_type = typename ArrayBase<T, I, 3>::index_type;
    for (index_type k{}; k < arr.nk(); ++k) {
        os << "k = " << k << std::endl;
        for (index_type j = arr.nj() - static_cast<index_type>(1); j >= index_type{}; --j) {
            for (index_type i{}; i < arr.ni(); ++i) {
                if constexpr (std::is_same<T, char>::value) {
                    os << std::right << std::setw(WIDTH) << std::setprecision(PRECISION)
                       << static_cast<int>(arr(i, j, k));
                } else {
                    os << std::right << std::setw(WIDTH) << std::setprecision(PRECISION)
                       << arr(i, j, k);
                }
            }
            os << std::endl;
        }
    }
    return os;
}
