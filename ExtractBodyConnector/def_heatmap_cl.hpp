#ifndef DEF_HEATMAP_CL_HPP
#define DEF_HEATMAP_CL_HPP
#include <vector>
#include <fstream>
#include <iterator>
#include <array>

struct SimpleData{
	size_t rows;
	size_t cols;
	size_t channels;
	std::vector<float> data;
};

extern SimpleData HEATMAP_CF;
extern SimpleData HEATMAP_CL;
extern SimpleData PAF_CF;
extern SimpleData PAF_CL;

template<typename T>
struct InpData {
	size_t rows;
	size_t cols;
	size_t channels;
	std::vector<T> data;
};

template<typename T>
std::istream& operator >> (std::istream &in, InpData<T> &inp_data) {
	in >> inp_data.rows >> inp_data.cols >> inp_data.channels;
	std::copy(std::istream_iterator<T>(in), std::istream_iterator<T>(),
		      std::back_inserter(inp_data.data));
	return in;
}

// channel first
template<typename T>
struct InpDataCF : InpData<T>{

};

// channel last
template<typename T>
struct InpDataCL : InpData<T>{
};

template<typename T, size_t d1, size_t d2, size_t d3>
struct Tensor{
	enum elems { all_elems = d1*d2*d3, row_elems = d2*d3, col_elems = d3 };

	std::array<T, all_elems> data;

	T& get(size_t i, size_t j, size_t k) {
		return data[i*row_elems + j*col_elems + k];
	}
};

#endif // DEF_HEATMAP_CL_HPP