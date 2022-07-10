
#include "layerstack.hpp"
#include <algorithm>

namespace mj {
namespace draw {
LayerStack::~LayerStack()
{
    for (auto &layer : layers_)
    {
        layer->on_detach();
        delete layer;
    }
}

void LayerStack::push_layer(Layer *layer)
{
    layers_.emplace(layers_.begin() + end_++, layer);
}

void LayerStack::push_overlay(Layer *layer) { layers_.emplace_back(layer); }

void LayerStack::pop_layer(Layer *layer)
{
    auto it = std::find(layers_.begin(), layers_.end(), layer);
    if (it != layers_.end())
    {
        layers_.erase(it);
        --end_;
    }
}

void LayerStack::pop_overlay(Layer *layer)
{
    auto it = std::find(layers_.begin() + end_, layers_.end(), layer);
    if (it != layers_.end())
    {
        layers_.erase(it);
    }
}

} // namespace draw
} // namespace mj