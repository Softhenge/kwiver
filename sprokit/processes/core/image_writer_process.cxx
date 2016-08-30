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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS [yas] elisp error!AS IS''
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

#include "image_writer_process.h"

#include <vital/algorithm_plugin_manager.h>
#include <vital/vital_types.h>
#include <vital/types/image_container.h>
#include <vital/types/image.h>
#include <vital/types/timestamp.h>
#include <vital/algo/image_io.h>
#include <vital/exceptions.h>
#include <vital/util/string_format.h>

#include <kwiver_type_traits.h>

#include <sprokit/pipeline/process_exception.h>
#include <sprokit/pipeline/datum.h>

#include <kwiversys/SystemTools.hxx>

#include <vector>
#include <stdint.h>
#include <fstream>

// -- DEBUG
#if defined DEBUG
#include <arrows/algorithms/ocv/image_container.h>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
#endif

namespace algo = kwiver::vital::algo;

namespace kwiver {

// (config-key, value-type, default-value, description )
create_config_trait( file_name_template, std::string, "image%04d.png",
                     "Template for generating output file names. The template is interpreted as a printf format with one "
                     "format specifier to convert an integer increasing image number." );

//----------------------------------------------------------------
// Private implementation class
class image_writer_process::priv
{
public:
  priv();
  ~priv();

  // Configuration values
  std::string m_file_template;

  // Number for current image.
  kwiver::vital::timestamp::frame_t m_frame_number;

  // processing classes
  algo::image_io_sptr m_image_writer;

}; // end priv class


// ================================================================

image_writer_process
::image_writer_process( kwiver::vital::config_block_sptr const& config )
  : process( config ),
    d( new image_writer_process::priv )
{
  // Attach our logger name to process logger
  attach_logger( kwiver::vital::get_logger( name() ) ); // could use a better approach
  kwiver::vital::algorithm_plugin_manager::load_plugins_once();
  make_ports();
  make_config();
}


image_writer_process
::~image_writer_process()
{
}


// ----------------------------------------------------------------
void image_writer_process
::_configure()
{
  // Get process config entries
  d->m_file_template = config_value_using_trait( file_name_template );

  // Get algo conrig entries
  kwiver::vital::config_block_sptr algo_config = get_config(); // config for process
  algo::image_io::set_nested_algo_configuration( "image_writer", algo_config, d->m_image_writer);
  if ( ! d->m_image_writer )
  {
    throw sprokit::invalid_configuration_exception( name(),
             "Unable to create image_writer." );
  }

  // instantiate image reader and converter based on config type
  if ( ! algo::image_io::check_nested_algo_configuration( "image_writer", algo_config ) )
  {
    throw sprokit::invalid_configuration_exception( name(), "Configuration check failed." );
  }
}


// ----------------------------------------------------------------
void image_writer_process
::_step()
{
  if ( has_input_port_edge_using_trait( timestamp ) )
  {
    kwiver::vital::timestamp frame_time;
    frame_time = grab_from_port_using_trait( timestamp );
    if (frame_time.has_valid_frame() )
    {
      kwiver::vital::timestamp::frame_t next_frame;
      next_frame = frame_time.get_frame();

      if ( next_frame <= d->m_frame_number )
      {
        ++d->m_frame_number;
        LOG_WARN( logger(), "Frame number from input timestamp ("
                  << next_frame
                  << ") is not greater than last frame number. Adjusting frame number to "
                  << d->m_frame_number );
      }
    }
    else
    {
      ++d->m_frame_number;
    }
  }
  else
  {
    ++d->m_frame_number;
  }

  vital::image_container_sptr input = grab_from_port_using_trait( image );

  std::string a_file = kwiver::vital::string_format( d->m_file_template, d->m_frame_number );

  LOG_DEBUG( logger(), "Writing image to file \"" << a_file << "\"" );
  d->m_image_writer->save( a_file, input );
}


// ----------------------------------------------------------------
void image_writer_process
::make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t optional;
  sprokit::process::port_flags_t required;
  required.insert( flag_required );

  declare_input_port_using_trait( image, required );
  declare_input_port_using_trait( timestamp, optional );
}


// ----------------------------------------------------------------
void image_writer_process
::make_config()
{
  declare_config_using_trait( file_name_template );
}


// ================================================================
image_writer_process::priv
::priv()
  : m_frame_number(0)
{
}


image_writer_process::priv
::~priv()
{
}

} // end namespace
