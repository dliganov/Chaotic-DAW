#ifndef _AWFUL_OBJECTS_
#define _AWFUL_OBJECTS_

class Object
{
public:
    Object() {};
    virtual ~Object() {};
    Object* owner;
};

#endif
