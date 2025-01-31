// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// This file contains the implementation for the KLV specialization of
/// the vital::metadata class.

#include "klv_metadata.h"

namespace kwiver {

namespace arrows {

namespace klv {

// ----------------------------------------------------------------------------
vital::metadata*
klv_metadata
::clone() const
{
  return new klv_metadata{ *this };
}

// ----------------------------------------------------------------------------
std::vector< klv_packet > const&
klv_metadata
::klv() const
{
  return m_klv_packets;
}

// ----------------------------------------------------------------------------
std::vector< klv_packet >&
klv_metadata
::klv()
{
  return m_klv_packets;
}

} // namespace klv

} // namespace arrows

} // namespace kwiver
