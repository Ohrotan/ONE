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

#ifndef __ONERT_BACKEND_CONTROLFLOW_DYNAMICTENSOR_MANAGER_H__
#define __ONERT_BACKEND_CONTROLFLOW_DYNAMICTENSOR_MANAGER_H__

#include "UserTensorRegistry.h"

#include <backend/IDynamicTensorManager.h>
#include <backend/cpu_common/MemoryManager.h>
#include <backend/cpu_common/TensorRegistry.h>
#include <ir/OperandInfo.h>
#include <ir/Operation.h>
#include <ir/Index.h>

namespace onert
{
namespace backend
{
namespace controlflow
{

// TODO Find optimized algorithm to manage memory.

/**
 * @brief Class to manage dynamic tensor and its memory
 */
class DynamicTensorManager : public backend::IDynamicTensorManager
{
public:
  DynamicTensorManager(const std::shared_ptr<cpu_common::TensorRegistry> &reg,
                       const std::shared_ptr<UserTensorRegistry> &user_reg);

  virtual ~DynamicTensorManager() = default;

  void applyShape(const ir::OperandIndex &ind, const ir::Shape &new_shape) override;

  void buildTensor(const ir::OperandIndex &ind, const ir::OperandInfo &tensor_info,
                   ir::Layout backend_layout);

  void planDealloc(ir::OperationIndex op_ind, ir::OperandIndex operand_ind) override;
  void deallocInput(ir::OperationIndex op_ind) override;
  void deallocSubgraphOutput(ir::OperandIndex ind) override;

private:
  /**
   * @brief Memory manager for dynamic tensor.
   * @todo  DynamicMemoryManager is not optimized. Optimized one is needed
   */
  std::shared_ptr<cpu_common::DynamicMemoryManager> _dynamic_mem_mgr;
  const std::shared_ptr<cpu_common::TensorRegistry> _tensors;
  const std::shared_ptr<UserTensorRegistry> _user_tensors;

  // contains list of dynamic tensor index, which can be deallocated after running operation
  // note: this map could contain static tensor index too. Careful use is required.
  std::unordered_map<ir::OperationIndex, std::unordered_set<ir::OperandIndex>> _dealloc_tensor_map;
};

} // namespace controlflow
} // namespace backend
} // namespace onert

#endif // __ONERT_BACKEND_CONTROLFLOW_DYNAMICTENSOR_MANAGER_H__
