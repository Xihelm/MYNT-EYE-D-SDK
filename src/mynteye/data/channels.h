// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef MYNTEYE_DATA_CHANNELS_H_
#define MYNTEYE_DATA_CHANNELS_H_
#pragma once

#include <functional>
#include <map>
#include <memory>
#include <thread>
#include <vector>

#include "mynteye/data/types_internal.h"
#include "mynteye/types.h"

MYNTEYE_BEGIN_NAMESPACE

namespace hid {

class hid_device;

}  // namespace hid

class MYNTEYE_API Channels {
 public:
  typedef enum FileId {
    FID_DEVICE_INFO = 1,  // device info
    FID_RESERVE = 2,      // reserve
    FID_IMU_PARAMS = 4,   // imu intrinsics & extrinsics
    FID_LAST,
  } file_id_t;

  using device_info_t = DeviceParams;

  typedef struct ImuParams {
    bool ok;
    ImuIntrinsics in_accel;
    ImuIntrinsics in_gyro;
    Extrinsics ex_left_to_imu;
  } imu_params_t;

  using imu_packets_t = std::vector<ImuDataPacket>;
  using img_packets_t = std::vector<ImgInfoPacket>;

  using imu_callback_t = std::function<void(const ImuDataPacket &packet)>;
  using img_callback_t = std::function<void(const ImgInfoPacket &packet)>;

  Channels();
  virtual ~Channels();

  void SetImuCallback(imu_callback_t callback);
  void SetImgInfoCallback(img_callback_t callback);

  void Open();
  void Close();

  bool StartHidTracking();
  bool StopHidTracking();

  void DoHidTrack();

  bool GetFiles(device_info_t *info,
      imu_params_t *imu_params,
      Version *spec_version = nullptr);

  bool SetFiles(device_info_t *info,
      imu_params_t *imu_params,
      Version *spec_version);

  bool IsHidExist();

 protected:
  bool ExtractHidData(imu_packets_t &imu, img_packets_t &img);  // NOLINT
  bool RequireFileData(bool device_info,
      bool reserve,
      bool imu_params,
      std::uint8_t *data,
      std::uint16_t &file_size);  // NOLINT
  bool UpdateFileData(std::uint8_t *data, std::uint16_t size);

 private:
  std::shared_ptr<hid::hid_device> device_;

  bool is_hid_tracking_ = false;
  volatile bool hid_track_stop_ = false;
  std::thread hid_track_thread_;

  imu_callback_t imu_callback_;
  img_callback_t img_callback_;

  std::uint8_t req_count_ = 0;
  std::uint16_t package_sn_ = 0;

  bool is_hid_open_ = false;
};

MYNTEYE_END_NAMESPACE

#endif  // MYNTEYE_DATA_CHANNELS_H_
