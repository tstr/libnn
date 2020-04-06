﻿/*
	Activation functions
*/

#include <cmath>
#include <algorithm>

#include "nn/ops/activations.h"

using namespace nn;

/*************************************************************************************************************************************/

const buffer& activation::sigmoid::forward(const buffer& x)
{
	return activate(x, [](scalar x) {
		return 1.0f / (1.0f + std::exp(-x));
	});
}

const buffer& activation::sigmoid::backward(const buffer& x, const buffer& dy)
{
	// σ'(x) = σ(x) * (1 - σ(x))
	return derivative(x, dy, [](scalar x, scalar y, scalar dy, scalar dx) {
		return (y * (1.0f - y)) * dy;
	});
}

/*************************************************************************************************************************************/

const buffer& activation::tanh::forward(const buffer& x)
{
	return activate(x, [](scalar x) {
		scalar a = std::exp(x);
		scalar b = 1.0f / a;
		return (a - b) / (a + b);
	});
}

const buffer& activation::tanh::backward(const buffer& x, const buffer& dy)
{
	// tanh'(x) = 1 - tanh(x)^2
	return derivative(x, dy, [](scalar x, scalar y, scalar dy, scalar dx) {
		return (1 - (y * y)) * dy;
	});
}

/*************************************************************************************************************************************/

const buffer& activation::relu::forward(const buffer& x)
{
	return activate(x, [](scalar x) {
		return std::max(x, 0.0f);
	});
}

const buffer& activation::relu::backward(const buffer& x, const buffer& dy)
{
	return derivative(x, dy, [](scalar x, scalar y, scalar dy, scalar dx) {
		return (x > 0.0f) ? dy : 0;
	});
}

/*************************************************************************************************************************************/

const buffer& activation::leaky_relu::forward(const buffer& x)
{
	return activate(x, [=](scalar x) {
		return (x > 0) ? x : _leakiness * x;
	});
}

const buffer& activation::leaky_relu::backward(const buffer& x, const buffer& dy)
{
	return derivative(x, dy, [=](scalar x, scalar y, scalar dy, scalar dx) {
		return ((x > 0) ? 1.0f : _leakiness) * dy;
	});
}

/*************************************************************************************************************************************/

const buffer& activation::softmax::forward(const buffer& _x)
{
	auto x = _x.as_vector();

	scalar max = x[0];
	for (uint i = 0; i < x.size(); i++)
		if (x[i] > max)
			max = x[i];

	scalar sum = 0.0f;
	for (uint i = 0; i < x.size(); i++)
	{
		scalar a = std::exp(x[i] - max);
		sum += a;
		y[i] = a;
	}

	for (uint i = 0; i < x.size(); i++)
	{
		y[i] /= sum;
	}

	return y.data();
}

const buffer& activation::softmax::backward(const buffer& x, const buffer& _dy)
{
	auto dy = _dy.as_vector();
	for (uint j = 0; j < x.size(); j++)
	{
		float sum = 0.0f;
		for (uint i = 0; i < x.size(); i++)
		{
			float dz = (i == j)
				? y[i] * (1 - y[i])
				: -y[i] * y[j];

			sum += dy[i] * dz;
		}
		dx[j] = sum;
	}

	return dx.data();
}

/*************************************************************************************************************************************/
