#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

//____________________________________________________________________________________________
namespace json {//__________________________namespace json____________________________________

    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    //Ошибка парсинга json
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };
    //===============================NODE=============================================
    class Node {
    public:
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node(Array array) :value_(array) {}
        Node(Dict map) :value_(map) {}
        Node(int value) :value_(value) {}
        Node(std::string value) :value_(value) {}
        Node(double value) : value_(value) {}
        Node(bool value) : value_(value) {}
        Node(std::nullptr_t value) : value_(value) {}
        Node() = default;

        const Value& GetValue() const;

        bool IsNull() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsString() const;
        bool IsArray() const;
        bool IsMap() const;
        bool IsBool() const;

        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;
        double AsDouble() const;
        std::nullptr_t AsNull() const;
        bool AsBool() const;

    private:
        Value value_;
    };
    //_______________________________________________________________________________________________


    //===============================DOCUMENT========================================================
    class Document {
    public:
        explicit Document(Node root);
        const Node& GetRoot() const;
    private:
        Node root_;
    };
    //_______________________________________________________________________________________________


    //===============================LOADING=========================================================
    Document Load(std::istream& input);
    Node LoadNode(std::istream& input);
    Node LoadArray(std::istream& input);
    Node LoadDict(std::istream& input);
    Node LoadNumber(std::istream& input);
    std::string LoadString(std::istream& input);
    //_______________________________________________________________________________________________


    //===============================OPERATORS(== && !=)=============================================
    bool operator==(const json::Document& left, const json::Document& right);
    bool operator!=(const json::Document& left, const json::Document& right);
    bool operator==(const json::Node& left, const json::Node& right);
    bool operator!=(const json::Node& left, const json::Node& right);
    //_______________________________________________________________________________________________


    //===============================MARGINS=========================================================
    struct PrintContext {
        std::ostream& out;
        int indent_step = 3;
        int indent = 3;

        void PrintIndent() const;
        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const;
    };
    //_______________________________________________________________________________________________


    //===============================PRINT===========================================================
    void Print(const Document& doc, std::ostream& output);

    void PrintNode(const json::Node& node, PrintContext constext);

    struct PrintTheSame {
        PrintContext context;
        void operator()(const std::nullptr_t);
        void operator()(const bool value);
        void operator()(const json::Array& array);
        void operator()(const json::Dict& dict);
        void operator()(const std::string& str);
        void operator()(const double number);
        void operator()(const int number);
    };
    //_______________________________________________________________________________________________

    //_______________________________________________________________________________________________
}   //_________________________________namespace json________________________________________________
