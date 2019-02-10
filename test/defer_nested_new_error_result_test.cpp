// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int A>
struct info
{
	int value;
};

leaf::error_id f0()
{
	auto load = leaf::defer( [ ] { return info<0>{-1}; } );
	return leaf::new_error( info<1>{-1} );
}

leaf::error_id f1()
{
	auto load = leaf::defer( [ ] { return info<0>{0}; }, [ ] { return info<1>{1}; }, [ ] { return info<2>{2}; } );
	(void) f0();
	return leaf::new_error();
}

leaf::error_id f2()
{
	return f1().load( info<3>{3} );
}

int main()
{
	int r = leaf::try_handle_all(
		[ ]() -> leaf::result<int>
		{
			return f2();
		},
		[ ]( info<0> i0, info<1> i1, info<2> i2, info<3> i3 )
		{
			BOOST_TEST_EQ(i0.value, 0);
			BOOST_TEST_EQ(i1.value, 1);
			BOOST_TEST_EQ(i2.value, 2);
			BOOST_TEST_EQ(i3.value, 3);
			return 1;
		},
		[ ]
		{
			return 2;
		} );
	BOOST_TEST_EQ(r, 1);

	return boost::report_errors();
}
