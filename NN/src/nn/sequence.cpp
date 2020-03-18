
#include "sequence.h"

using namespace nn;

const buffer& sequence::forward(const buffer& x)
{
	auto a = std::cref(x);
	_activations.clear();
	_activations.push_back(a);

	for (auto& node : _nodes)
	{
		node->set_state(is_training());
		a = node->forward(a);
		_activations.push_back(a);
	}

	return _activations.back();
}

const buffer& sequence::backward(const buffer& x, const buffer& dy)
{
	auto d = std::ref(dy);

	for (int i = _nodes.size() - 1; i >= 0; i--)
	{
		auto node = _nodes[i].get();
		node->set_state(is_training());
		d = node->backward(_activations[i], d);
	}

	return d;
}
