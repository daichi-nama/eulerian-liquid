#include <nama/common/grid.h>
#include <nama/project/3d/fluid/levelset_emitter.h>

#include <Eigen/Core>

namespace nama::_3d {

namespace function {

std::function<float(const Eigen::Vector3f&)> sphere(const Eigen::Vector3f& center, float radius) {
    return [=](const Eigen::Vector3f& x) -> float { return ((x - center).norm() - radius); };
}

std::function<float(const Eigen::Vector3f&)> pool(float height) {
    return [=](const Eigen::Vector3f& x) -> float { return x[1] - height; };
}

std::function<float(int, int, int)> room(int ni, int nj, int nk) {
    return [=](int i, int j, int k) -> float {
        if (i == 0 || i == ni - 1     //
            || j == 0 || j == nj - 1  //
            || k == 0 || k == nk - 1) {
            return -1.0;
        } else if (i == 1 || i == ni - 2     //
                   || j == 1 || j == nj - 2  //
                   || k == 1 || k == nk - 2) {
            return -0.01;
        } else {
            return 1.0;
        }
    };
}

}  // namespace function

namespace fluid {

LevelSetEmitterFunction::LevelSetEmitterFunction(
    std::function<float(const Eigen::Vector3f&)>&& function)
    : m_function(std::move(function)) {}

void LevelSetEmitterFunction::set_function(
    std::function<float(const Eigen::Vector3f&)>&& function) {
    m_function = std::move(function);
}

void LevelSetEmitterFunction::apply(Grid3f& grid) const {
    parallel_loop(grid, [&](int i, int j, int k) {
        grid(i, j, k) = std::min(grid(i, j, k), m_function(grid.pos(i, j, k)));
    });
}

LevelSetEmitterGrid::LevelSetEmitterGrid(std::function<float(int, int, int)>&& function)
    : m_function(std::move(function)) {}

void LevelSetEmitterGrid::set_function(std::function<float(int, int, int)>&& function) {
    m_function = std::move(function);
}

void LevelSetEmitterGrid::apply(Grid3f& grid) const {
    parallel_loop(grid, [&](int i, int j, int k) {
        grid(i, j, k) = std::min(grid(i, j, k), m_function(i, j, k));
    });
}

LevelSetEmitterList::LevelSetEmitterList(
    std::vector<std::shared_ptr<LevelSetEmitterBase>>& emitters)
    : m_emitters(emitters) {}

void LevelSetEmitterList::add_emitter(std::shared_ptr<LevelSetEmitterBase>&& emitter) {
    m_emitters.push_back(std::move(emitter));
}

void LevelSetEmitterList::apply(Grid3f& grid) const {
    std::fill(grid.begin(), grid.end(), std::numeric_limits<float>::max());
    for (const auto& e : m_emitters) {
        e->apply(grid);
    }
}

const auto& LevelSetEmitterList::get_emitters() const {
    return m_emitters;
}

}  // namespace fluid

}  // namespace nama::_3d
