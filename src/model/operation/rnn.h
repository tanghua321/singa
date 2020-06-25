/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SRC_MODEL_OPERATION_RNN_H_
#define SRC_MODEL_OPERATION_RNN_H_

#include <iostream>
#include <tuple>
#include <vector>

#include "singa/core/tensor.h"
#include "singa/singa_config.h"
#include "singa/utils/logging.h"

#ifdef USE_CUDNN
#include <cudnn.h>

#include "../layer/cudnn_utils.h"
#endif  // USE_CUDNN

namespace singa {

#ifdef USE_CUDNN
class CudnnRNNHandle {
 public:
  CudnnRNNHandle(const Tensor &x, const int hidden_size, const int mode = 0,
                 const int num_layers = 1, const int bias = 1,
                 const float dropout = 0.0f, const int bidirectional = 0);

  Context *ctx;
  std::shared_ptr<Device> dev;

  // parameters
  int bias;
  int mode;
  float dropout;
  int bidirectional;
  // dont change
  size_t feature_size;
  size_t hidden_size;
  size_t weights_size;
  int num_layers;
  // vary
  size_t batch_size;  // remove
  size_t seq_length;

  /* workspace data */
  size_t workspace_size;
  size_t reserve_size;
  Tensor workspace;
  Tensor reserve_space;

  /* dropout */
  void *states;
  cudnnDropoutDescriptor_t dropoutDesc;

  /* rnn desc */
  cudnnRNNDescriptor_t rnnDesc;
  cudnnRNNMode_t RNNMode;
  cudnnRNNAlgo_t cudnnRNNAlgo;
  cudnnDataType_t cudnnDataType;

  /* weights desc */
  cudnnFilterDescriptor_t wDesc, dwDesc;

  void init_dropout_desc();
  void init_rnn_desc();
  void init_parameters_desc(cudnnTensorDescriptor_t *xDesc);
  void init_workspace(cudnnTensorDescriptor_t *xDesc);
  Tensor merge_inputs(size_t num, const vector<Tensor> &in);
  vector<Tensor> split_output(size_t num, size_t dim, const vector<Tensor> &in,
                              const Tensor output);
  Tensor get_weight(size_t pseudo_layer, const Tensor &w, size_t lin_layer_id);
};

void init_xDesc(cudnnTensorDescriptor_t *xDesc, CudnnRNNHandle &h);
void init_yDesc(cudnnTensorDescriptor_t *yDesc, CudnnRNNHandle &h);
void init_hc_Desc(cudnnTensorDescriptor_t &hDesc, CudnnRNNHandle &h);

vector<Tensor> GpuRNNForwardTraining(const Tensor &x, const Tensor &hx,
                                     const Tensor &cx, const Tensor &W,
                                     CudnnRNNHandle &h);
vector<Tensor> GpuRNNForwardInference(const Tensor &x, const Tensor &hx,
                                      const Tensor &cx, const Tensor &W,
                                      CudnnRNNHandle &h);
vector<Tensor> GpuRNNBackwardx(const Tensor &y, const Tensor &dy,
                               const Tensor &dhy, const Tensor &dcy,
                               const Tensor &W, const Tensor &hx,
                               const Tensor &cx, CudnnRNNHandle &h);
Tensor GpuRNNBackwardW(const Tensor &x, const Tensor &hx, const Tensor &y,
                       CudnnRNNHandle &h);

#endif  // USE_CUDNN

}  // namespace singa
#endif  // SRC_MODEL_OPERATION_RNN_H_