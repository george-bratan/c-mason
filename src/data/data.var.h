#include "data.h"

#ifndef DATA_VALUE_H
#define DATA_VALUE_H

namespace mason {
namespace data {

	class var;

	typedef std::vector<var> vector;
	typedef std::map<std::string, var> object;

	typedef std::function<void(var&, var&)> caster;
	static std::unordered_map<size_t, std::unordered_map<size_t, caster>> _casters;

	template<typename T>
	var ref(T& value);

	template<typename T, template<typename...> class Ref>
	struct is_specialization : std::false_type {};

	template<template<typename...> class Ref, typename... Args>
	struct is_specialization<Ref<Args...>, Ref>: std::true_type {};

	//
	class var
	{
		public:
			// null variant
			var(): _flags(flags::none), _data(nullptr), _type(nullptr) 
			{
				//
			}

			// variant from object
			template <typename T>
			var(const T& data): _flags(flags::none) 
			{
				// if array, convert to const reference
				//if (is_array<T>::value) 
				//	set_const();

				// initializer
				init<T>(*this)(const_cast<T&>(data));
			}

			template <typename T>
			var(T& data, int flags): _flags(flags)
			{
				typedef typename std::remove_const<T>::type non_const;

				// maintain constness if reference
				//if (is_ref() && std::is_const<T>::value) 
				//	set_const();

				// initializer
				init<non_const>(*this)(const_cast<non_const&>(data));
			}

			var(std::initializer_list<var> values): _flags(flags::none), _data(nullptr), _type(nullptr) 
			{
				bool is_object = true;
				for (auto& i: values) {
					//
					is_object = is_object && i.is_vector() && i.size() == 2 && i.at(0).is_string();
				}

				if (is_object) {
					//
					*this = object();
					for (auto& i: values) {
						// force copy constructor with const cast
						// instead of template constructor
			    		as<object>()[ i.at(0).as<std::string>() ] = const_cast<const var&>( i.at(1) );
					}
				}
				else {
					//
					*this = vector();
					for (auto& i: values) {
						as<vector>().push_back( i );
					}
				}
			}

			// copy construct
			var(const var& source): _flags(flags::none), _data(nullptr), _type(nullptr)
			{
				if (!source._type)
					return;


				_type = source._type;

				if (source._flags & flags::copy_by_ref)
				{
					// copy by reference
					_data = source._ptr();
					_type = source._type;
					_flags = source._flags | flags::is_xref;
				}
				else
				{
					// allocate memory
					_allocate();

					//
					if (_type->is_class()) {
						//
						try {
							// copy constructor
							_type->copy(source, *this);
						}
						catch(std::exception& e) {
							// release memory
							_release();

							throw e;
						}
					}
					else {
						// raw copy
						std::memcpy(_ptr(), source._ptr(), _type->size());
					}
				}
			}

			// move construct
			var(var && source): _flags(flags::none), _data(nullptr), _type(nullptr)
			{
				std::swap(_data, source._data);
				std::swap(_type, source._type);
				std::swap(_flags, source._flags);
			}

			~var() 
			{
				if (!_data) 
					return;

				if (is_ref())
					return;

				//
				if (_type->is_class()) {
					// destructor
					_type->destroy(*this);
				}

				// release memory
				_release();
			}

			// return reference to variant
			var get_ref() const
			{
				var ref;
				ref._data = _data;
				ref._type = _type;
				ref._flags = _flags | flags::is_xref;

				// need a const cast to force copy constructor
				// instead of template constructor
				return const_cast<const var&>(ref);
			}

			// set const flag, cannot be reversed
			void set_const()
			{
				_flags = _flags | flags::is_xconst;
			}

			// implicit cast
			template <typename T>
			operator T&() 
			{
				return as<T>();
			}

