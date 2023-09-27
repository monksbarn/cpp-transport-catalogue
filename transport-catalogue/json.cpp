#include "json.h"

using namespace std::literals;

//_________________________________________________________________________________________________
namespace json {//__________________________namespace json_________________________________________
    const Node::Value& Node::GetValue() const {
        return value_;
    }
    //=============================================================================================WHAT_IS_THIS
    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    }
    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }
    bool Node::IsDouble() const {
        return (std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_));
    }
    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }
    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }
    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(value_);
    }
    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }



    //=============================================================================================RETURN_THE_SAME_VALUE
    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("error");
        }
        return std::get<Array>(value_);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw std::logic_error("error");
        }
        return std::get<Dict>(value_);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("error");
        }
        return std::get<int>(value_);
    }

    const std::string& Node::AsString() const {
        if (!IsString()) {
            throw std::logic_error("error");
        }
        return std::get<std::string>(value_);
    }

    double Node::AsDouble() const {
        if (!IsDouble()) {
            throw std::logic_error("error");
        }
        return std::holds_alternative<double>(value_) ? std::get<double>(value_) : std::get<int>(value_);
    }

    std::nullptr_t Node::AsNull() const {
        if (!IsNull()) {
            throw std::logic_error("error");
        }
        return std::get<std::nullptr_t>(value_);
    }

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("error");
        }
        return std::get<bool>(value_);
    }



    //=============================================================================================LOAD_ARRAY
    Node LoadArray(std::istream& input) {
        Array result;
        char c;
        while (input >> c && c != ']') {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }
        if (result.empty() && c != ']') {
            throw ParsingError("Empty sequence"s);
        }
        return Node(std::move(result));
    }



    //=============================================================================================LOAD_DICT
    Node LoadDict(std::istream& input) {
        Dict result;
        char c;
        while (input >> c && c != '}') {
            if (c == ',') {
                input >> c;
            }
            while (input.peek() == '\n' || input.peek() == '\r') {
                input >> c;
            }
            std::string key = LoadString(input);
            input >> c;
            result.insert({ key,LoadNode(input) });
        }
        if (result.empty() && c != '}') {
            throw ParsingError("Empty sequence"s);
        }
        return Node(result);
    }



    //=============================================================================================LOAD_NUMBER
    Node LoadNumber(std::istream& input) {
        using namespace std::literals;

        std::string parsed_num;

        // Считывает в parsed_num очередной символ из input
        auto read_char = [&parsed_num, &input] {
            parsed_num += static_cast<char>(input.get());
            if (!input) {
                throw ParsingError("Failed to read number from stream"s);
            }
            };

        // Считывает одну или более цифр в parsed_num из input
        auto read_digits = [&input, read_char] {
            if (!std::isdigit(input.peek())) {
                throw ParsingError("A digit is expected"s);
            }
            while (std::isdigit(input.peek())) {
                read_char();
            }
            };

        if (input.peek() == '-') {
            read_char();
        }
        // Парсим целую часть числа
        if (input.peek() == '0') {
            read_char();
            // После 0 в JSON не могут идти другие цифры
        }
        else {
            read_digits();
        }

        bool is_int = true;
        // Парсим дробную часть числа
        if (input.peek() == '.') {
            read_char();
            read_digits();
            is_int = false;
        }

        // Парсим экспоненциальную часть числа
        if (int ch = input.peek(); ch == 'e' || ch == 'E') {
            read_char();
            if (ch = input.peek(); ch == '+' || ch == '-') {
                read_char();
            }
            read_digits();
            is_int = false;
        }

        try {
            if (is_int) {
                // Сначала пробуем преобразовать строку в int
                try {
                    return Node(std::stoi(parsed_num));
                }
                catch (...) {
                    // В случае неудачи, например, при переполнении,
                    // код ниже попробует преобразовать строку в double
                }
            }
            return Node(std::stod(parsed_num));
        }
        catch (...) {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }



    //=============================================================================================LOAD_STRING

    // Считывает содержимое строкового литерала JSON-документа
    // Функцию следует использовать после считывания открывающего символа ":
    std::string LoadString(std::istream& input) {
        using namespace std::literals;

        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        std::string s;
        while (true) {
            if (it == end) {
                // Поток закончился до того, как встретили закрывающую кавычку?
                throw ParsingError("std::string parsing error");
            }
            const char ch = *it;
            if (ch == '"') {
                // Встретили закрывающую кавычку
                ++it;
                break;
            }
            else if (ch == '\\') {
                // Встретили начало escape-последовательности
                ++it;
                if (it == end) {
                    // Поток завершился сразу после символа обратной косой черты
                    throw ParsingError("std::string parsing error");
                }
                const char escaped_char = *(it);
                // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                switch (escaped_char) {
                case 'n': s.push_back('\n');break;
                case 't': s.push_back('\t');break;
                case 'r': s.push_back('\r');break;
                case '"': s.push_back('"');break;
                case '\\': s.push_back('\\');break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                }
            }
            else if (ch == '\n' || ch == '\r') {
                // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                throw ParsingError("Unexpected end of line"s);
            }
            else {
                // Просто считываем очередной символ и помещаем его в результирующую строку
                s.push_back(ch);
            }
            ++it;
        }
        return s;
    }


    Node LoadKeyWord(std::istream& input) {
        char c;
        std::string key_word;
        Node result;

        while (input >> c && c > 96 && c < 123) {
            key_word += c;
        }
        input.putback(c);
        if (key_word == "null"sv) {
            result = Node{ nullptr };
        }
        else if (key_word == "true"sv) {
            result = Node{ true };
        }
        else if (key_word == "false"sv) {
            result = Node{ false };
        }
        else {
            throw ParsingError("LoadKeyWord() error: the key \"" + key_word + "\" doesn't exists for this context"s);
        }
        return result;
    }


    //=============================================================================================LOAD_NODE
    Node LoadNode(std::istream& input) {
        char c;
        Node node;;
        input >> c;

        if (c == '[') {
            node = std::move(LoadArray(input));
        }
        else if (c == '{') {
            node = std::move(LoadDict(input));
        }
        else if (c == '"' || c == '\t' || c == '\\') {
            node = std::move(LoadString(input));
        }
        else if (c == 'n' || c == 'f' || c == 't') {
            input.putback(c);
            node = std::move(LoadKeyWord(input));
        }
        else {
            input.putback(c);
            node = LoadNumber(input);
        }
        return node;
    }



    //=============================================================================================DOCUMENT
    Document::Document(Node root)
        : root_(std::move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(std::istream& input) {
        return Document{ LoadNode(input) };
    }



    //=============================================================================================PRINT
    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), PrintContext{ output });
        output << std::endl;
    }

    void PrintNode(const json::Node& node, PrintContext context) {
        std::visit(PrintTheSame{ context }, node.GetValue());
    }


    //=============================================================================================PRINT_THE_SAME

    //_____________________________________________________________________________________________for_nullptr
    void PrintTheSame::operator()(const std::nullptr_t) {
        context.out << "null"sv;
    }

    //_____________________________________________________________________________________________for_bool
    void PrintTheSame::operator()(const bool value) {
        context.out << std::boolalpha << value;
    }

    //______________________________________________________________________________________________for_double
    void PrintTheSame::operator()(const double number) {
        context.out << number;
    }

    //______________________________________________________________________________________________for_int
    void PrintTheSame::operator()(const int number) {
        context.out << number;
    }

    //_____________________________________________________________________________________________for_Array
    void PrintTheSame::operator()(const json::Array& array) {
        if (array.size() < 5) {
            context.out << "["sv;
            for (auto it = array.begin(); it != array.end(); ++it) {
                PrintNode(*it, context);
                if (std::next(it) != array.end()) {
                    context.out << ", ";
                }
            }
            context.out << "]";
        }
        else {
            context.out << "[\n"sv;
            for (auto it = array.begin(); it != array.end(); ++it) {
                context.PrintIndent();
                PrintNode(*it, context.Indented());
                if (std::next(it) == array.end()) {
                    context.out << "\n"sv;
                    context.indent -= context.indent_step;
                    context.PrintIndent();
                    context.out << "]"sv;
                }
                else {
                    context.out << ",\n"sv;
                }
            }
        }
    }

    //_____________________________________________________________________________________________for_Map(Dict)
    void PrintTheSame::operator()(const json::Dict& dict) {
        context.out << "{\n";
        for (auto it = dict.begin(); it != dict.end(); ++it) {
            context.PrintIndent();
            context.out << "\""sv << it->first << "\": "sv;
            PrintNode(it->second, context.Indented());
            if (std::next(it) == dict.end()) {
                context.out << "\n"sv;
                context.indent -= context.indent_step;
                context.PrintIndent();
                context.out << "}"sv;
            }
            else {
                context.out << ",\n"sv;
            }
        }
    }

    //_____________________________________________________________________________________________for_string
    void PrintTheSame::operator()(const std::string& str) {
        context.out << "\"";
        for (const char c : str) {
            switch (c)
            {
            case '\"': context.out << "\\" << c; break;
            case '\r': context.out << "\\" << 'r'; break;
            case '\n': context.out << "\\" << 'n'; break;
            case '\\': context.out << "\\" << "\\"; break;
            default: context.out << c;
            }
        }
        context.out << "\"";
    }



    //=============================================================================================MARGINS
    void PrintContext::PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out << " "sv;
        }
    }

    PrintContext PrintContext::Indented() const {
        return { out, indent_step, indent_step + indent };
    }



    //=============================================================================================OPERATORS(== && !=)
    bool operator==(const json::Node& left, const json::Node& right) {
        return left.GetValue() == right.GetValue();
    }

    bool operator!=(const json::Node& left, const json::Node& right) {
        return !(left == right);
    }

    bool operator==(const json::Document& left, const json::Document& right) {
        return left.GetRoot() == right.GetRoot();
    }

    bool operator!=(const json::Document& left, const json::Document& right) {
        return left.GetRoot() != right.GetRoot();
    }

}//_________________________________________________________________________________________________
//______________________________________namespace json______________________________________________