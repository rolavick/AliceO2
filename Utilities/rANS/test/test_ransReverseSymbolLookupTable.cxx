// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// @file   test_ransReverseSymbolLookupTable.cxx
/// @author Michael Lettrich
/// @since  2021-06-02
/// @brief

#define BOOST_TEST_MODULE Utility test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/mpl/vector.hpp>

#include "rANS/rans.h"

template <typename T>
size_t getNUniqueSymbols(const T& container)
{
  return std::count_if(container.begin(), container.end(), [](uint32_t value) { return value != 0; });
};

BOOST_AUTO_TEST_CASE(test_empty)
{
  const auto renormedFrequencyTable = o2::rans::renorm(o2::rans::FrequencyTable{});
  const o2::rans::internal::ReverseSymbolLookupTable rLut{renormedFrequencyTable};

  const auto size = 1 << o2::rans::MinRenormThreshold;
  BOOST_CHECK_EQUAL(rLut.size(), size);

  const std::vector<int32_t> res(size, 0);
  BOOST_CHECK_EQUAL_COLLECTIONS(rLut.begin(), rLut.end(), res.begin(), res.end());
}

BOOST_AUTO_TEST_CASE(test_buildRLUT)
{
  const std::vector<int> A{5, 5, 6, 6, 8, 8, 8, 8, 8, -1, -5, 2, 7, 3};
  const std::vector<uint32_t> histA{1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 2, 2, 1, 5, 1};
  const size_t scaleBits = 17;
  const auto size = 1 << scaleBits;

  const auto renormedFrequencyTable = o2::rans::renorm(o2::rans::makeFrequencyTableFromSamples(A.begin(), A.end()), scaleBits);
  const o2::rans::internal::ReverseSymbolLookupTable rLut{renormedFrequencyTable};

  BOOST_CHECK_EQUAL(rLut.size(), size);

  const auto min = *std::min_element(A.begin(), A.end());
  const std::vector<uint32_t> frequencies{8738, 0, 0, 0, 8738, 0, 0, 8738, 8738, 0, 17476, 17477, 8738, 43690, 8739};
  const std::vector<uint32_t> cumulative{0, 8738, 8738, 8738, 8738, 17476, 17476, 17476, 26214, 34952, 34952, 52428, 69905, 78643, 122333};

  for (size_t i = 0; i < frequencies.size(); ++i) {
    const int symbol = min + i;
    for (size_t cumul = cumulative[i]; cumul < cumulative[i] + frequencies[i]; ++cumul) {
      BOOST_CHECK(cumul < rLut.size());
      BOOST_CHECK_EQUAL(rLut[cumul], symbol);
    }
  }
}
