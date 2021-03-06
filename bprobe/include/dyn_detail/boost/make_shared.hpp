#ifndef BOOST_MAKE_SHARED_HPP_INCLUDED
#define BOOST_MAKE_SHARED_HPP_INCLUDED

//  make_shared.hpp
//
//  Copyright (c) 2007, 2008 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  See http://www.boost.org/libs/smart_ptr/make_shared.html
//  for documentation.

#include <dyn_detail/boost/config.hpp>
#include <dyn_detail/boost/shared_ptr.hpp>
#include <dyn_detail/boost/type_traits/type_with_alignment.hpp>
#include <dyn_detail/boost/type_traits/alignment_of.hpp>
#include <cstddef>
#include <new>

namespace dyn_detail
{

namespace boost
{

namespace detail
{

template< std::size_t N, std::size_t A > struct sp_aligned_storage
{
    union type
    {
        char data_[ N ];
        typename ::dyn_detail::boost::type_with_alignment< A >::type align_;
    };
};

template< class T > class sp_ms_deleter
{
private:

    typedef typename sp_aligned_storage< sizeof( T ), boost::alignment_of< T >::value >::type storage_type;

    bool initialized_;
    storage_type storage_;

private:

    void destroy()
    {
        if( initialized_ )
        {
            reinterpret_cast< T* >( storage_.data_ )->~T();
            initialized_ = false;
        }
    }

public:

    sp_ms_deleter(): initialized_( false )
    {
    }

    ~sp_ms_deleter()
    {
        destroy();
    }

    void operator()( T * )
    {
        destroy();
    }

    void * address()
    {
        return storage_.data_;
    }

    void set_initialized()
    {
        initialized_ = true;
    }
};

template< class T > T forward( T t )
{
    return t;
}

} // namespace detail

// Zero-argument versions
//
// Used even when variadic templates are available because of the new T() vs new T issue

template< class T > boost::shared_ptr< T > make_shared()
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T();
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A > boost::shared_ptr< T > allocate_shared( A const & a )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T();
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

#if defined( BOOST_HAS_VARIADIC_TMPL ) && defined( BOOST_HAS_RVALUE_REFS )

// Variadic templates, rvalue reference

template< class T, class... Args > boost::shared_ptr< T > make_shared( Args && ... args )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( detail::forward<Args>( args )... );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class... Args > boost::shared_ptr< T > allocate_shared( A const & a, Args && ... args )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( detail::forward<Args>( args )... );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

#else

// C++03 version

template< class T, class A1 >
boost::shared_ptr< T > make_shared( A1 const & a1 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A1, class A2 >
boost::shared_ptr< T > make_shared( A1 const & a1, A2 const & a2 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1, class A2 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1, A2 const & a2 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A1, class A2, class A3 >
boost::shared_ptr< T > make_shared( A1 const & a1, A2 const & a2, A3 const & a3 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1, class A2, class A3 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1, A2 const & a2, A3 const & a3 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A1, class A2, class A3, class A4 >
boost::shared_ptr< T > make_shared( A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1, class A2, class A3, class A4 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A1, class A2, class A3, class A4, class A5 >
boost::shared_ptr< T > make_shared( A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1, class A2, class A3, class A4, class A5 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A1, class A2, class A3, class A4, class A5, class A6 >
boost::shared_ptr< T > make_shared( A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5, a6 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1, class A2, class A3, class A4, class A5, class A6 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5, a6 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
boost::shared_ptr< T > make_shared( A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6, A7 const & a7 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5, a6, a7 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6, A7 const & a7 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5, a6, a7 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
boost::shared_ptr< T > make_shared( A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6, A7 const & a7, A8 const & a8 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5, a6, a7, a8 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6, A7 const & a7, A8 const & a8 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5, a6, a7, a8 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
boost::shared_ptr< T > make_shared( A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6, A7 const & a7, A8 const & a8, A9 const & a9 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >() );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

template< class T, class A, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
boost::shared_ptr< T > allocate_shared( A const & a, A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6, A7 const & a7, A8 const & a8, A9 const & a9 )
{
    boost::shared_ptr< T > pt( static_cast< T* >( 0 ), detail::sp_ms_deleter< T >(), a );

    detail::sp_ms_deleter< T > * pd = boost::get_deleter< detail::sp_ms_deleter< T > >( pt );

    void * pv = pd->address();

    new( pv ) T( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
    pd->set_initialized();

    return boost::shared_ptr< T >( pt, static_cast< T* >( pv ) );
}

#endif

} // namespace boost

} // namespace dyn_detail

#endif // #ifndef BOOST_MAKE_SHARED_HPP_INCLUDED
