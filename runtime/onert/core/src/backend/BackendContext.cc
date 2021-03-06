/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include "backend/BackendContext.h"

#include "ir/Operation.h"
#include "backend/IConstantInitializer.h"

namespace onert
{
namespace backend
{

void BackendContext::initialize(const std::vector<OperationInfo> &operation_list,
                                const std::vector<ir::OperandIndex> &operand_list)
{
  _operation_list = operation_list;
  _operand_list = operand_list;
}

void BackendContext::initConsts()
{
  for (auto &op : _operation_list)
  {
    constant_initializer->setLayout(op.layout);
    _graph->operations().at(op.index).accept(*constant_initializer);
  }

  for (auto ind : _operand_list)
  {
    const auto &obj = _graph->operands().at(ind);
    if (obj.isConstant() && !constant_initializer->exist(ind))
    {
      constant_initializer->registerDefaultInitializer(ind, obj);
    }
  }

  constant_initializer->run();
}

} // namespace backend
} // namespace onert
