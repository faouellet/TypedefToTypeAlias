DoxygenTemplateGenerator
===================

DoxygenTemplateGenerator (DTG) is a tool that will generate doxygen style comments for specific elements of a program.

For a function:
```cpp
int MyFunc(bool b, int i)
{
	// ...
}
```

the comment generated will look like this:
```cpp
/**
* \fn MyFunc
* \brief	DESCRIPTION
* \param	b	DESCRIPTION
* \param	i	DESCRIPTION
* \return DESCRIPTION
*/
int MyFunc(bool b, int i)
{
	// ...
}
```

For a class:
```cpp
class MyClass
{
	// ...
};
```

the comment generated will look like this:
```cpp
/**
* \class Class
* \brief	DESCRIPTION
*/
class MyClass
{
	// ...
};
```

For an enum:
```cpp
enum MyEnum { /* ... */};
```

the following comment generated will be generated:
```cpp
/**
* \enum MyEnum
* \brief	DESCRIPTION
*/
enum MyEnum { /* ... */};
```

Finally, for an union:
```cpp
enum MyUnion { /* ... */};
```

the following comment generated will be generated:
```cpp
/**
* \union MyUnion
* \brief	DESCRIPTION
*/
enum MyUnion { /* ... */};
```