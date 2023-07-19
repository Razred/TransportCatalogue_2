#include "json.h"
#include <algorithm>

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadNull(istream& input) {
            const string nameNull = "null";
            for (size_t i = 0; i < nameNull.size(); i++) {
                if (nameNull.at(i) == input.get()) continue;
                else throw ParsingError("Null parsing error");
            }
            return {};
        }

        using Number = variant<int, double>;

        Node LoadNumber(istream& input) {
            using namespace literals;

            string parse_num;

            auto read_char = [&parse_num, &input] {
                parse_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            auto read_digits = [&input, read_char] {
                if (!isdigit(input.peek()))
                    throw ParsingError("A digit is expected");
                while (isdigit(input.peek()))
                    read_char();
            };

            if (input.peek() == '-')
                read_char();

            if (input.peek() == '0')
                read_char();
            else
                read_digits();

            bool is_int = true;
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            if (char ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '-' || ch == '+') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    try {
                        return Node(stoi(parse_num));
                    } catch(...) {

                    }
                }
                return Node(stod(parse_num));
            } catch(...) {
                throw ParsingError("Failed to convert"s + parse_num + " to number"s);
            }
        }

        Node LoadString(istream& input) {
            using namespace literals;

            auto it = istreambuf_iterator<char>(input);
            auto end = istreambuf_iterator<char>();
            string s;
            while(true) {
                if (it == end) {
                    throw ParsingError("String parsing error"s);
                }
                char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end)
                        throw ParsingError("String parsing error"s);
                    const char escaped_char = *(it);
                    switch (escaped_char) {
                        case 'n':
                            s.push_back('\n');
                            break;
                        case 'r':
                            s.push_back('\r');
                            break;
                        case '"':
                            s.push_back('\"');
                            break;
                        case 't':
                            s.push_back('\t');
                            break;
                        case '\\':
                            s.push_back('\\');
                            break;
                        default:
                            throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r')
                    throw ParsingError("Unexpected end of line"s);
                else {
                    s.push_back(ch);
                }
                ++it;
            }
            return Node(move(s));
        }

        Node LoadBool(istream& input) {
            const std::string name_false = "false";
            const std::string name_true = "true";
            char c = input.get();
            bool value = (c == 't');
            std::string const* name = value ? &name_true : &name_false;
            for (size_t i = 1; i < name->size(); ++i) {
                if (name->at(i) == input.get())
                    continue;
                else
                    throw ParsingError("Bool parsing error"s);
            }
            return Node(value);
        }

        Node LoadArray(istream& input) {
            Array result;
            if (input.peek() == -1)
                throw ParsingError("Array parsing error"s);

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }

            return Node(move(result));
        }

        Node LoadDict(istream& input) {
            Dict result;
            if (input.peek() == -1)
                throw ParsingError("Dictionary parsing error"s);

            for (char c; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({move(key), LoadNode(input)});
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            } else if (c == '{') {
                return LoadDict(input);
            } else if (c == '"') {
                return LoadString(input);
            } else if (c =='n') {
                input.putback(c);
                return LoadNull(input);
            } else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            } else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace

    Node::Node(::nullptr_t) {
        value_ = nullptr;
    }

    Node::Node(int value) : value_(value) {
    }

    Node::Node(double value) : value_(value) {
    }

    Node::Node(bool value) : value_(value) {
    }

    Node::Node(std::string value) : value_(value) {
    }

    Node::Node(Array value) : value_(value) {
    }

    Node::Node(Dict value) : value_(value) {
    }

    const Node::Value& Node::GetValue() const {
        return value_;
    }

    bool Node::IsInt() const {
        return holds_alternative<int>(value_);
    }

    bool Node::IsDouble() const {
        return holds_alternative<double>(value_) || IsInt();
    }

    bool Node::IsPureDouble() const {
        return holds_alternative<double>(value_);
    }

    bool Node::IsBool() const {
        return holds_alternative<bool>(value_);
    }

    bool Node::IsString() const {
        return holds_alternative<std::string>(value_);
    }

    bool Node::IsNull() const {
        return holds_alternative<nullptr_t>(value_);
    }

    bool Node::IsArray() const {
        return holds_alternative<Array>(value_);
    }

    bool Node::IsDict() const {
        return holds_alternative<Dict>(value_);
    }

    int Node::AsInt() const {
        if (!IsInt())
            throw std::logic_error("wrong_type");
        return get<int>(value_);
    }

    bool Node::AsBool() const {
        if (!IsBool())
            throw std::logic_error("wrong_type");
        return get<bool>(value_);
    }

    double Node::AsDouble() const {
        if (!IsDouble())
            throw std::logic_error("wrong_type");
        if (IsInt())
            return static_cast<double>(get<int>(value_));
        return get<double>(value_);
    }

    const std::string& Node::AsString() const {
        if (!IsString())
            throw std::logic_error("wrong_type");
        return get<std::string>(value_);
    }

    const Array& Node::AsArray() const {
        if (!IsArray())
            throw std::logic_error("wrong_type");
        return get<Array>(value_);
    }

    const Dict& Node::AsMap() const {
        if (!IsDict())
            throw std::logic_error("wrong_type");
        return get<Dict>(value_);
    }

    bool Node::operator==(const Node& rhs) const {
        return this->value_ == rhs.value_;
    }

    bool Node::operator!=(const Node &rhs) const {
        return (!(this->value_ == rhs.value_));
    }

    Document::Document(Node root)
            : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document &rhs) {
        return this->root_ == rhs.root_;
    }

    bool Document::operator!=(const Document &rhs) {
        return !(this->root_ == rhs.root_);
    }

    Document Load(istream& input) {
        return Document{LoadNode(input)};
    }

    void ValuePrinter::operator()(const std::nullptr_t) {
        out << "null"sv;
    }

    void ValuePrinter::operator()(const int value) {
        out << value;
    }

    void ValuePrinter::operator()(const double value) {
        out << value;
    }

    void ValuePrinter::operator()(const bool value) {
        out << std::boolalpha << value;
    }

    void ValuePrinter::operator()(std::string value) {
        out << "\""sv;
        for (const char& c : value) {
            if (c == '\n') {
                out << "\\n"sv;
                continue;
            }
            if (c == '\t') {
                out << "\t"sv;
                continue;
            }
            if (c == '\r') {
                out << "\\r"sv;
                continue;
            }
            if (c == '\"')
                out << "\\"sv;
            if (c == '\\'){
                out << "\\"sv;
            }
            out << c;
        }
        out << "\""sv;
    }

    void ValuePrinter::operator()(const Array &array) {
        out << "[";
        bool i = 1;
        for (const auto &val : array) {
            if (i)
                i = 0;
            else
                out << ", ";
            std::visit(ValuePrinter{out}, val.GetValue());
        }
        out << "]";
    }

    void ValuePrinter::operator()(const Dict &dict) {
        out << "{";
        bool i = 1;
        for (const auto &val : dict) {
            if (i)
                i = 0;
            else
                out << ", ";
            out << "\"" << val.first << "\": ";
            std::visit(ValuePrinter{out}, val.second.GetValue());
        }
        out << "}";
    }

    void Print(const Document& doc, std::ostream& output) {
        std::visit(ValuePrinter{output}, doc.GetRoot().GetValue());

        // Реализуйте функцию самостоятельно
    }

}