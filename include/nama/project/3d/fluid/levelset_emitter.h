#pragma once

#include <nama/common/grid.h>

#include <Eigen/Core>

namespace nama::_3d {

namespace function {

std::function<float(const Eigen::Vector3f&)> sphere(const Eigen::Vector3f& center, float radius);

std::function<float(const Eigen::Vector3f&)> pool(float height);

std::function<float(int, int, int)> room(int ni, int nj, int nk);

}  // namespace function

namespace fluid {

class LevelSetEmitterBase {
   public:
    LevelSetEmitterBase()                                      = default;
    LevelSetEmitterBase(const LevelSetEmitterBase&)            = default;
    LevelSetEmitterBase& operator=(const LevelSetEmitterBase&) = default;
    LevelSetEmitterBase(LevelSetEmitterBase&&)                 = default;
    LevelSetEmitterBase& operator=(LevelSetEmitterBase&&)      = default;
    virtual ~LevelSetEmitterBase()                             = default;

    virtual void apply(Grid3f& grid) const = 0;
};

class LevelSetEmitterFunction : public LevelSetEmitterBase {
   public:
    LevelSetEmitterFunction()                                          = default;
    LevelSetEmitterFunction(const LevelSetEmitterFunction&)            = default;
    LevelSetEmitterFunction& operator=(const LevelSetEmitterFunction&) = default;
    LevelSetEmitterFunction(LevelSetEmitterFunction&&)                 = default;
    LevelSetEmitterFunction& operator=(LevelSetEmitterFunction&&)      = default;
    virtual ~LevelSetEmitterFunction()                                 = default;

    explicit LevelSetEmitterFunction(std::function<float(const Eigen::Vector3f&)>&& function);

    void set_function(std::function<float(const Eigen::Vector3f&)>&& function);

    virtual void apply(Grid3f& grid) const override;

   private:
    std::function<float(const Eigen::Vector3f&)> m_function;
};

class LevelSetEmitterGrid : public LevelSetEmitterBase {
   public:
    LevelSetEmitterGrid()                                      = default;
    LevelSetEmitterGrid(const LevelSetEmitterGrid&)            = default;
    LevelSetEmitterGrid& operator=(const LevelSetEmitterGrid&) = default;
    LevelSetEmitterGrid(LevelSetEmitterGrid&&)                 = default;
    LevelSetEmitterGrid& operator=(LevelSetEmitterGrid&&)      = default;
    virtual ~LevelSetEmitterGrid()                             = default;

    explicit LevelSetEmitterGrid(std::function<float(int, int, int)>&& function);

    void set_function(std::function<float(int, int, int)>&& function);

    virtual void apply(Grid3f& grid) const override;

   private:
    std::function<float(int, int, int)> m_function;
};

class LevelSetEmitterList : public LevelSetEmitterBase {
   public:
    LevelSetEmitterList()                                      = default;
    LevelSetEmitterList(const LevelSetEmitterList&)            = default;
    LevelSetEmitterList& operator=(const LevelSetEmitterList&) = default;
    LevelSetEmitterList(LevelSetEmitterList&&)                 = default;
    LevelSetEmitterList& operator=(LevelSetEmitterList&&)      = default;
    virtual ~LevelSetEmitterList()                             = default;

    explicit LevelSetEmitterList(std::vector<std::shared_ptr<LevelSetEmitterBase>>& emitters);

    void add_emitter(std::shared_ptr<LevelSetEmitterBase>&& emitter);

    virtual void apply(Grid3f& grid) const override;

    const auto& get_emitters() const;

   private:
    std::vector<std::shared_ptr<LevelSetEmitterBase>> m_emitters;
};

}  // namespace fluid

}  // namespace nama::_3d
