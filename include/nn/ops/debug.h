/*
    Debug layer
*/

#pragma once

#include "node.h"

namespace nn
{
	enum class debug_flag
	{
		print_all       = -1,
		print_forward   = 1,
		print_backward = 2
	};

	inline debug_flag operator|(debug_flag a, debug_flag b) { return (debug_flag)((int)a | (int)b); }
	inline bool operator&(debug_flag a, debug_flag b) { return ((int)a & (int)b) != 0; }

	class debug_layer final : public uniform_node
	{
		debug_flag _flags;

	public:

		debug_layer(tensor_shape shape, debug_flag flags = debug_flag::print_all);

		vector forward(scope& dc, const vector& x) override;
		vector backward(scope& dc, const vector& x, const vector& dy) override;
	};
}