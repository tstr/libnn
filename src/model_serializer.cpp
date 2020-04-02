/*
	Model serialization
*/

#include <fstream>

#include "nn/model.h"
#include "nn/ops/dense.h"

using namespace nn;

/*************************************************************************************************************************************/

static const char s_magic[4] = { 'N', 'N', 'M', 'D' };

template<typename type>
void write(std::ostream& out, const type& value)
{
	out.write(reinterpret_cast<const char*>(&value), sizeof(type));
}

template<typename type>
type read(std::istream& in)
{
	type v;
	in.read(reinterpret_cast<char*>(&v), sizeof(type));
	return v;
}

/*************************************************************************************************************************************/


bool model::serialize(const std::string& filename)
{
	std::fstream f(filename, std::ios::binary | std::ios::out);
	if (f.fail())
	{
		std::cout << "could not write: " << filename << std::endl;
		return false;
	}

	std::vector<parameterised_node*> layers;
	for (auto& node : _nodes)
	{
		auto d = dynamic_cast<parameterised_node*>(node.get());
		if (d != nullptr) layers.push_back(d);
	}

	write(f, s_magic);
	write<uint>(f, (uint)layers.size());

	for (auto layer : layers)
	{
		const buffer& w = layer->get_w().p;
		const buffer& b = layer->get_b().p;

		write<uint>(f, w.size());
		write<uint>(f, b.size());
		write<uint>(f, (uint)layer->input_shape().size());
		for (uint i : layer->input_shape())
			write<uint>(f, i);

		write<uint>(f, (uint)layer->output_shape().size());
		for (uint i : layer->output_shape())
			write<uint>(f, i);
		

		f.write((const char*)w.ptr(), (std::streamoff)sizeof(scalar) * w.size());
		f.write((const char*)b.ptr(), (std::streamoff)sizeof(scalar) * b.size());
	}

	return true;
}

bool model::deserialize(const std::string& filename)
{
	std::fstream f(filename, std::ios::binary | std::ios::in);
	if (f.fail())
	{
		std::cout << "could not read file: " << filename << std::endl;
		return false;
	}

	std::vector<parameterised_node*> layers;
	for (auto& node : _nodes)
	{
		auto d = dynamic_cast<parameterised_node*>(node.get());
		if (d != nullptr) layers.push_back(d);
	}

	uint magic = read<uint>(f);
	if (magic != *reinterpret_cast<const uint*>(s_magic))
	{
		std::cout << "incorrect file format: " << filename << std::endl;
		return false;
	}

	uint layer_count = read<uint>(f);
	if (layer_count != layers.size())
	{
		std::cout << "incorrect layer count";
		return false;
	}

	for (auto layer : layers)
	{
		const auto& in_shape = layer->input_shape();
		const auto& out_shape = layer->output_shape();

		uint w_size = read<uint>(f);
		uint b_size = read<uint>(f);

		const buffer& w = layer->get_w().p;
		const buffer& b = layer->get_b().p;

		if (w_size != w.size())
		{
			std::cout << "weight count does not match: " << w_size << " != " << w.size() << std::endl;
			return false;
		}
		if (b_size != b.size())
		{
			std::cout << "bias count does not match: " << b_size << " != " << b.size() << std::endl;
			return false;
		}

		uint in_shape_size = read<uint>(f);
		if (in_shape_size != in_shape.size())
		{
			std::cout << "shape does not match" << std::endl;
			return false;
		}
		for (uint i = 0; i < in_shape_size; i++)
		{
			if (read<uint>(f) != in_shape[i])
			{
				std::cout << "shape dimension does not match" << std::endl;
				return false;
			}
		}

		uint out_shape_size = read<uint>(f);
		if (out_shape_size != out_shape.size())
		{
			std::cout << "shape does not match" << std::endl;
			return false;
		}
		for (uint i = 0; i < out_shape_size; i++)
		{
			if (read<uint>(f) != out_shape[i])
			{
				std::cout << "shape dimension does not match" << std::endl;
				return false;
			}
		}

		f.read((char*)w.ptr(), (std::streamoff)sizeof(scalar) * w.size());
		f.read((char*)b.ptr(), (std::streamoff)sizeof(scalar) * b.size());
	}

	return true;
}

/*************************************************************************************************************************************/