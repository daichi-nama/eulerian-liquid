#pragma once

#include <Eigen/Core>
#include <algorithm>
#include <filesystem>

#include "array.h"
#include "constants.h"

namespace nama {

template <typename T, typename U, typename UV, typename I, int N>
class GridBase : public ArrayBase<T, I, N> {
    static_assert(N == 1 || N == 2 || N == 3, "unsupported dimension");
};

template <typename T, typename U, typename UV, typename I>
class GridBase<T, U, UV, I, 1> : public ArrayBase<T, I, 1> {
   public:
    using reference              = typename ArrayBase<T, I, 1>::reference;
    using const_reference        = typename ArrayBase<T, I, 1>::const_reference;
    using iterator               = typename ArrayBase<T, I, 1>::iterator;
    using const_iterator         = typename ArrayBase<T, I, 1>::const_iterator;
    using size_type              = typename ArrayBase<T, I, 1>::size_type;
    using difference_type        = typename ArrayBase<T, I, 1>::difference_type;
    using value_type             = typename ArrayBase<T, I, 1>::value_type;
    using allocator_type         = typename ArrayBase<T, I, 1>::allocator_type;
    using pointer                = typename ArrayBase<T, I, 1>::pointer;
    using const_pointer          = typename ArrayBase<T, I, 1>::const_pointer;
    using reverse_iterator       = typename ArrayBase<T, I, 1>::reverse_iterator;
    using const_reverse_iterator = typename ArrayBase<T, I, 1>::const_reverse_iterator;

    using index_type = typename ArrayBase<T, I, 1>::index_type;

    using coordinate_type = U;
    using vector_type     = U;

    using ArrayBase<T, I, 1>::ni;
    using ArrayBase<T, I, 1>::resize;

    GridBase()                           = default;
    GridBase(const GridBase&)            = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase(GridBase&&)                 = default;
    GridBase& operator=(GridBase&&)      = default;
    virtual ~GridBase()                  = default;

    GridBase(index_type ni_, coordinate_type dx, vector_type origin,
             const value_type& value = value_type{})
        : ArrayBase<T, I, 1>(ni_, value), m_dx(dx), m_origin(origin) {
        assert(m_dx > coordinate_type{});
    }

    void create(index_type ni_, coordinate_type dx, vector_type origin,
                const value_type& value = value_type{}) {
        m_dx = dx;
        assert(m_dx > coordinate_type{});
        m_origin = origin;
        resize(ni_, value);
    }

    template <typename TT>
    void reshape_like(const GridBase<TT, U, UV, I, 1>& g) {
        create(g.ni(), g.dx(), g.origin());
    }

    coordinate_type dx() const {
        return m_dx;
    }

    coordinate_type li() const {
        return m_dx * ni();
    }

    vector_type origin() const {
        return m_origin;
    }

    vector_type pos(index_type i) const {
        assert((ni() > index_type{}));
        assert((i >= index_type{} && i < ni()));
        return vector_type(i + static_cast<coordinate_type>(0.5)) * m_dx + m_origin;
    }

    const_reference extrapolate(index_type i) const {
        assert((ni() > index_type{}));
        i = std::clamp(i, {}, ni() - static_cast<index_type>(1));
        return (*this)[i];
    }

    template <typename F>
    void set(F f) {
        parallel_loop(*this, [&](index_type i) { (*this)(i) = f(pos(i)); });
    }

    // io
    void save(const std::filesystem::path& output_path) const {
        if (output_path.extension() == ".txt") {
            std::ofstream file(output_path);
            if (!file) {
                throw std::runtime_error("File '" + output_path.string() + "' not open.");
            }
            for (index_type i{}; i < ni(); ++i) {
                file << pos(i);
                if (i != ni() - static_cast<index_type>(1)) {
                    file << ',';
                } else {
                    file << std::endl;
                }
            }
            for (index_type i{}; i < ni(); ++i) {
                file << (*this)(i);
                if (i != ni() - static_cast<index_type>(1)) {
                    file << ',';
                } else {
                    file << std::endl;
                }
            }
        } else {
            throw std::invalid_argument("unsupported extension");
        }
    }

