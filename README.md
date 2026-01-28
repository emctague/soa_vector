# soa_vector

soa_vector is a templated data-type that makes managing a
[structure-of-arrays](https://en.wikipedia.org/wiki/AoS_and_SoA)
simple in C++.

To install, simply copy `soa_vector.h` from this repository.
The `soa_vector` lives in the `eam` namespace.

## Important Caveats

By default, `soa_vector` **does not** respect move semantics or call destructors.
It does not perform any bounds checking. You can alter this behavior by
implementing a custom `alloc_strategy` as defined in the header file, and using
`basic_soa_vector<KeyEnum, AllocStrat, Vs...>`.

`soa_vector` currently has no mechanisms for iteration, reordering,
removal, or (non-appending) insertion. At present, you can treat it like an
append-only buffer.

## Example

Suppose we want to convert an array of this structure to an SoA format:

```c++
struct person {
    const char* name;
    unsigned age;
    const Image* profile_picture;
};
```

We simply define an enum that represents the different fields of our SoA,
and then create an soa_vector, passing in that enum type and the types of each
field as template parameters.

```c++
#include "soa_vector.h"

enum class field { name, age, profile_picture };
eam::soa_vector<field, const char*, unsigned, const Image*> people;
```

To add a new item to the vector, we use `emplace_back`:

```c++
people.emplace_back(
    "John Smith", 42, profile_pic
);
```

`emplace_back` returns the index of the newly inserted entry.

We can then use `.at<field key>(index)` to access and manipulate fields:

```c++
cout << people.at<field::name>(0) " is "
     << people.at<field::age>(0) << " years old."
     << endl;

// Happy birthday!
people.at<field::age>(0)++;
```

Just like `std::vector`, `soa_vector` has `.size()` and `.empty()` methods.
