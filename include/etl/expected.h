///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2022 John Wellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef ETL_EXPECTED_INCLUDED
#define ETL_EXPECTED_INCLUDED

#include "platform.h"
#include "type_traits.h"
#include "utility.h"
#include "variant.h"

namespace etl
{
  //***************************************************************************
  /// Base exception for et::expected
  //***************************************************************************
  class expected_exception : public etl::exception
  {
  public:

    expected_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// expected_invalid
  //***************************************************************************
  template <typename TError>
  class expected_invalid;

  //*******************************************
  template<>
  class expected_invalid<void> : public etl::expected_exception
  {
  public:

    expected_invalid(string_type file_name_, numeric_type line_number_)
      : expected_exception(ETL_ERROR_TEXT("expected:invalid", ETL_EXPECTED_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //*******************************************
  template <typename TError>
  class expected_invalid : etl::expected_invalid<void>
  {
  public:

    expected_invalid(string_type file_name_, numeric_type line_number_)
      : expected_invalid<void>(file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Unexpected type.
  /// etl::unexpected represents an unexpected value stored in etl::expected.
  //***************************************************************************
  template <typename TError>
  class unexpected
  {
  public:

    typedef TError error_type;

    //*******************************************
    /// Copy constructor.
    //*******************************************
    ETL_CONSTEXPR14 unexpected(const unexpected& other)
      : error_value(other.error_value)
    {
    }

#if ETL_USING_CPP11
    //*******************************************
    /// Move constructor.
    //*******************************************
    ETL_CONSTEXPR unexpected(unexpected&& other)
      : error_value(etl::move(other.error_value))
    {
    }
#endif

#if ETL_USING_CPP11
    //*******************************************
    /// Construct from argument.
    //*******************************************
    template <typename TErr = TError, typename = typename etl::enable_if<!etl::is_same<typename etl::remove_cvref<TErr>::type, etl::unexpected<TErr>>::value &&
                                                                         !etl::is_same<typename etl::remove_cvref<TErr>::type, etl::in_place_t>::value, int>::type>
    constexpr explicit unexpected(TErr&& e)
      : error_value(etl::forward<TErr>(e))
    {
    }
#else
    //*******************************************
    /// Construct from argument.
    //*******************************************
    template <typename TErr>
    explicit unexpected(const TErr& e, typename etl::enable_if<!etl::is_same<typename etl::remove_cvref<TErr>::type, etl::unexpected<TErr> >::value &&
                                                               !etl::is_same<typename etl::remove_cvref<TErr>::type, etl::in_place_t>::value, int>::type = 0)
      : error_value(e)
    {
    }
#endif

#if ETL_USING_CPP11
    //*******************************************
    /// Construct from arguments.
    //*******************************************
    template <typename... Args >
    constexpr explicit unexpected(etl::in_place_t, Args&&... args)
      : error_value(etl::forward<Args>(args)...)
    {
    }
#endif

#if ETL_HAS_INITIALIZER_LIST
    //*******************************************
    /// Construct from initializer_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    constexpr explicit unexpected(etl::in_place_t, std::initializer_list<U> init, Args&&... args)
      : error_value(init, etl::forward<Args>(args)...)
    {
    }
#endif

    //*******************************************
    /// Assign from etl::unexpected.
    //*******************************************
    ETL_CONSTEXPR14
    etl::unexpected<TError>& operator =(const etl::unexpected<TError>& rhs)
    {
      ETL_STATIC_ASSERT(etl::is_copy_constructible<TError>::value, "Error not copy assignable");

      error_value = rhs.error_value;
      return *this;
    }

#if ETL_USING_CPP11
    //*******************************************
    /// Move assign from etl::unexpected.
    //*******************************************
    ETL_CONSTEXPR14
    etl::unexpected<TError>& operator =(etl::unexpected<TError>&& rhs)
    {
      ETL_STATIC_ASSERT(etl::is_move_constructible<TError>::value, "Error not move assignable");

      error_value = etl::move(rhs.error_value);
      return *this;
    }
#endif

#if ETL_USING_CPP11
    //*******************************************
    /// Get the error.
    //*******************************************
    constexpr TError& error() & noexcept
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************
    constexpr const TError& error() const& noexcept
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************    
    constexpr TError&& error() && noexcept
    {
      return etl::move(error_value);
    }

    //*******************************************
    /// Get the error.
    //*******************************************    
    constexpr TError&& error() const && noexcept
    {
      return etl::move(error_value);
    }
#else
    //*******************************************
    /// Get the error.
    //*******************************************
    const TError& error() const
    {
      return error_value;
    }
#endif

    //*******************************************
    /// Swap with another etl::unexpected.
    //*******************************************
    void swap(etl::unexpected<TError>& other)
    {
      using ETL_OR_STD::swap;

      swap(error_value, other.error_value);
    }

  private:

    TError error_value;
  };

#if ETL_USING_CPP17
  template <typename TError>
  bool operator ==(const etl::unexpected<TError>& lhs, const etl::unexpected<TError>& rhs)
  {
    return lhs.error_value == rhs.error_value;
  }

  //*******************************************
  /// Swap etl::unexpected.
  //*******************************************
  template <typename TError>
  void swap(etl::unexpected<TError>& lhs, etl::unexpected<TError>& rhs)
  {
    lhs.swap(rhs);
  }

  //*****************************************************************************
  /// unexpect_t
  //*****************************************************************************
  struct unexpect_t 
  {
    ETL_CONSTEXPR14 explicit unexpect_t()
    {
    }
  };
#endif

#if ETL_USING_CPP17
  inline constexpr unexpect_t unexpect{};
#else
  static const unexpect_t unexpect;
#endif

  //*****************************************************************************
  /// Expected type.
  //*****************************************************************************
  template <typename TValue, typename TError>
  class expected
  {
  public:

    typedef etl::expected<TValue, TError> this_type;
    typedef TValue                        value_type;
    typedef TError                        error_type;
    typedef etl::unexpected<TError>       unexpected_type;

    template <typename U>
    using rebind = expected<U, TError>;

    //*******************************************
    /// Default constructor
    //*******************************************
    ETL_CONSTEXPR14 expected() ETL_NOEXCEPT
      : storage(etl::in_place_index<Value_Type>, value_type())
    {
    }

    //*******************************************
    /// Constructor
    //*******************************************
    ETL_CONSTEXPR14 expected(const value_type& value_) ETL_NOEXCEPT
      : storage(etl::in_place_index<Value_Type>, value_)
    {
    }

    //*******************************************
    /// Constructor
    //*******************************************
    ETL_CONSTEXPR14 expected(value_type&& value_) ETL_NOEXCEPT
      : storage(etl::in_place_index<Value_Type>, etl::move(value_))
    {
    }

    //*******************************************
    /// Copy constructor
    //*******************************************
    ETL_CONSTEXPR14 expected(const expected& other) ETL_NOEXCEPT
      : storage(other.storage)
    {
    }

    //*******************************************
    /// Move constructor
    //*******************************************
    ETL_CONSTEXPR14 expected(expected&& other) ETL_NOEXCEPT
      : storage(etl::move(other.storage))
    {
    }

    //*******************************************
    /// Copy construct from unexpected type.
    //*******************************************
    template <typename F>
    ETL_CONSTEXPR14 explicit expected(const etl::unexpected<F>& ue)
      : storage(etl::in_place_index<Error_Type>, ue.error())
    {
    }

    //*******************************************
    /// Move construct from unexpected type.
    //*******************************************
    template <typename F>
    ETL_CONSTEXPR14 explicit expected(etl::unexpected<F>&& ue)
      : storage(etl::in_place_index<Error_Type>, etl::move(ue.error()))
    {
    }

    //*******************************************
    /// Construct with default value type.
    //*******************************************
    ETL_CONSTEXPR14 explicit expected(etl::in_place_t) ETL_NOEXCEPT
      : storage(value_type())
    {
    }

    //*******************************************
    /// Construct value type from arguments.
    //*******************************************
    template <typename... Args>
    ETL_CONSTEXPR14 explicit expected(etl::in_place_t, Args&&... args)
      : storage(etl::forward<Args>(args)...)
    {
    }

    //*******************************************
    /// Construct value type from initializser_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    ETL_CONSTEXPR14 explicit expected(etl::in_place_t, std::initializer_list<U> il, Args&&... args)
      : storage(il, etl::forward<Args>(args)...)
    {
    }

    //*******************************************
    /// Construct error type from arguments.
    //*******************************************
    template <typename... Args>
    ETL_CONSTEXPR14 explicit expected(etl::unexpect_t, Args&&... args)
      : storage(error_type(etl::forward<Args>(args)...))
    {
    }

    //*******************************************
    /// Construct error type from initializser_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    ETL_CONSTEXPR14 explicit expected(etl::unexpect_t, std::initializer_list<U> il, Args&&... args)
      : storage(error_type(il, etl::forward<Args>(args)...))
    {
    }

    //*******************************************
    /// 
    //*******************************************
    this_type& operator =(const this_type& other)
    {
      ETL_STATIC_ASSERT(etl::is_copy_constructible<TValue>::value && etl::is_copy_constructible<TError>::value, "Not copy assignable");

      storage = other.storage;

      return *this;
    }

    //*******************************************
    /// 
    //*******************************************
    this_type& operator =(this_type&& other)
    {
      ETL_STATIC_ASSERT(etl::is_move_constructible<TValue>::value && etl::is_move_constructible<TError>::value, "Not move assignable");

      storage = etl::move(other.storage);
      return *this;
    }

    //*******************************************
    /// Copy assign from value
    //*******************************************
    expected& operator =(const value_type& value)
    {
      ETL_STATIC_ASSERT(etl::is_copy_constructible<TValue>::value, "Value not copy assignable");

      storage.emplace<Value_Type>(value);
      return *this;
    }

    //*******************************************
    /// Move assign from value
    //*******************************************
    expected& operator =(value_type&& value)
    {
      ETL_STATIC_ASSERT(etl::is_move_constructible<TValue>::value, "Value not move assignable");

      storage.emplace<Value_Type>(etl::move(value));
      return *this;
    }

    //*******************************************
    /// Copy assign from error
    //*******************************************
    expected& operator =(const unexpected_type& error)
    {
      ETL_STATIC_ASSERT(etl::is_copy_constructible<TError>::value, "Error not copy assignable");

      storage.emplace<Error_Type>(error);
      return *this;
    }

    //*******************************************
    /// Move assign from error
    //*******************************************
    expected& operator =(unexpected_type&& error)
    {
      ETL_STATIC_ASSERT(etl::is_move_constructible<TError>::value, "Error not move assignable");

      storage.emplace<Error_Type>(etl::move(error));
      return *this;
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 value_type& value()&
    {
      return etl::get<Value_Type>(storage);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 const value_type& value() const&
    {
      return etl::get<Value_Type>(storage);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 value_type&& value()&&
    {
      return etl::move(etl::get<Value_Type>(storage));
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 const value_type&& value() const&&
    {
      return etl::move(etl::get<Value_Type>(storage));
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14 
    bool has_value() const
    {
      return (storage.index() == Value_Type);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14 
    operator bool() const
    {
      return has_value();
    }

    //*******************************************
    /// 
    //*******************************************
    template <typename U>
    ETL_NODISCARD
    ETL_CONSTEXPR14 
    value_type value_or(U&& default_value) const&
    {
      if (has_value())
      {
        return value();
      }
      else
      {
        return default_value;
      }
    }

    //*******************************************
    /// 
    //*******************************************
    template <typename U>
    ETL_NODISCARD
    ETL_CONSTEXPR14
    value_type value_or(U&& default_value)&&
    {
      if (has_value())
      {
        return etl::move(value());
      }
      else
      {
        return etl::move(default_value);
      }
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14 
    error_type& error()& ETL_NOEXCEPT
    {
      return etl::get<Error_Type>(storage);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    const error_type& error() const& ETL_NOEXCEPT
    {
      return etl::get<Error_Type>(storage);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    error_type&& error() && ETL_NOEXCEPT
    {
      return etl::move(etl::get<Error_Type>(storage));
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    const error_type&& error() const&& ETL_NOEXCEPT
    {
      return etl::move(etl::get<Error_Type>(storage));
    }

    //*******************************************
    /// 
    //*******************************************
    template <typename... Args>
    ETL_CONSTEXPR14 value_type& emplace(Args&&... args) ETL_NOEXCEPT
    {
      storage.emplace(args...);
    }

    //*******************************************
    /// 
    //*******************************************
    template <typename U, typename... Args>
    ETL_CONSTEXPR14 value_type& emplace(std::initializer_list<U>& il, Args&&... args) ETL_NOEXCEPT
    {
      storage.emplace(il, args...);
    }

    //*******************************************
    /// 
    //*******************************************
    value_type* operator ->()
    {
#if ETL_IS_DEBUG_BUILD
      ETL_ASSERT(valid, ETL_ERROR(expected_invalid));
#endif

      return etl::addressof(storage.get<Value_Type>());
    }

    //*******************************************
    /// 
    //*******************************************
    const value_type* operator ->() const
    {
#if ETL_IS_DEBUG_BUILD
      ETL_ASSERT(valid, ETL_ERROR(expected_invalid));
#endif

      return etl::addressof(storage.get<Value_Type>());
    }

    //*******************************************
    /// 
    //*******************************************
    value_type& operator *()
    {
#if ETL_IS_DEBUG_BUILD
      ETL_ASSERT(valid, ETL_ERROR(expected_invalid));
#endif

      return storage.get<Value_Type>();
    }

    //*******************************************
    /// 
    //*******************************************
    const value_type& operator *() const
    {
#if ETL_IS_DEBUG_BUILD
      ETL_ASSERT(valid, ETL_ERROR(expected_invalid));
#endif

      return storage.get<Value_Type>();
    }

  private:

    enum
    {
      Uninitialised,
      Value_Type,
      Error_Type
    };

    using storage_type = etl::variant<etl::monostate, value_type, error_type>;
    storage_type storage;
  };

  //*****************************************************************************
  /// Specialisation for void value type.
  //*****************************************************************************
  template<typename TError>
  class expected<void, TError>
  {
  public:

    typedef etl::expected<void, TError> this_type; 
    typedef void                        value_type;
    typedef TError                      error_type;
    typedef etl::unexpected<TError>     unexpected_type;
    
    //*******************************************
    /// Default constructor
    //*******************************************
    ETL_CONSTEXPR14
    expected()
    {
    }

    //*******************************************
    /// Copy construct from unexpected
    //*******************************************
    ETL_CONSTEXPR14
    expected(const unexpected_type& ue_)
      : storage(ue_.error())
    {
    }

    //*******************************************
    /// Move construct from unexpected
    //*******************************************
    ETL_CONSTEXPR14
    expected(unexpected_type&& ue_)
      : storage(etl::move(ue_.error()))
    {
    }

    //*******************************************
    /// Copy construct
    //*******************************************
    ETL_CONSTEXPR14
      expected(const this_type& other)
      : storage(other.storage)
    {
    }

    //*******************************************
    /// Move construct
    //*******************************************
    ETL_CONSTEXPR14
      expected(this_type&& other)
      : storage(etl::move(other.storage))
    {
    }

    //*******************************************
    /// Copy assign
    //*******************************************
    this_type& operator =(const this_type& other)
    {
      ETL_STATIC_ASSERT(etl::is_copy_constructible<TError>::value, "Not copy assignable");

      storage = other.storage;
      return *this;
    }

    //*******************************************
    /// Move assign
    //*******************************************
    this_type& operator =(this_type&& other)
    {
      ETL_STATIC_ASSERT(etl::is_move_constructible<TError>::value, "Not move assignable");

      storage = etl::move(other.storage);
      return *this;
    }

    //*******************************************
    /// Copy assign from error
    //*******************************************
    expected& operator =(const unexpected_type& error)
    {
      ETL_STATIC_ASSERT(etl::is_copy_constructible<TError>::value, "Error not copy assignable");

      storage.emplace<Error_Type>(error);
      return *this;
    }

    //*******************************************
    /// Move assign from error
    //*******************************************
    expected& operator =(unexpected_type&& error)
    {
      ETL_STATIC_ASSERT(etl::is_move_constructible<TError>::value, "Error not move assignable");

      storage.emplace<Error_Type>(etl::move(error));
      return *this;
    }

    //*******************************************
    /// Returns true if expected has a value
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14 
    bool has_value() const
    {
      return (storage.index() != Error_Type);
    }

    //*******************************************
    /// Returns true if expected has a value
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14 
    operator bool() const
    {
      return has_value();
    }

    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    error_type& error()& ETL_NOEXCEPT
    {
      return etl::get<Error_Type>(storage);
    }

    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    const error_type& error() const& ETL_NOEXCEPT
    {
      return etl::get<Error_Type>(storage);
    }

    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    error_type&& error() && ETL_NOEXCEPT
    {
      return etl::move(etl::get<Error_Type>(storage));
    }

    //*******************************************
    /// Returns the error
    /// Undefined behaviour if an error has not been set.
    //*******************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    const error_type&& error() const&& ETL_NOEXCEPT
    {
      return etl::move(etl::get<Error_Type>(storage));
    }

  private:

    enum
    {
      Uninitialised,
      Error_Type
    };

    etl::variant<etl::monostate, error_type> storage;
  };
}

//*******************************************
/// Equivalence operator.
//*******************************************
template <typename TError>
ETL_CONSTEXPR14
bool operator ==(const etl::unexpected<TError>& lhs, const etl::unexpected<TError>& rhs)
{
  return lhs.error_value == rhs.error_value;
}

//*******************************************
/// Swap etl::unexpected.
//*******************************************
template <typename TError>
ETL_CONSTEXPR14
void swap(etl::unexpected<TError>& lhs, etl::unexpected<TError>& rhs)
{
  lhs.swap(rhs);
}

#endif