			// explicit cast
			template <typename T>
			T& as() const
			{
				// get static type
				const type * _as_type = &get_type<T>();

				// const correctness
				//if (is_const() && !std::is_const<T>::value)
				//	throw std::domain_error("const exception");

				//
				if (_type == _as_type) {
					// trivial case
					return * reinterpret_cast<T*>(_ptr());
				}
				else {
					// need cast
					caster caster;
					bool found = get_caster( *_type, *_as_type, caster );

					if (found) {
						//
						var result;
						var& non_const = const_cast<var&>(*this);
						caster(non_const, result);

						return result.as<T>();
					}

					throw std::domain_error("cast exception " + _type->name() + " as " + _as_type->name());
				}
			}

			std::string as_string() const
			{
				if (is_null())
					return "null";
				if (is_bool())
					return as<bool>() ? "true" : "false";
				if (is_int())
					return std::to_string( as<long>() );
				if (is_float())
					return std::to_string( as<double>() );
				if (is_string())
					return as<std::string>();

				return "";
			}

			template <typename T>
			bool is() const
			{
				const type* _is_type = &get_type<T>();
				return (_type == _is_type);
			}

			//
			bool is_ref() const 	{ return _flags & flags::is_xref; 	}
			bool is_const() const 	{ return _flags & flags::is_xconst; }

			// broad type checking
			bool is_null() const 	{ return this->_type->is_null(); 	};
			bool is_bool() const 	{ return this->_type->is_bool(); 	};
			bool is_int() const 	{ return this->_type->is_int(); 	};
			bool is_float() const 	{ return this->_type->is_float(); 	};
			bool is_string() const 	{ return this->_type->is_string(); 	};
			bool is_vector() const 	{ return this->_type->is_vector(); 	};
			bool is_object() const 	{ return this->_type->is_object(); 	};

			// get type name
			std::string name() const { return this->_type->name(); 	};

			// copy assignment
			var& operator=(var value)
			{
				if (is_ref()) 
				{
					const type * _in_type = value._type;

					if (_type == _in_type) {
						//
						std::memcpy(_ptr(), value._ptr(), _type->size());
					}
					else {
						//
						caster caster;
						bool found = get_caster( *_in_type, *_type, caster );

						if (!found) {
							//
							throw std::domain_error("cast exception: " + _in_type->name() + " to " + _type->name());
						}

						//
						var out_value;
						var in_value = value;
						caster(value, out_value);

						std::memcpy(_ptr(), out_value._ptr(), _type->size());
					}
				}
				else
				{
					std::swap(_data, value._data);
					std::swap(_type, value._type);
					std::swap(_flags, value._flags);
				}

				return *this;
			}
			
			// copy assignment
			template <typename T>
			const T& operator =(const T& value)
			{
				// get static type 
				const type * _in_type = &get_type<T>();

				// existing object
				if (_type && _type->is_class()) {
					// destroy
					_type->destroy(*this);
				}

				if (!_type || is_null()) {
					// new type
					_type = _in_type;

					// allocate memory
					_allocate();
				}

				if (_type == _in_type) {
					//
					if (_type->is_class()) {
						//
						_type->copy(data::ref(value), *this);
					}
					else {
						// raw copy
						T& non_const = const_cast<T&>(value);
						void * _ptr_value = static_cast<void*>(&non_const);

						std::memcpy(_ptr(), _ptr_value, sizeof(value));
					}
				}
				else {
					//
					caster caster;
					bool found = get_caster( *_in_type, *_type, caster );

					if (!found) {
						//
						throw std::domain_error("cast exception: " + _in_type->name() + " to " + _type->name());
					}

					//
					var out_value;
					var in_value = value;
					caster(in_value, out_value);

					std::memcpy(_ptr(), out_value._ptr(), _type->size());
				}

				// return the value
				return value;
			}

			const char* operator =(const char * value)
			{
				operator = ( std::string(value) );
				return value;
			}

			// indexing
    		var& at(const size_t index) const {
		        // 
		        return as<vector>().at(index);
		    }

