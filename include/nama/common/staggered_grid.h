#pragma once

#include <algorithm>
#include <array>
#include <tuple>
#include <type_traits>

#include "grid.h"

namespace nama {

template <typename T, typename U, typename UV, typename I, int N>
class StaggeredGrid {
   public:
    using grid_type            = GridBase<T, U, UV, I, N>;
    using grid_reference       = GridBase<T, U, UV, I, N>&;
    using grid_const_reference = const GridBase<T, U, UV, I, N>&;

    using reference              = typename grid_type::reference;
    using const_reference        = typename grid_type::const_reference;
    using iterator               = typename grid_type::iterator;
    using const_iterator         = typename grid_type::const_iterator;
    using size_type              = typename grid_type::size_type;
    using difference_type        = typename grid_type::difference_type;
    using value_type             = typename grid_type::value_type;
    using allocator_type         = typename grid_type::allocator_type;
    using pointer                = typename grid_type::pointer;
    using const_pointer          = typename grid_type::const_pointer;
    using reverse_iterator       = typename grid_type::reverse_iterator;
    using const_reverse_iterator = typename grid_type::const_reverse_iterator;

    using index_type = typename grid_type::index_type;

    using coordinate_type = typename grid_type::coordinate_type;
    using vector_type     = typename grid_type::vector_type;

    inline static constexpr int dim = N;
    static_assert(dim == 1 || dim == 2 || dim == 3, "unsupported dimension");
    static_assert(dim == grid_type::dim, "dimension mismatch");

    StaggeredGrid() = default;

    template <typename A = StaggeredGrid>
    StaggeredGrid(index_type ni_, coordinate_type dx, vector_type origin,
                  const value_type& value                       = value_type{},
                  std::enable_if_t<A::dim == 1, std::nullptr_t> = nullptr)
        : m_ni(ni_)
        , m_dx(dx)
        , m_origin(origin)
        , m_data{grid_type(ni_ + 1, dx, origin - static_cast<vector_type>(0.5), value)} {}

    StaggeredGrid(const std::array<grid_type, dim>& data) : m_data(data) {}

    template <typename A = StaggeredGrid>
    StaggeredGrid(index_type ni_, index_type nj_, coordinate_type dx, const vector_type& origin,
                  const value_type& value                       = value_type{},
                  std::enable_if_t<A::dim == 2, std::nullptr_t> = nullptr)
        : m_ni(ni_)
        , m_nj(nj_)
        , m_dx(dx)
        , m_origin(origin)
        , m_data{grid_type(ni_ + 1, nj_, dx, origin - vector_type(0.5, 0) * dx, value),
                 grid_type(ni_, nj_ + 1, dx, origin - vector_type(0, 0.5) * dx, value)} {}

    template <typename A = StaggeredGrid>
    StaggeredGrid(index_type ni_, index_type nj_, index_type nk_, coordinate_type dx,
                  const vector_type& origin, const value_type& value = value_type{},
                  std::enable_if_t<A::dim == 3, std::nullptr_t> = nullptr)
        : m_ni(ni_)
        , m_nj(nj_)
        , m_nk(nk_)
        , m_dx(dx)
        , m_origin(origin)
        , m_data{grid_type(ni_ + 1, nj_, nk_, dx, origin - vector_type(0.5, 0, 0) * dx, value),
                 grid_type(ni_, nj_ + 1, nk_, dx, origin - vector_type(0, 0.5, 0) * dx, value),
                 grid_type(ni_, nj_, nk_ + 1, dx, origin - vector_type(0, 0, 0.5) * dx, value)} {}

    template <typename A = StaggeredGrid, std::enable_if_t<A::dim == 1, std::nullptr_t> = nullptr>
    void create(index_type ni_, coordinate_type dx, vector_type origin,
                const value_type& value = value_type{}) {
        m_ni = ni_;
        m_dx = dx;
        assert(m_dx > coordinate_type{});
        m_origin = origin;
        m_data[0].create(ni_ + 1, dx, origin - static_cast<vector_type>(0.5), value);
    }