   private:
    coordinate_type m_dx;
    vector_type m_origin;
};

template <typename T, typename U, typename UV, typename I>
class GridBase<T, U, UV, I, 2> : public ArrayBase<T, I, 2> {
   public:
    using reference              = typename ArrayBase<T, I, 2>::reference;
    using const_reference        = typename ArrayBase<T, I, 2>::const_reference;
    using iterator               = typename ArrayBase<T, I, 2>::iterator;
    using const_iterator         = typename ArrayBase<T, I, 2>::const_iterator;
    using size_type              = typename ArrayBase<T, I, 2>::size_type;
    using difference_type        = typename ArrayBase<T, I, 2>::difference_type;
    using value_type             = typename ArrayBase<T, I, 2>::value_type;
    using allocator_type         = typename ArrayBase<T, I, 2>::allocator_type;
    using pointer                = typename ArrayBase<T, I, 2>::pointer;
    using const_pointer          = typename ArrayBase<T, I, 2>::const_pointer;
    using reverse_iterator       = typename ArrayBase<T, I, 2>::reverse_iterator;
    using const_reverse_iterator = typename ArrayBase<T, I, 2>::const_reverse_iterator;

    using index_type = typename ArrayBase<T, I, 2>::index_type;

    using coordinate_type = U;
    using vector_type     = UV;

    using ArrayBase<T, I, 2>::ni;
    using ArrayBase<T, I, 2>::nj;
    using ArrayBase<T, I, 2>::resize;

    static_assert(std::is_same<coordinate_type, typename vector_type::Scalar>::value);

    GridBase()                           = default;
    GridBase(const GridBase&)            = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase(GridBase&&)                 = default;
    GridBase& operator=(GridBase&&)      = default;
    virtual ~GridBase()                  = default;

    GridBase(index_type ni_, index_type nj_, coordinate_type dx, const vector_type& origin,
             const value_type& value = value_type{})
        : ArrayBase<T, I, 2>(ni_, nj_, value), m_dx(dx), m_origin(origin) {
        assert(m_dx > coordinate_type{});
    }

    void create(index_type ni_, index_type nj_, coordinate_type dx, const vector_type& origin,
                const value_type& value = value_type{}) {
        m_dx = dx;
        assert(m_dx > coordinate_type{});
        m_origin = origin;
        resize(ni_, nj_, value);
    }

    template <typename TT>
    void reshape_like(const GridBase<TT, U, UV, I, 2>& g) {
        create(g.ni(), g.nj(), g.dx(), g.origin());
    }

    coordinate_type dx() const {
        return m_dx;
    }

    coordinate_type li() const {
        return m_dx * ni();
    }

    coordinate_type lj() const {
        return m_dx * nj();
    }

    vector_type origin() const {
        return m_origin;
    }

    vector_type pos(index_type i, index_type j) const {
        assert((ni() > index_type{}));
        assert((nj() > index_type{}));
        assert((i >= index_type{} && i < ni()));
        assert((j >= index_type{} && j < nj()));
        return vector_type(i + static_cast<coordinate_type>(0.5),
                           j + static_cast<coordinate_type>(0.5))
                   * m_dx
               + m_origin;
    }

    const_reference extrapolate(index_type i, index_type j) const {
        assert((ni() > index_type{}));
        assert((nj() > index_type{}));
        i = std::clamp(i, {}, ni() - static_cast<index_type>(1));
        j = std::clamp(j, {}, nj() - static_cast<index_type>(1));
        return (*this)[i + ni() * j];
    }

    GridBase<T, U, U, I, 1> slice_i(index_type i_) const {
        GridBase<T, U, U, I, 1> result(nj(), dx(), origin()[1]);
        for (index_type j{}; j < nj(); ++j) {
            result(j) = (*this)(i_, j);
        }
        return result;
    }

