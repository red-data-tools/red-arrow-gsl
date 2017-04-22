/*
 * Copyright 2017 Kouhei Sutou <kou@clear-code.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <arrow-glib/arrow-glib.h>

#include <rbgobject.h>

#include <rb_gsl.h>

void Init_arrow_gsl(void);

static VALUE
rb_arrow_int32_array_to_gsl(VALUE self)
{
  /*
  GArrowInt32Array *arrow_array;
  gint64 length;
  GArrowBuffer *buffer;
  GBytes *data;
  gconstpointer data_raw;
  gsize data_size;
  gsl_vector_int *gsl_vector;

  arrow_array = RVAL2GOBJ(self);
  length = garrow_array_get_length(GARROW_ARRAY(arrow_array));
  gsl_vector = gsl_vector_int_alloc(length);

  buffer = garrow_primary_array_get_buffer(GARROW_PRIMARY_ARRAY(arrow_array));
  data = garrow_buffer_get_data(buffer);
  data_raw = g_bytes_get_data(data, &data_size);
  memcpy(gsl_vector->data, data_raw, data_size);
  g_object_unref(buffer);

  return Data_Wrap_Struct(cgsl_vector_int, 0, gsl_vector_int_free, gsl_vector);
  */
  return Qnil;
}

static VALUE
rb_arrow_tensor_to_gsl(VALUE self)
{
  GArrowTensor *tensor;
  gint64 *shape;
  gint n_dimensions;
  GArrowType value_type;
  VALUE rb_matrix = Qnil;

  tensor = RVAL2GOBJ(self);
  shape = garrow_tensor_get_shape(tensor, &n_dimensions);
  if (n_dimensions != 2) {
    g_free(shape);
    rb_raise(rb_eArgError,
             "the number of dimensions must be 2: <%d>", n_dimensions);
  }

  value_type = garrow_tensor_get_value_type(tensor);
  if (value_type == GARROW_TYPE_INT32) {
    gsl_matrix_int *matrix;
    GArrowBuffer *buffer;
    GBytes *data;
    gconstpointer data_raw;
    gsize data_size;

    matrix = gsl_matrix_int_alloc(shape[0], shape[1]);
    buffer = garrow_tensor_get_buffer(tensor);
    data = garrow_buffer_get_data(buffer);
    data_raw = g_bytes_get_data(data, &data_size);
    memcpy(matrix->data, data_raw, data_size);
    g_bytes_unref(data);
    g_object_unref(buffer);

    rb_matrix = Data_Wrap_Struct(cgsl_matrix_int,
                                 NULL,
                                 gsl_matrix_int_free,
                                 matrix);
  } else if (value_type == GARROW_TYPE_DOUBLE) {
    gsl_matrix *matrix;
    GArrowBuffer *buffer;
    GBytes *data;
    gconstpointer data_raw;
    gsize data_size;

    matrix = gsl_matrix_alloc(shape[0], shape[1]);
    buffer = garrow_tensor_get_buffer(tensor);
    data = garrow_buffer_get_data(buffer);
    data_raw = g_bytes_get_data(data, &data_size);
    memcpy(matrix->data, data_raw, data_size);
    g_bytes_unref(data);
    g_object_unref(buffer);

    rb_matrix = Data_Wrap_Struct(cgsl_matrix,
                                 NULL,
                                 gsl_matrix_free,
                                 matrix);
  } else {
    GArrowDataType *value_data_type;
    VALUE rb_value_data_type;

    g_free(shape);
    value_data_type = garrow_tensor_get_value_data_type(tensor);
    rb_value_data_type = GOBJ2RVAL(value_data_type);
    g_object_unref(value_data_type);
    rb_raise(rb_eArgError,
             "value data type must be int32 or double: <%" PRIsVALUE ">",
             rb_value_data_type);
  }

  g_free(shape);

  return rb_matrix;
}

static VALUE
rb_gsl_matrix_int_to_arrow(VALUE self)
{
  gsl_matrix_int *matrix = NULL;
  GArrowInt32DataType *data_type;
  GArrowBuffer *data;
  gint64 shape[2];
  GArrowTensor *tensor;
  VALUE rb_tensor;

  Data_Get_Struct(self, gsl_matrix_int, matrix);

  data_type = garrow_int32_data_type_new();
  data = garrow_buffer_new((const guint8 *)(matrix->data),
                           sizeof(gint32) * matrix->size1 * matrix->size2);
  shape[0] = matrix->size1;
  shape[1] = matrix->size2;
  tensor = garrow_tensor_new(GARROW_DATA_TYPE(data_type),
                             data,
                             shape,
                             2,
                             NULL,
                             0,
                             NULL,
                             0);
  g_object_unref(data);
  g_object_unref(data_type);

  rb_tensor = GOBJ2RVAL(tensor);
  g_object_unref(tensor);

  return rb_tensor;
}

static VALUE
rb_gsl_matrix_to_arrow(VALUE self)
{
  gsl_matrix *matrix = NULL;
  GArrowDoubleDataType *data_type;
  GArrowBuffer *data;
  gint64 shape[2];
  GArrowTensor *tensor;
  VALUE rb_tensor;

  Data_Get_Struct(self, gsl_matrix, matrix);

  data_type = garrow_double_data_type_new();
  data = garrow_buffer_new((const guint8 *)(matrix->data),
                           sizeof(gdouble) * matrix->size1 * matrix->size2);
  shape[0] = matrix->size1;
  shape[1] = matrix->size2;
  tensor = garrow_tensor_new(GARROW_DATA_TYPE(data_type),
                             data,
                             shape,
                             2,
                             NULL,
                             0,
                             NULL,
                             0);
  g_object_unref(data);
  g_object_unref(data_type);

  rb_tensor = GOBJ2RVAL(tensor);
  g_object_unref(tensor);

  return rb_tensor;
}

void
Init_arrow_gsl(void)
{
  VALUE rb_Arrow;
  VALUE rb_ArrowInt32Array;
  VALUE rb_ArrowTensor;

  rb_Arrow = rb_const_get(rb_cObject, rb_intern("Arrow"));
  rb_ArrowInt32Array = rb_const_get(rb_Arrow, rb_intern("Int32Array"));
  rb_ArrowTensor = rb_const_get(rb_Arrow, rb_intern("Tensor"));

  rb_define_method(rb_ArrowInt32Array, "to_gsl",
                   rb_arrow_int32_array_to_gsl, 0);
  rb_define_method(rb_ArrowTensor, "to_gsl",
                   rb_arrow_tensor_to_gsl, 0);

  /*
  rb_define_method(cgsl_vector_int, "to_arrow",
                   rb_gsl_vector_int_to_arrow, 0);
  rb_define_method(cgsl_vector, "to_arrow",
                   rb_gsl_vector_to_arrow, 0);
  */

  rb_define_method(cgsl_matrix_int, "to_arrow",
                   rb_gsl_matrix_int_to_arrow, 0);
  rb_define_method(cgsl_matrix, "to_arrow",
                   rb_gsl_matrix_to_arrow, 0);
}
