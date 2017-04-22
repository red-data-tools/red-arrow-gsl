# README

## Name

Red Arrow GSL

## Description

Red Arrow GSL is a library that provides converters between Apache Arrow's array data (`Arrow::*Array`) / tensor data (`Arrow::Tensor`) and Ruby/GSL's vector data (`GSL::Vector` and `GSL::Vector::Int`) / matrix data (`GSL::Matrix::*`).

Red Arrow GSL adds `Arrow::*Array#to_gsl`/`Arrow::Tensor#to_gsl` for Apache Arrow to GSL conversion. Red Arrow GSL adds `GSL::Vector#to_arrow`/`GSL::Vector::Int#to_arrow`/`GSL::Matrix::*#to_arrow` for GSL to Apache Arrow conversion.

## Install

```text
% gem install red-arrow-gsl
```

## Usage

```ruby
require "arrow-gsl"

double_array.to_gsl # -> An object of GSL::Vector
int32_array.to_gsl  # -> An object of GSL::Vector::Int
tensor.to_tsl       # -> An object of GSL::Matrix or GSL::Matrix::Int

gsl_vector.to_arrow     # -> An object of Arrow::DoubleArray
gsl_int_vector.to_arrow # -> An object of Arrow::Int32Array
gsl_matrix.to_arrow     # -> An object of Arrow::Tensor
```

## Dependencies

* [Red Arrow](https://github.com/red-data-tools/red-arrow)

* [Ruby/GSL](https://github.com/SciRuby/rb-gsl)

## Authors

* Kouhei Sutou \<kou@clear-code.com\>

## License

Apache License 2.0. See doc/text/apache-2.0.txt for details.

(Kouhei Sutou has a right to change the license including contributed
patches.)