    GridBase<T, U, U, I, 1> slice_j(index_type j_) const {
        GridBase<T, U, U, I, 1> result(ni(), dx(), origin()[0]);
        for (index_type i{}; i < ni(); ++i) {
            result(i) = (*this)(i, j_);
        }
        return result;
    }

    template <typename F>
    void set(F f) {
        parallel_loop(*this, [&](index_type i, index_type j) { (*this)(i, j) = f(pos(i, j)); });
    }

    // io
    void save(const std::filesystem::path& output_path) const {
        if (output_path.extension() == ".txt") {
            std::ofstream file(output_path);
            if (!file) {
                throw std::runtime_error("File '" + output_path.string() + "' not open.");
            }
            for (index_type i{}; i < ni(); ++i) {
                file << pos(i, {})[0];
                if (i != ni() - static_cast<index_type>(1)) {
                    file << ',';
                } else {
                    file << std::endl;
                }
            }
            for (index_type j{}; j < nj(); ++j) {
                file << pos({}, j)[1];
                if (j != nj() - static_cast<index_type>(1)) {
                    file << ',';
                } else {
                    file << std::endl;
                }
            }
            for (index_type j{}; j < nj(); ++j) {
                for (index_type i{}; i < ni(); ++i) {
                    file << (*this)(i, j);
                    if (i != ni() - static_cast<index_type>(1)) {
                        file << ',';
                    } else {
                        file << std::endl;
                    }
                }
            }
        } else {
            throw std::invalid_argument("unsupported extension");
        }
    }

   private:
    coordinate_type m_dx;
    vector_type m_origin;
};

template <typename T, typename U, typename UV, typename I>
class GridBase<T, U, UV, I, 3> : public ArrayBase<T, I, 3> {
   public:
    using reference              = typename ArrayBase<T, I, 3>::reference;
    using const_reference        = typename ArrayBase<T, I, 3>::const_reference;
    using iterator               = typename ArrayBase<T, I, 3>::iterator;
    using const_iterator         = typename ArrayBase<T, I, 3>::const_iterator;
    using size_type              = typename ArrayBase<T, I, 3>::size_type;
    using difference_type        = typename ArrayBase<T, I, 3>::difference_type;
    using value_type             = typename ArrayBase<T, I, 3>::value_type;
    using allocator_type         = typename ArrayBase<T, I, 3>::allocator_type;
    using pointer                = typename ArrayBase<T, I, 3>::pointer;
    using const_pointer          = typename ArrayBase<T, I, 3>::const_pointer;
    using reverse_iterator       = typename ArrayBase<T, I, 3>::reverse_iterator;
    using const_reverse_iterator = typename ArrayBase<T, I, 3>::const_reverse_iterator;

    using index_type = typename ArrayBase<T, I, 3>::index_type;

    using coordinate_type = U;
    using vector_type     = UV;

    using ArrayBase<T, I, 3>::ni;
    using ArrayBase<T, I, 3>::nj;
    using ArrayBase<T, I, 3>::nk;
    using ArrayBase<T, I, 3>::resize;

    static_assert(std::is_same<coordinate_type, typename vector_type::Scalar>::value);

    GridBase()                           = default;
    GridBase(const GridBase&)            = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase(GridBase&&)                 = default;
    GridBase& operator=(GridBase&&)      = default;
    virtual ~GridBase()                  = default;

    GridBase(index_type ni_, index_type nj_, index_type nk_, coordinate_type dx,
             const vector_type& origin, const value_type& value = value_type{})
        : ArrayBase<T, I, 3>(ni_, nj_, nk_, value), m_dx(dx), m_origin(origin) {
        assert(m_dx > coordinate_type{});
    }

    void create(index_type ni_, index_type nj_, index_type nk_, coordinate_type dx,
                const vector_type& origin, const value_type& value = value_type{}) {
        m_dx = dx;
        assert(m_dx > coordinate_type{});
        m_origin = origin;
        resize(ni_, nj_, nk_, value);
    }