    template <typename A = StaggeredGrid, std::enable_if_t<A::dim == 2, std::nullptr_t> = nullptr>
    void create(index_type ni_, index_type nj_, coordinate_type dx, const vector_type& origin,
                const value_type& value = value_type{}) {
        m_ni = ni_;
        m_nj = nj_;
        m_dx = dx;
        assert(m_dx > coordinate_type{});
        m_origin = origin;
        m_data[0].create(ni_ + 1, nj_, dx, origin - vector_type(0.5, 0) * dx, value);
        m_data[1].create(ni_, nj_ + 1, dx, origin - vector_type(0, 0.5) * dx, value);
    }

    template <typename A = StaggeredGrid, std::enable_if_t<A::dim == 3, std::nullptr_t> = nullptr>
    void create(index_type ni_, index_type nj_, index_type nk_, coordinate_type dx,
                vector_type origin, const value_type& value = value_type{}) {
        m_ni = ni_;
        m_nj = nj_;
        m_nk = nk_;
        m_dx = dx;
        assert(m_dx > coordinate_type{});
        m_origin = origin;
        m_data[0].create(ni_ + 1, nj_, nk_, dx, origin - vector_type(0.5, 0, 0) * dx, value);
        m_data[1].create(ni_, nj_ + 1, nk_, dx, origin - vector_type(0, 0.5, 0) * dx, value);
        m_data[2].create(ni_, nj_, nk_ + 1, dx, origin - vector_type(0, 0, 0.5) * dx, value);
    }

    void reshape_like(const StaggeredGrid& grid) {
        for (int d = 0; d < dim; ++d) {
            m_data[d].reshape_like(grid[d]);
        }
    }

    grid_const_reference u() const {
        static_assert(dim >= 1, "out of range");
        return m_data[0];
    }

    grid_reference u() {
        static_assert(dim >= 1, "out of range");
        return m_data[0];
    }

    grid_const_reference v() const {
        static_assert(dim >= 2, "out of range");
        return m_data[1];
    }

    grid_reference v() {
        static_assert(dim >= 2, "out of range");
        return m_data[1];
    }

    grid_const_reference w() const {
        static_assert(dim >= 3, "out of range");
        return m_data[2];
    }

    grid_reference w() {
        static_assert(dim >= 3, "out of range");
        return m_data[2];
    }

    grid_const_reference operator[](std::size_t i) const {
        assert(i >= 0 && i < dim);
        return m_data[i];
    }

    grid_reference operator[](std::size_t i) {
        assert(i >= 0 && i < dim);
        return m_data[i];
    }

    grid_const_reference at(std::size_t i) const {
        assert(i >= 0 && i < dim);
        return m_data.at(i);
    }

    grid_reference at(std::size_t i) {
        assert(i >= 0 && i < dim);
        return m_data.at(i);
    }

    template <typename A = StaggeredGrid, std::enable_if_t<A::dim == 2, std::nullptr_t> = nullptr>
    std::tuple<grid_const_reference, grid_const_reference> get_component() const {
        return {u(), v()};
    }

    template <typename A = StaggeredGrid, std::enable_if_t<A::dim == 2, std::nullptr_t> = nullptr>
    std::tuple<grid_reference, grid_reference> get_component() {
        return {u(), v()};
    }

    template <typename A = StaggeredGrid, std::enable_if_t<A::dim == 3, std::nullptr_t> = nullptr>
    std::tuple<grid_const_reference, grid_const_reference, grid_const_reference> get_component()
        const {
        return {u(), v(), w()};
    }

    template <typename A = StaggeredGrid, std::enable_if_t<A::dim == 3, std::nullptr_t> = nullptr>
    std::tuple<grid_reference, grid_reference, grid_reference> get_component() {
        return {u(), v(), w()};
    }

    template <typename F>
    void set(F f) {
        for (int i = 0; i < dim; ++i) {
            m_data[i].set(f);
        }
    }

    void fill(const value_type& value) {
        for (auto& e : m_data) {
            std::fill(e.begin(), e.end(), value);
        }
    }

