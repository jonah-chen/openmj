
#pragma once
#include "core/mahjong/constants.hpp"
#include <vector>

namespace mj {
namespace draw {
class Layer
{
public:
    Layer() = default;
    virtual ~Layer() {}

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_update(float ms) {}
    virtual void on_gui_render() {}
};

class LayerStack
{
public:
    using container = std::vector<Layer *>;
    LayerStack() = default;
    ~LayerStack();

    void push_layer(Layer *layer);
    void push_overlay(Layer *layer);
    void pop_layer(Layer *layer);
    void pop_overlay(Layer *layer);

    container::iterator begin() { return layers_.begin(); }
    container::iterator end() { return layers_.end(); }
    container::const_iterator begin() const { return layers_.begin(); }
    container::const_iterator end() const { return layers_.end(); }

    constexpr std::size_t layers() const { return end_; }
    CONSTEXPR12 std::size_t overlays() const { return layers_.size() - end_; }
    CONSTEXPR12 std::size_t size() const { return layers_.size(); }

private:
    container layers_;
    std::size_t end_{};
};

} // namespace draw
} // namespace mj