    template <typename TT>
    void reshape_like(const GridBase<TT, U, UV, I, 3>& g) {
        create(g.ni(), g.nj(), g.nk(), g.dx(), g.origin());
    }

    coordinate_type dx() const {
        return m_dx;
    }

    coordinate_type li() const {
        return m_dx * ni();
    }

    coordinate_type lj() const {
        return m_dx * nj();
    }

    coordinate_type lk() const {
        return m_dx * nk();
    }

    vector_type origin() const {
        return m_origin;
    }

    vector_type pos(index_type i, index_type j, index_type k) const {
        assert((ni() > index_type{}));
        assert((nj() > index_type{}));
        assert((nk() > index_type{}));
        assert((i >= index_type{} && i < ni()));
        assert((j >= index_type{} && j < nj()));
        assert((k >= index_type{} && k < nk()));
        return vector_type(i + static_cast<coordinate_type>(0.5),
                           j + static_cast<coordinate_type>(0.5),
                           k + static_cast<coordinate_type>(0.5))
                   * m_dx
               + m_origin;
    }

    const_reference extrapolate(index_type i, index_type j, index_type k) const {
        assert((ni() > index_type{}));
        assert((nj() > index_type{}));
        assert((nk() > index_type{}));
        i = std::clamp(i, {}, ni() - static_cast<index_type>(1));
        j = std::clamp(j, {}, nj() - static_cast<index_type>(1));
        k = std::clamp(k, {}, nk() - static_cast<index_type>(1));
        return (*this)[i + ni() * (j + nj() * k)];
    }

    template <typename UV2 = Eigen::Vector2f>
    GridBase<T, U, UV2, I, 2> slice_i(index_type i_) const {
        GridBase<T, U, UV2, I, 2> result(nk(), nj(), dx(), UV2(origin()[2], origin()[1]));
        for (index_type j{}; j < nj(); ++j) {
            for (index_type k{}; k < nk(); ++k) {
                result(k, j) = (*this)(i_, j, k);
            }
        }
        return result;
    }

    template <typename UV2 = Eigen::Vector2f>
    GridBase<T, U, UV2, I, 2> slice_j(index_type j_) const {
        GridBase<T, U, UV2, I, 2> result(ni(), nk(), dx(), UV2(origin()[0], origin()[2]));
        for (index_type k{}; k < nk(); ++k) {
            for (index_type i{}; i < ni(); ++i) {
                result(i, k) = (*this)(i, j_, k);
            }
        }
        return result;
    }

    template <typename UV2 = Eigen::Vector2f>
    GridBase<T, U, UV2, I, 2> slice_k(index_type k_) const {
        GridBase<T, U, UV2, I, 2> result(ni(), nj(), dx(), UV2(origin()[0], origin()[1]));
        for (index_type j{}; j < nj(); ++j) {
            for (index_type i{}; i < ni(); ++i) {
                result(i, j) = (*this)(i, j, k_);
            }
        }
        return result;
    }

    template <typename F>
    void set(F f) {
        parallel_loop(*this, [&](index_type i, index_type j, index_type k) {
            (*this)(i, j, k) = f(pos(i, j, k));
        });
    }

   private:
    coordinate_type m_dx;
    vector_type m_origin;
};

template <typename T>
using Grid1  = GridBase<T, float, float, int, 1>;
using Grid1i = Grid1<int>;
using Grid1f = Grid1<float>;
using Grid1d = Grid1<double>;
using Grid1c = Grid1<char>;

template <typename T>
using Grid2  = GridBase<T, float, Eigen::Vector2f, int, 2>;
using Grid2i = Grid2<int>;
using Grid2f = Grid2<float>;
using Grid2d = Grid2<double>;
using Grid2c = Grid2<char>;

template <typename T>
using Grid3  = GridBase<T, float, Eigen::Vector3f, int, 3>;
using Grid3i = Grid3<int>;
using Grid3f = Grid3<float>;
using Grid3d = Grid3<double>;
using Grid3c = Grid3<char>;

}  // namespace nama
