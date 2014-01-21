#pragma once
#include <map>

namespace Chthon { /// @defgroup Types Type system
/// @{

/** Stores pointer to type object.
 * If stored pointer is invalid (i.e. null) then default-contstructed value is returned.
 */
template<class T>
struct TypePtr {
	/// Constructs class using given type_pointer (null by default).
	explicit TypePtr(const T * type_pointer = nullptr)
		: pointer(type_pointer) {}
	/// Returns true if stored pointer is valid (i.e. not null), otherwise returns false.
	bool valid() const
	{
		return pointer;
	}
	/// Compares stored pointer with other.
	bool equal(const TypePtr<T> & other) const
	{
		return other == pointer;
	}
	/// Compares stored pointer with other.
	bool equal(const T * other) const
	{
		return other == pointer;
	}
	/** Returns stored value.
	 * If stored pointer is invalid (i.e. null) then default-contstructed value is returned.
	 */
	const T & operator*() const
	{
		if(pointer) {
			return *pointer;
		}
		static T empty;
		return empty;
	}
	/** Returns stored value.
	 * If stored pointer is invalid (i.e. null) then default-contstructed value is returned.
	 */
	const T * operator->() const
	{
		return &(operator*());
	}
private:
	const T * pointer;
};
/// Compares two type object pointers.
template<class T>
bool operator==(const TypePtr<T> & typeptr, const T * ptr)
{
	return typeptr.equal(ptr);
}
/// Compares two type object pointers.
template<class T>
bool operator==(const TypePtr<T> & typeptr, const TypePtr<T> & other)
{
	return typeptr.equal(other);
}
/// Compares two type object pointers.
template<class T> bool operator==(const T * ptr, const TypePtr<T> & typeptr) { return operator==(typeptr, ptr); }
/// Compares two type object pointers.
template<class T> bool operator!=(const TypePtr<T> & typeptr, const TypePtr<T> & other) { return !operator==(typeptr, other); }
/// Compares two type object pointers.
template<class T> bool operator!=(const TypePtr<T> & typeptr, const T * ptr) { return !operator==(typeptr, ptr); }
/// Compares two type object pointers.
template<class T> bool operator!=(const T * ptr, const TypePtr<T> & typeptr) { return !operator==(typeptr, ptr); }

/** Stores type objects by their Id's.
 * Class is instantiated using basic object type Value.
 * It's inner class (or typedef) Value::Type is considered as type object.
 * Such object must have an `id` member, which is of type Id.
 * Also it should have inner class or typedef called Builder, which has constructor receiving type object.
 * Without inner Builder one of the insert() methods would not work.
 */
template<class Id, class Value>
struct TypeRegistry {
	typedef typename Value::Type ValueType;
	std::map<Id, ValueType> types;

	/// Returns true if type object with id exists, otherwise returns false.
	bool has(const Id & id) const
	{
		return (types.count(id) > 0);
	}
	/// Returns pointer to a type object by its id, or null pointer if no object exists with such id.
	const ValueType * get(const Id & id) const
	{
		if(has(id)) {
			return &types.find(id)->second;
		}
		return nullptr;
	}
	/// Inserts type object in registry. Type object is stored under its id.
	const ValueType * insert(const ValueType & type)
	{
		return &(types[type.id] = type);
	}
	/// Creates empty type object and returns its Builder instance.
	typename ValueType::Builder insert(const Id & id)
	{
		types[id].id = id;
		return typename ValueType::Builder(types[id]);
	}
};

/// @}
}
