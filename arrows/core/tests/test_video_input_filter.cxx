/*ckwg +29
 * Copyright 2017 by Kitware, Inc.
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
 * \brief test reading images and metadata with video_input_split
 */

#include <test_gtest.h>

#include <arrows/core/video_input_filter.h>
#include <vital/algo/algorithm_factory.h>
#include <vital/plugin_loader/plugin_manager.h>

#include <memory>
#include <string>
#include <iostream>

#include "barcode_decode.h"
#include "seek_frame_common.h"

kwiver::vital::path_t g_data_dir;

namespace algo = kwiver::vital::algo;
namespace kac = kwiver::arrows::core;
static int num_expected_frames = 50;
static int num_expected_frames_subset = 20;
static std::string list_file_name = "frame_list.txt";

// ----------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  TEST_LOAD_PLUGINS();

  GET_ARG(1, g_data_dir);

  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
class video_input_filter : public ::testing::Test
{
  TEST_ARG(data_dir);
};

// ----------------------------------------------------------------------------
TEST_F(video_input_filter, create)
{
  EXPECT_NE( nullptr, algo::video_input::create("filter") );
}

// ----------------------------------------------------------------------------
static
bool
set_config(kwiver::vital::config_block_sptr config, std::string const& data_dir)
{
  config->set_value( "video_input:type", "split" );
  config->set_value( "video_input:split:image_source:type", "image_list" );
  if ( kwiver::vital::has_algorithm_impl_name( "image_io", "ocv" ) )
  {
    config->set_value( "video_input:split:image_source:image_list:image_reader:type", "ocv" );
  }
  else if ( kwiver::vital::has_algorithm_impl_name( "image_io", "vxl" ) )
  {
    config->set_value( "video_input:split:image_source:image_list:image_reader:type", "vxl" );
  }
  else
  {
    std::cout << "Skipping tests since there is no image reader." << std::endl;
    return false;
  }

  config->set_value( "video_input:split:metadata_source:type", "pos" );
  config->set_value( "video_input:split:metadata_source:pos:metadata_directory", data_dir + "/pos");

  return true;
}

// ----------------------------------------------------------------------------
TEST_F(video_input_filter, read_list)
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config(config, data_dir) )
  {
    return;
  }

  kwiver::arrows::core::video_input_filter vif;

  EXPECT_TRUE( vif.check_configuration( config ) );
  vif.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;
  vif.open( list_file );

  kwiver::vital::timestamp ts;

  EXPECT_EQ( num_expected_frames, vif.num_frames() )
    << "Number of frames before extracting frames should be "
    << num_expected_frames;

  int num_frames = 0;
  while ( vif.next_frame( ts ) )
  {
    auto img = vif.frame_image();
    auto md = vif.frame_metadata();

    if (md.size() > 0)
    {
      std::cout << "-----------------------------------\n" << std::endl;
      kwiver::vital::print_metadata( std::cout, *md[0] );
    }

    ++num_frames;
    EXPECT_EQ( num_frames, ts.get_frame() )
      << "Frame numbers should be sequential";
    EXPECT_EQ( ts.get_frame(), decode_barcode(*img) )
      << "Frame number should match barcode in frame image";
  }
  EXPECT_EQ( num_expected_frames, num_frames )
    << "Number of frames found should be "
    << num_expected_frames;
  EXPECT_EQ( num_expected_frames, vif.num_frames() )
    << "Number of frames after extracting frames should be "
    << num_expected_frames;
}


// ----------------------------------------------------------------------------
TEST_F(video_input_filter, read_list_subset)
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config(config, data_dir) )
  {
    return;
  }

  config->set_value( "start_at_frame", "11" );
  config->set_value( "stop_after_frame", "30" );

  kwiver::arrows::core::video_input_filter vif;

  EXPECT_TRUE( vif.check_configuration( config ) );
  vif.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;
  vif.open( list_file );

  kwiver::vital::timestamp ts;

  EXPECT_EQ( num_expected_frames_subset, vif.num_frames() )
    << "Number of frames before extracting frames should be "
    << num_expected_frames_subset;

  int num_frames = 0;
  int frame_idx = 10;
  while ( vif.next_frame( ts ) )
  {
    auto img = vif.frame_image();
    auto md = vif.frame_metadata();

    if ( md.size() > 0 )
    {
      std::cout << "-----------------------------------\n" << std::endl;
      kwiver::vital::print_metadata( std::cout, *md[0] );
    }

    ++num_frames;
    ++frame_idx;
    EXPECT_EQ( frame_idx, ts.get_frame() )
      << "Frame numbers should be sequential";
    EXPECT_EQ( ts.get_frame(), decode_barcode(*img) )
      << "Frame number should match barcode in frame image";
  }
  EXPECT_EQ( num_expected_frames_subset, num_frames )
    << "Number of frames found should be "
    << num_expected_frames_subset;
  EXPECT_EQ( num_expected_frames_subset, vif.num_frames() )
    << "Number of frames after extracting frames should be "
    << num_expected_frames_subset;
}

TEST_F(video_input_filter, seek_frame)
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config(config, data_dir) )
  {
    return;
  }

  kwiver::arrows::core::video_input_filter vif;

  EXPECT_TRUE( vif.check_configuration( config ) );
  vif.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;

  // Open the video
  vif.open( list_file );

  test_seek_frame( vif );

  vif.close();
}

TEST_F(video_input_filter, seek_frame_sublist)
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config(config, data_dir) )
  {
    return;
  }

  config->set_value( "start_at_frame", "11" );
  config->set_value( "stop_after_frame", "30" );

  kwiver::arrows::core::video_input_filter vif;

  EXPECT_TRUE( vif.check_configuration( config ) );
  vif.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;

  // Open the video
  vif.open( list_file );

  test_seek_frame_sublist( vif );

  vif.close();
}

// ----------------------------------------------------------------------------
TEST_F(video_input_filter, test_capabilities)
{
  // make config block
  auto config = kwiver::vital::config_block::empty_config();

  if( !set_config(config, data_dir) )
  {
    return;
  }

  kwiver::arrows::core::video_input_filter vif;

  EXPECT_TRUE( vif.check_configuration( config ) );
  vif.set_configuration( config );

  kwiver::vital::path_t list_file = data_dir + "/" + list_file_name;
  vif.open( list_file );

  auto cap = vif.get_implementation_capabilities();
  auto cap_list = cap.capability_list();

  for ( auto one : cap_list )
  {
    std::cout << one << " -- "
              << ( cap.capability( one ) ? "true" : "false" )
              << std::endl;
  }
}
