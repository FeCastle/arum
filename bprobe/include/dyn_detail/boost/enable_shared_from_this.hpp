#ifndef DYN_DETAIL_BOOST_ENABLE_SHARED_FROM_THIS_HPP_INCLUDED
#define DYN_DETAIL_BOOST_ENABLE_SHARED_FROM_THIS_HPP_INCLUDED

//
//  enable_shared_from_this.hpp
//
//  Copyright (c) 2002 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  http://www.boost.org/libs/smart_ptr/enable_shared_from_this.html
//

#include <dyn_detail/boost/weak_ptr.hpp>
#include <dyn_detail/boost/shared_ptr.hpp>
#include <dyn_detail/boost/assert.hpp>
#include <dyn_detail/boost/config.hpp>

namespace dyn_detail
{
  
namespace boost
{

template<class T> class enable_shared_from_this
{
protected:

    enable_shared_from_this()
    {
    }

    enable_shared_from_this(enable_shared_from_this const &)
    {
    }

    enable_shared_from_this & operator=(enable_shared_from_this const &)
    {
        return *this;
    }

    ~enable_shared_from_this()
    {
    }

public:

    shared_ptr<T> shared_from_this()
    {
        shared_ptr<T> p(_internal_weak_this);
        DYN_DETAIL_BOOST_ASSERT(p.get() == this);
        return p;
    }

    shared_ptr<T const> shared_from_this() const
    {
        shared_ptr<T const> p(_internal_weak_this);
        DYN_DETAIL_BOOST_ASSERT(p.get() == this);
        return p;
    }

    typedef T _internal_element_type; // for bcc 5.5.1
    mutable weak_ptr<_internal_element_type> _internal_weak_this;
};

} // namespace boost
} // namespace dyn_detail

#endif  // #ifndef DYN_DETAIL_BOOST_ENABLE_SHARED_FROM_THIS_HPP_INCLUDED
