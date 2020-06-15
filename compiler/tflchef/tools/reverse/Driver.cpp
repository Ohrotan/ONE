/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <tflchef/RawModel.h>
#include <tflchef/RecipeChef.h>

#include <foder/FileLoader.h>

#include <memory>
#include <iostream>

int entry(int argc, char **argv)
{
  if (argc != 3)
  {
    std::cerr << "ERROR: Failed to parse arguments" << std::endl;
    std::cerr << std::endl;
    std::cerr << "USAGE: " << argv[0] << " [tflite] [output]" << std::endl;
    return 255;
  }

  // Load TF lite model from a tflite file
  const foder::FileLoader fileLoader{argv[1]};
  std::vector<char> modelData = fileLoader.load();
  const tflite::Model *tflmodel = tflite::GetModel(modelData.data());
  if (tflmodel == nullptr)
  {
    std::cerr << "ERROR: Failed to load tflite '" << argv[1] << "'" << std::endl;
    return 255;
  }

  // Generate ModelRecipe recipe
  std::unique_ptr<tflchef::ModelRecipe> recipe = tflchef::generate_recipe(tflmodel);
  if (recipe.get() == nullptr)
  {
    std::cerr << "ERROR: Failed to generate recipe" << std::endl;
    return 255;
  }

  // Save to a file
  bool result = tflchef::write_recipe(argv[2], recipe);
  if (!result)
  {
    std::cerr << "ERROR: Failed to write to recipe '" << argv[2] << "'" << std::endl;
    return 255;
  }
  return 0;
}
