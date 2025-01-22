#ifndef VARIANT_H
#define VARIANT_H
/// this code is inspired in https://github.com/martinmoene/variant-lite
/// the variant-lite is much better I created this just a utility nothing fancy.
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstring>

// Helper to identify types
template <typename T, typename U>
struct is_same {
    static const bool value = false;
};

template <typename T>
struct is_same<T, T> {
    static const bool value = true;
};

struct NoneType{
    ~NoneType(){}
    NoneType(){}
};

// Variant class that supports three types
template <typename T1, typename T2, typename T3/*, typename T4=NoneType, typename T5=NoneType*/>
class Variant {
private:
    union {
        T1 value1;
        T2 value2;
        T3 value3;
      //  T4 value4;
      //  T5 value5;
    };
    enum Type { TYPE_NONE, TYPE_T1, TYPE_T2, TYPE_T3/*, TYPE_T4, TYPE_T5*/ } currentType;

    void destroy() {
        switch (currentType) {
            case TYPE_T1: value1.~T1(); break;
            case TYPE_T2: value2.~T2(); break;
            case TYPE_T3: value3.~T3(); break;
            //case TYPE_T4: value3.~T4(); break;
           // case TYPE_T5: value3.~T5(); break;
            default: break;
        }
        currentType = TYPE_NONE;
    }

public:
    // Default constructor
    Variant() : currentType(TYPE_NONE) {}

    // Constructor for T1
    Variant(const T1& val) : currentType(TYPE_T1) {
        new (&value1) T1(val);
    }

    // Constructor for T2
    Variant(const T2& val) : currentType(TYPE_T2) {
        new (&value2) T2(val);
    }

    // Constructor for T3
    Variant(const T3& val) : currentType(TYPE_T3) {
        new (&value3) T3(val);
    }
    
    /*Variant(const T4& val) : currentType(TYPE_T4) {
        new (&value4) T4(val);
    }
    
    Variant(const T5& val) : currentType(TYPE_T5) {
        new (&value5) T5(val);
    }*/

    // Destructor
    ~Variant() {
        destroy();
    }

    // Copy Constructor
    Variant(const Variant& other) : currentType(TYPE_NONE) {
        switch (other.currentType) {
            case TYPE_T1: new (&value1) T1(other.value1); currentType = TYPE_T1; break;
            case TYPE_T2: new (&value2) T2(other.value2); currentType = TYPE_T2; break;
            case TYPE_T3: new (&value3) T3(other.value3); currentType = TYPE_T3; break;
            ///case TYPE_T4: new (&value4) T3(other.value4); currentType = TYPE_T4; break;
           /// case TYPE_T5: new (&value5) T3(other.value5); currentType = TYPE_T5; break;
            default: break;
        }
    }

    // Assignment Operator
    Variant& operator=(const Variant& other) {
        if (this != &other) {
            destroy();
            switch (other.currentType) {
                case TYPE_T1: new (&value1) T1(other.value1); currentType = TYPE_T1; break;
                case TYPE_T2: new (&value2) T2(other.value2); currentType = TYPE_T2; break;
                case TYPE_T3: new (&value3) T3(other.value3); currentType = TYPE_T3; break;
               /// case TYPE_T4: new (&value4) T3(other.value4); currentType = TYPE_T4; break;
               /// case TYPE_T5: new (&value5) T3(other.value5); currentType = TYPE_T5; break;
                default: break;
            }
        }
        return *this;
    }

    // Getters with type checking
    T1& get_as_t1() {
        if (currentType != TYPE_T1) throw std::runtime_error("Bad Variant Access: Not T1");
        return value1;
    }

    T2& get_as_t2() {
        if (currentType != TYPE_T2) throw std::runtime_error("Bad Variant Access: Not T2");
        return value2;
    }

    T3& get_as_t3() {
        if (currentType != TYPE_T3) throw std::runtime_error("Bad Variant Access: Not T3");
        return value3;
    }
    // Getters with type checking
    template <typename T>
    T& get() {
        if (is<T>()) {
            if (is_same<T, T1>::value) return value1;
            if (is_same<T, T2>::value) return value2;
            if (is_same<T, T3>::value) return value3;
           /// if (is_same<T, T4>::value) return value4;
          ///  if (is_same<T, T5>::value) return value5;
        }
        throw std::runtime_error("Bad Variant Access: Type mismatch");
    }

    // Template type querying
    template <typename T>
    bool is() const {
        if (is_same<T, T1>::value) return currentType == TYPE_T1;
        if (is_same<T, T2>::value) return currentType == TYPE_T2;
        if (is_same<T, T3>::value) return currentType == TYPE_T3;
        ///if (is_same<T, T4>::value) return currentType == TYPE_T4;
        ///if (is_same<T, T5>::value) return currentType == TYPE_T5;
        return false;
    }
    // Type querying
    bool is_t1() const { return currentType == TYPE_T1; }
    bool is_t2() const { return currentType == TYPE_T2; }
    bool is_t3() const { return currentType == TYPE_T3; }
   // bool is_t4() const { return currentType == TYPE_T4; }
   // bool is_t5() const { return currentType == TYPE_T5; }
};

#endif // VARIANT_H
