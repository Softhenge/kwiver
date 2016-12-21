/*ckwg +29
 * Copyright 2016 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief Header defining matlab image object detector
 */

#ifndef VITAL_BINDINGS_MATLAB_IMAGE_OBJECT_DETECTOR_H
#define VITAL_BINDINGS_MATLAB_IMAGE_OBJECT_DETECTOR_H

#include <vital/algo/image_object_detector.h>
#include <arrows/matlab/kwiver_algo_matlab_export.h>

namespace kwiver {
namespace arrows {
namespace matlab {

class KWIVER_ALGO_MATLAB_EXPORT matlab_image_object_detector
  : public vital::algorithm_impl< matlab_image_object_detector, vital::algo::image_object_detector >
{
public:
  matlab_image_object_detector();
  virtual ~matlab_image_object_detector();

  virtual std::string impl_name() const { return "matlab"; }

  virtual vital::config_block_sptr get_configuration() const;
  virtual void set_configuration(vital::config_block_sptr config);
  virtual bool check_configuration(vital::config_block_sptr config) const;

  // Main detection method
  virtual vital::detected_object_set_sptr detect( vital::image_container_sptr image_data) const;

private:
  class priv;
  const std::unique_ptr<priv> d;
};

} } } // end namespace


#endif // VITAL_BINDINGS_MATLAB_IMAGE_OBJECT_DETECTOR_H
