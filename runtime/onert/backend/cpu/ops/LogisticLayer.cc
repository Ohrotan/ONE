/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "LogisticLayer.h"

#include "OperationUtils.h"

#include <cker/operation/Logistic.h>

namespace onert
{
namespace backend
{
namespace cpu
{
namespace ops
{

LogisticLayer::LogisticLayer() : _input(nullptr), _output(nullptr)
{
  // DO NOTHING
}

void LogisticLayer::populateLookupTable()
{
  const auto input_scale = static_cast<double>(_input->data_scale());
  const auto input_zero_point = static_cast<int32_t>(_input->data_offset());
  const auto output_scale = static_cast<double>(_output->data_scale());
  const auto output_zero_point = static_cast<int32_t>(_output->data_offset());
  const float inverse_scale = 1 / output_scale;
  int32_t maxval = std::numeric_limits<uint8_t>::max();
  int32_t minval = std::numeric_limits<uint8_t>::min();
  for (int32_t val = minval; val <= maxval; ++val)
  {
    const float dequantized = input_scale * (val - input_zero_point);
    const float transformed = 1.0f / (1.0f + std::exp(-dequantized));
    const float rescaled = std::round(transformed * inverse_scale);
    const int32_t quantized = static_cast<int32_t>(rescaled + output_zero_point);
    _table[val] = static_cast<uint8_t>(std::max(std::min(maxval, quantized), minval));
  }
}

void LogisticLayer::logisticFloat32()
{
  nnfw::cker::Logistic(getTensorShape(_input), reinterpret_cast<const float *>(_input->buffer()),
                       getTensorShape(_output), reinterpret_cast<float *>(_output->buffer()));
}

void LogisticLayer::logisticQuant8()
{
  const int size = MatchingFlatSize(getTensorShape(_input), getTensorShape(_output));
  const uint8_t *input_data = reinterpret_cast<const uint8_t *>(_input->buffer());
  uint8_t *output_data = reinterpret_cast<uint8_t *>(_output->buffer());

  for (int i = 0; i < size; ++i)
  {
    output_data[i] = _table[input_data[i]];
  }
}

void LogisticLayer::configure(const IPortableTensor *input, IPortableTensor *output)
{
  _input = input;
  _output = output;

  if (_input->data_type() == OperandType::QUANT_UINT8_ASYMM)
  {
    if (_output->data_scale() != 1.f / 256)
    {
      throw std::runtime_error{"incorrect scale for output"};
    }
    populateLookupTable();
  }
}

void LogisticLayer::run()
{
  if (_input->data_type() == OperandType::FLOAT32)
  {
    logisticFloat32();
  }
  else if (_input->data_type() == OperandType::QUANT_UINT8_ASYMM)
  {
    logisticQuant8();
  }
  else
  {
    throw std::runtime_error{"Logistic: unsupported data type"};
  }
}

} // namespace ops
} // namespace cpu
} // namespace backend
} // namespace onert
