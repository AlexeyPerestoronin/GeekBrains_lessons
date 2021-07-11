#include "tests/test-unit-common.hpp"

#include "list.h"

class TestClass {
    public:
    std::string d_str;

    TestClass(const std::string& i_str)
        : d_str(i_str) {
        std::cout << "TestClass::TestClass" << std::endl;
    }

    ~TestClass() {
        std::cout << "TestClass::~TestClass" << std::endl;
    }
};

std::unique_ptr<TestClass>& Create1() {
    return std::make_unique<TestClass>("test string 1");
}

std::unique_ptr<TestClass> Create2() {
    return std::make_unique<TestClass>("test string 2");
    // ЗАМЕЧАНИЕ 1: если функция должна вернуть объект (не ссылку и не указатель), то она всегда возвращает rvalue-ссылку на возвращаемый объект!
    // ЗАМЕЧАНИЕ 2: в данном случае, прежде чем вернуть rvalue ссылку на возвращаемый объект, по идее должен вызваться конструктор копирования, что в случае
    // с std::unique_ptr вызвало бы ошибку, т.к. он помечен как deleted. Однако, ошибки не происходит, по причине, что вызов конструктора копирования является лишним
    // и компиляр пропускает этот шаг, просто превращая возвращаемый объект в rvalue-ссылку - т.е. делая его безымянным и ни к чему не относящимся.
}

// ЗАМЕЧАНИЕ: Create2 абсолютно идентична Create3
std::unique_ptr<TestClass>&& Create3() {
    return std::make_unique<TestClass>("test string 2");
}

static std::ostream& cout = std::cout;

TEST(Test, Step1) {
    // ОШИБКА КОМПИЛЯЦИИ: у std::unique_ptr конструктор копирования помечен как удалённый
    //std::unique_ptr<TestClass> ptr = Create1();
}

TEST(Test, Step2) {
    // ОШИБКА ВЫПОЛНЕНИЯ: т.к. возвращаемый объект разрушится ещё до того, как будет назначен ptr2,
    // поэтому будет произведена попытка обращения к не существующему объекту
    std::unique_ptr<TestClass> ptr = std::move(Create1());
}

TEST(Test, Step3) {
    // ВНИМАНИЕ: так сделать получится, но поведение программы НЕ ОПРЕДЕЛЕНО! Причина: ptr3 - это ссылка на разрушенный объект
    std::unique_ptr<TestClass>& ptr = Create1();
    // ВНИМАНИЕ: т.к. ptr3 является ссылкой на разрушенный объект, то НИКТО НЕ ЗНАЕТ, как поведён себя этот код! Это зависит от многих факторов: тапа данных члена d_str; компилятора; системы и т.д.
    std::cout << ptr->d_str << std::endl;
}

TEST(Test, Step4) {
    // здесь всё хорошо.
    // ЗАМЕЧАНИЕ: по идее, здесь должен быть вызван конструктор копирования, который удалён, а потому должна возникнуть ошибка компиляции,
    // но этого не происходит, т.к. функция на самом деле возвращает rvalue-ссылку, а следовательно - вызвается конструктор перемещения, который разрешён.
    std::unique_ptr<TestClass> ptr;
    ptr = Create2(); // <1>
    std::cout << ptr->d_str << std::endl;
    std::unique_ptr<TestClass> ptr1{ Create2() }; // <2>
    std::unique_ptr<TestClass> ptr2(Create2());   // <3>
    // в данном случае: <1> эквивалентно <2> эквивалентно <3>
}

TEST(Test, Step5) {
    // здесь всё тоже хорошо, только т.к. мы присваиваем возвращемый объект ссылочному типу, то вынуждены сразу же проинициализировать ptr.
    std::unique_ptr<TestClass>& ptr = Create2();
    ptr = Create2();
    std::cout << ptr->d_str << std::endl;
}