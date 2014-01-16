#pragma once
#include <map>

namespace Chthon {

template<class T>
struct TypePtr {
	explicit TypePtr(const T * type_pointer = nullptr)
		: pointer(type_pointer) {}
	bool valid() const
	{
		return pointer;
	}
	bool equal(const TypePtr<T> & other) const
	{
		return other == pointer;
	}
	bool equal(const T * other) const
	{
		return other == pointer;
	}
	const T & operator*() const
	{
		if(pointer) {
			return *pointer;
		}
		static T empty;
		return empty;
	}
	const T * operator->() const
	{
		return &(operator*());
	}
private:
	const T * pointer;
};
template<class T>
bool operator==(const TypePtr<T> & typeptr, const T * ptr)
{
	return typeptr.equal(ptr);
}
template<class T>
bool operator==(const TypePtr<T> & typeptr, const TypePtr<T> & other)
{
	return typeptr.equal(other);
}
template<class T> bool operator==(const T * ptr, const TypePtr<T> & typeptr) { return operator==(typeptr, ptr); }
template<class T> bool operator!=(const TypePtr<T> & typeptr, const TypePtr<T> & other) { return !operator==(typeptr, other); }
template<class T> bool operator!=(const TypePtr<T> & typeptr, const T * ptr) { return !operator==(typeptr, ptr); }
template<class T> bool operator!=(const T * ptr, const TypePtr<T> & typeptr) { return !operator==(typeptr, ptr); }

template<class Id, class Value>
struct TypeRegistry {
	typedef typename Value::Type ValueType;
	std::map<Id, ValueType> types;

	bool has(const Id & id) const
	{
		return (types.count(id) > 0);
	}
	const ValueType * get(const Id & id) const
	{
		if(has(id)) {
			return &types.find(id)->second;
		}
		return nullptr;
	}
	const ValueType * insert(const ValueType & type)
	{
		return &(types[type.id] = type);
	}
	typename ValueType::Builder insert(const Id & id)
	{
		types[id].id = id;
		return typename ValueType::Builder(types[id]);
	}
};

}