		    template <typename T, typename std::enable_if<std::is_constructible<std::string, T>::value, int>::type = 0>
		    var& at(const T& index) const {
		        // 
		        return as<object>().at(index);
		    }

		    var& operator [] (const size_t index) {
		        // 
		        if (is_null()) {
		            *this = vector();
		        }

		        return as<vector>()[ index ];
		    }

		    template <typename T, typename std::enable_if<std::is_constructible<std::string, T>::value, int>::type = 0>
		    var& operator [] (const T& index) {
		        // 
		        if (is_null()) {
		            *this = object();
		        }

		        return as<object>()[ index ];
		    }

		    template <typename T, typename std::enable_if<!std::is_array<T>::value, int>::type = 0>
		    bool operator == (const T& value) { 
		    	//
		    	return as<T>() == value;
		    }

		    template <typename T, typename std::enable_if<std::is_array<T>::value, int>::type = 0>
		    bool operator == (const T& value) { 
		    	//
		    	return operator == ( std::string(value) );
		    }

		    // shortcut
		    int size() const
		    {
		    	if (is<vector>())
		    		return as<vector>().size();

		    	if (is<object>())
		    		return as<object>().size();

		    	if (is_null())
		    		return 0;

		    	return 1;
		    }

		    // shortcut method
		    void push_back(const var& value)
		    {
		    	bool is_object = value.is_vector() && value.size() == 2 && value.at(0).is_string();

		    	if (is_null() && is_object) 
		            *this = object();

		        if (is_null()) 
		            *this = vector();

		    	if (is<vector>()) {
		    		//
		    		as<vector>().push_back(value);
		    	}

		    	if (is<object>() && is_object) {
		    		// force copy constructor with const cast
					// instead of template constructor
		    		as<object>()[value.at(0).as<std::string>()] = const_cast<const var&>(value.at(1));
		    	}
		    }
			
		public:
			template <typename T>
			friend var ref(T& value);

		private:
			enum flags
			{
				none = 0,
				is_xref = 1,
				is_xconst = 2,
				copy_by_ref = 4
			};

		private:
			void _allocate() 
			{
				if (is_ref() || _type->size() > sizeof(_data)) {
					// allocate memory 
					_data = ::operator new(_type->size());
				}
			}

			void _release()
			{
				if (is_ref() || _type->size() > sizeof(_data)) {
					// release memory
					::operator delete(_data);
				}
			}

			void * _ptr() const
			{
				if (is_ref() || _type->size() > sizeof(_data)) {
					// _data holds pointer to value
					return _data;
				}

				// _data holds value
				return const_cast<void**>(&_data);
			}

		private:
			int _flags;
			void * _data;

		private:
			template <typename T>
			struct init
			{
				public:
					init(var& value): _value(value) {}

					void operator()(T& data) 
					{
						// ensure type is registered.
						_value._type = &get_type<T>();

						//
						if (_value.is_ref()) 
						{
							// store pointer to data
							_value._data = &data;
						}
						else 
						{
							// allocate memory
							_value._allocate();

							//
							if (_value._type->is_class()) {
								//
								try {
									// copy constructor
									_value._type->copy(data::ref(data), _value);
								}
								catch(std::exception& e) {
									// release memory
									_value._release();

									throw e;
								}
							}
							else {
								// raw copy
								void * _ptr_data = static_cast<void*>(&data);
								std::memcpy(_value._ptr(), _ptr_data, sizeof(data));
							}
						}
					}

				private:
					// reference to variant
					var& _value;
			};

			// array initializer
			template<typename T, std::size_t size>
			struct init<T[size]>
			{
				public:
					init(var& value) : _value(value) {};

					void operator()(T data[size])
					{
						// init as reference to first element
						init<T*> initializer(_value);
						initializer(data);
					}

				private:
					// reference to variant
					var& _value;
			};

		private:
			struct type_base
			{
				type_base(const std::type_info& info): _info(&info) {}