    value_type max_value() const {
        if constexpr (dim == 1) {
            return std::max({*std::max_element(m_data[0].cbegin(), m_data[0].cend())});
        } else if constexpr (dim == 2) {
            return std::max({*std::max_element(m_data[0].cbegin(), m_data[0].cend()),
                             *std::max_element(m_data[1].cbegin(), m_data[1].cend())});
        } else {
            return std::max({*std::max_element(m_data[0].cbegin(), m_data[0].cend()),
                             *std::max_element(m_data[1].cbegin(), m_data[1].cend()),
                             *std::max_element(m_data[2].cbegin(), m_data[2].cend())});
        }
    }

    value_type min_value() const {
        if constexpr (dim == 1) {
            return std::min({*std::min_element(m_data[0].cbegin(), m_data[0].cend())});
        } else if constexpr (dim == 2) {
            return std::min({*std::min_element(m_data[0].cbegin(), m_data[0].cend()),
                             *std::min_element(m_data[1].cbegin(), m_data[1].cend())});
        } else {
            return std::min({*std::min_element(m_data[0].cbegin(), m_data[0].cend()),
                             *std::min_element(m_data[1].cbegin(), m_data[1].cend()),
                             *std::min_element(m_data[2].cbegin(), m_data[2].cend())});
        }
    }

    value_type max_absolute_value() const {
        auto compare
            = [](const value_type& a, const value_type& b) { return std::fabs(a) < std::fabs(b); };
        if constexpr (dim == 1) {
            return std::fabs(std::max(
                {*std::max_element(m_data[0].cbegin(), m_data[0].cend(), compare)}, compare));
        } else if constexpr (dim == 2) {
            return std::fabs(
                std::max({*std::max_element(m_data[0].cbegin(), m_data[0].cend(), compare),
                          *std::max_element(m_data[1].cbegin(), m_data[1].cend(), compare)},
                         compare));
        } else {
            return std::fabs(
                std::max({*std::max_element(m_data[0].cbegin(), m_data[0].cend(), compare),
                          *std::max_element(m_data[1].cbegin(), m_data[1].cend(), compare),
                          *std::max_element(m_data[2].cbegin(), m_data[2].cend(), compare)},
                         compare));
        }
    }

    index_type ni() const {
        static_assert(dim >= 1);
        return m_ni;
    }

    index_type nj() const {
        static_assert(dim >= 2);
        return m_nj;
    }

    index_type nk() const {
        static_assert(dim >= 3);
        return m_nk;
    }

    coordinate_type dx() const {
        return m_dx;
    }

    coordinate_type li() const {
        static_assert(dim >= 1);
        return m_dx * m_ni;
    }

    coordinate_type lj() const {
        static_assert(dim >= 2);
        return m_dx * m_nj;
    }

    coordinate_type lk() const {
        static_assert(dim >= 3);
        return m_dx * m_nk;
    }

    vector_type origin() const {
        return m_origin;
    }

   private:
    index_type m_ni, m_nj, m_nk;
    coordinate_type m_dx;
    vector_type m_origin;
    std::array<grid_type, dim> m_data;
};

template <typename T>
using StaggeredGrid1  = StaggeredGrid<T, float, float, int, 1>;
using StaggeredGrid1i = StaggeredGrid1<int>;
using StaggeredGrid1f = StaggeredGrid1<float>;
using StaggeredGrid1d = StaggeredGrid1<double>;
using StaggeredGrid1c = StaggeredGrid1<char>;

template <typename T>
using StaggeredGrid2  = StaggeredGrid<T, float, Eigen::Vector2f, int, 2>;
using StaggeredGrid2i = StaggeredGrid2<int>;
using StaggeredGrid2f = StaggeredGrid2<float>;
using StaggeredGrid2d = StaggeredGrid2<double>;
using StaggeredGrid2c = StaggeredGrid2<char>;

template <typename T>
using StaggeredGrid3  = StaggeredGrid<T, float, Eigen::Vector3f, int, 3>;
using StaggeredGrid3i = StaggeredGrid3<int>;
using StaggeredGrid3f = StaggeredGrid3<float>;
using StaggeredGrid3d = StaggeredGrid3<double>;
using StaggeredGrid3c = StaggeredGrid3<char>;

}  // namespace nama
