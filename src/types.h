#pragma once
#include <map>

namespace Chthon { /// @defgroup Types Type system
/// @{

/** Returns value of dereferenced pointer.
 * If pointer is null) then default-contstructed value is returned.
 */
template<class T>
const T & deref_default(const T * pointer)
{
	if(pointer) {
		return *pointer;
	}
	static T empty;
	return empty;
}

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