				virtual void create(var& value) const = 0;
				virtual void destroy(var& value) const = 0;
				virtual void copy(const var& source, var& dest) const = 0;

				virtual int size() const { return 0; };
				virtual bool is_class() const { return false; };
				virtual bool is_const() const { return false; };
				virtual bool is_null() const { return false; };
				virtual bool is_bool() const { return false; };
				virtual bool is_int() const { return false; };
				virtual bool is_float() const { return false; };
				virtual bool is_string() const { return false; };
				virtual bool is_vector() const { return false; };
				virtual bool is_object() const { return false; };

				size_t hash() const {
					return _info->hash_code();
				}

				virtual std::string name() const {
					return _info->name();
				}

				private:
					const std::type_info * _info;
			};

			template <typename T>
			struct type_impl: public type_base
			{
				type_impl(): type_base(typeid(T)) {}

				void create(var& value) const
				{
					new (&value.as<T>()) T();
				}

				void destroy(var& value) const
				{
					value.as<T>().~T();
				}

				void copy(const var& source, var& dest) const
				{
					var& non_const = const_cast<var&>(source);
					new (&dest.as<T>()) T(non_const.as<T>());
				}

				int size() const
				{
					return sizeof(T);
				}

				bool is_class() const
				{
					return std::is_class<T>::value;
				}

				bool is_const() const
				{
					return std::is_const<T>::value;
				}

				bool is_null() const
				{
					return std::is_null_pointer<T>::value;
				}

				bool is_bool() const
				{
					return std::is_same<T, bool>::value;
				}

				bool is_int() const
				{
					return std::is_integral<T>::value && !is_bool();
				}

				bool is_float() const
				{
					return std::is_floating_point<T>::value;
				}

				bool is_string() const
				{
					return std::is_constructible<std::string, T>::value && !is_null();
				}

				bool is_vector() const
				{
					return data::is_specialization<T, std::vector>::value;
				}

				bool is_object() const
				{
					return data::is_specialization<T, std::map>::value;
				}

				std::string name() const 
				{
					return util::name<T>();
				}
			};

		private:
			typedef type_base type;

			template <typename T>
			static const type& get_type()
			{
				static type* _type = nullptr;

				// check for already registered type
				if (_type) return *_type;

				_type = new type_impl<T>();

				return *_type;
			}

		private:
			const type * _type;

		//
		public:
			static std::nullptr_t setup()
			{
				add_static<int, long>();
				add_static<long, int>();
				add_static<float, double>();
				add_static<double, float>();
				add_static<char *, std::string>();
				
				add_caster<std::string, char *>([](var& source, var& dest) {
					//
					dest = const_cast<char *>( source.as<std::string>().c_str() );
				});

				return nullptr;
			}

		private:
			static bool get_caster(const type& from, const type& to, caster& caster)
			{
				if (_casters.count(from.hash())) {
					//
					if (_casters[from.hash()].count(to.hash())) {
						//
						caster = _casters[from.hash()][to.hash()];
						return true;
					}
				}

				return false;
			}

			template <typename S, typename D>
			static void add_caster(caster caster)
			{
				const type& from = get_type<S>();
				const type& to = get_type<D>();

				_casters[from.hash()][to.hash()] = caster;
			}

			template <typename S, typename D>
			static void add_static()
			{
				add_caster<S, D>([](var& source, var& dest) {
					//
					dest = static_cast<D>( source.as<S>() );
				});
			}

			template <typename S, typename D>
			static void add_dynamic()
			{
				add_caster<S, D>([](var& source, var& dest) {
					//
					var& non_const = const_cast<var&>(source);
					D* result = dynamic_cast<D*>( &non_const.as<S>() );
					dest = data::ref(*result);
				});
			}

		public:
			//static var null;
	};

	// get reference to object
	template<typename T>
	var ref(T& value)
	{
		return var(value, var::is_xref | var::copy_by_ref);
	}

	static var null = var::setup();

}
}

#endif