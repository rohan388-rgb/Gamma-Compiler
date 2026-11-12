
#pragma once


#include <string>
#include <vector>
#include <optional>
#include <cassert>
#include <cctype>
#include <iostream>
#include <utility>
#include <cstdlib>

using namespace std;

enum class TokenType {
    // Program structure
    start_,
    end_,

    // Variable declaration and assignment
    let_,
    const_,
    static_,
    var_,
    set_,
    to_,

    //data type
    number_,
    decimal_,
    text_,
    boolean_,
    character_,

    //literals
    int_lit,
    float_lit,
    string_lit,
    char_lit,
    true_,
    false_,
    null_,

    // Identifier
    ident,

    // English operators
    plus_,
    minus_,
    times_,
    divided_by_,
    remainder_,

    // Comparison words
    greater_,
    less_,
    equal_,
    than_,

    // Operators
    gt,
    lt,
    gte,
    lte,
    eqeq,
    neq,

    // Logical operators
    and_,
    or_,
    not_,

    //bitwise operator
    bit_and_,
    bit_or_,
    bit_xor_,
    bit_not_,
    shl_,
    shr_,

    //assignment operatoor
    assign_,
    plus_assign_,
    minus_assign_,
    multiply_assign_,
    divide_assign_,
    mod_assign_,

    // Input / output
    show_,
    ask_,
    store_,
    in_,
    read_,
    print_,

    // Conditions
    if_,
    else_if_,
    otherwise_,
    is_,

    // loops
    while_,
    from_,
    do_,
    for_,
    stop_,
    next_,
    continue_,
    exit_,

    //fucntions
    function_,
    define_,
    with_,
    call_,
    give_,
    return_,
    parameter_,
    argument_,

    //arrays
    array_,
    length_,
    index_,
    sizeof_,
    typeof_,

    // Brackets
    open_paren,
    close_paren,
    open_curly,
    close_curly,
    open_square,
    close_square,

    // Punctuation
    dot,
    comma,
    colon,
    question,

    // String operations
    substring_,
    contains_,
    starts_with_,
    ends_with_,
    trim_,
    uppercase_,
    lowercase_,
    replace_,
    split_,
    join_,

    //pointer
    pointer_,
    address_of_,
    dereference_,

    //oops
    class_,
    struct_,
    object_,
    new_,
    delete_,
    this_,
    allocate_,
    free_,

    //access modifier
    public_,
    private_,
    protected_,

    //constructor
    constructor_,
    extends_,
    base_,
    super_,

    //namespace
    namespace_,
    using_,
    import_,
    include_,

    //enum operation
    enum_,

    //switch
    choose_,
    option_,
    other_,

    //error handling
    try_,
    catch_,
    finally_,
    throw_,

    //special symbol
    semicolon,
    double_colon,
    arrow,

    create_,
};

inline string to_string(const TokenType type)
{
    switch (type) {

        // Program structure
        case TokenType::start_:
            return "`Start`";

        case TokenType::end_:
            return "`End`";


        // Variable declaration and assignment
        case TokenType::let_:
            return "`Let`";

        case TokenType::const_:
            return "`const`";

        case TokenType::static_:
            return "`static`";

        case TokenType::var_:
            return "`var`";

        case TokenType::set_:
            return "`Set`";

        case TokenType::to_:
            return "`to`";


        // Data types
        case TokenType::number_:
            return "`number`";

        case TokenType::decimal_:
            return "`decimal`";

        case TokenType::text_:
            return "`text`";

        case TokenType::boolean_:
            return "`boolean`";

        case TokenType::character_:
            return "`character`";


        // Literals
        case TokenType::int_lit:
            return "int literal";

        case TokenType::float_lit:
            return "float literal";

        case TokenType::string_lit:
            return "string literal";

        case TokenType::char_lit:
            return "char literal";

        case TokenType::true_:
            return "`true`";

        case TokenType::false_:
            return "`false`";

        case TokenType::null_:
            return "`null`";


        // Identifier
        case TokenType::ident:
            return "identifier";


        // English arithmetic operators
        case TokenType::plus_:
            return "`plus`";

        case TokenType::minus_:
            return "`minus`";

        case TokenType::times_:
            return "`times`";

        case TokenType::divided_by_:
            return "`divided by`";

        case TokenType::remainder_:
            return "`remainder`";


        // Comparison words
        case TokenType::greater_:
            return "`greater`";

        case TokenType::less_:
            return "`less`";

        case TokenType::equal_:
            return "`equal`";

        case TokenType::than_:
            return "`than`";


        // Comparison operators
        case TokenType::gt:
            return "`>`";

        case TokenType::lt:
            return "`<`";

        case TokenType::gte:
            return "`>=`";

        case TokenType::lte:
            return "`<=`";

        case TokenType::eqeq:
            return "`==`";

        case TokenType::neq:
            return "`!=`";


        // Logical operators
        case TokenType::and_:
            return "`and`";

        case TokenType::or_:
            return "`or`";

        case TokenType::not_:
            return "`not`";


        // Bitwise operators
        case TokenType::bit_and_:
            return "`bit_and`";

        case TokenType::bit_or_:
            return "`bit_or`";

        case TokenType::bit_xor_:
            return "`bit_xor`";

        case TokenType::bit_not_:
            return "`bit_not`";

        case TokenType::shl_:
            return "`<<`";

        case TokenType::shr_:
            return "`>>`";


        // Assignment operators
        case TokenType::assign_:
            return "`=`";

        case TokenType::plus_assign_:
            return "`+=`";

        case TokenType::minus_assign_:
            return "`-=`";

        case TokenType::multiply_assign_:
            return "`*=`";

        case TokenType::divide_assign_:
            return "`/=`";

        case TokenType::mod_assign_:
            return "`%=`";


        // Input / Output
        case TokenType::show_:
            return "`Show`";

        case TokenType::ask_:
            return "`Ask`";

        case TokenType::store_:
            return "`store`";

        case TokenType::in_:
            return "`in`";

        case TokenType::read_:
            return "`read`";

        case TokenType::print_:
            return "`print`";


        // Conditions
        case TokenType::if_:
            return "`If`";

        case TokenType::else_if_:
            return "`Else if`";

        case TokenType::otherwise_:
            return "`Otherwise`";

        case TokenType::is_:
            return "`is`";


        // Loops
        case TokenType::while_:
            return "`while`";

        case TokenType::from_:
            return "`from`";

        case TokenType::do_:
            return "`Do`";

        case TokenType::for_:
            return "`For`";

        case TokenType::stop_:
            return "`stop`";

        case TokenType::exit_:
            return "`exit`";

        case TokenType::next_:
            return "`next`";

        case TokenType::continue_:
            return "`continue`";


        // Functions
        case TokenType::function_:
            return "`function`";

        case TokenType::define_:
            return "`Define`";

        case TokenType::with_:
            return "`with`";

        case TokenType::call_:
            return "`Call`";

        case TokenType::give_:
            return "`Give`";

        case TokenType::return_:
            return "`return`";

        case TokenType::parameter_:
            return "`parameter`";

        case TokenType::argument_:
            return "`argument`";


        // Arrays
        case TokenType::array_:
            return "`array`";

        case TokenType::length_:
            return "`length`";

        case TokenType::index_:
            return "`index`";

        case TokenType::sizeof_:
            return "`sizeof`";

        case TokenType::typeof_:
            return "`typeof`";


        // Brackets
        case TokenType::open_paren:
            return "`(`";

        case TokenType::close_paren:
            return "`)`";

        case TokenType::open_curly:
            return "`{`";

        case TokenType::close_curly:
            return "`}`";

        case TokenType::open_square:
            return "`[`";

        case TokenType::close_square:
            return "`]`";


        // Punctuation
        case TokenType::dot:
            return "`.`";

        case TokenType::comma:
            return "`,`";

        case TokenType::colon:
            return "`:`";

        case TokenType::question:
            return "`?`";


        // String operations
        case TokenType::substring_:
            return "`substring`";

        case TokenType::contains_:
            return "`contains`";

        case TokenType::starts_with_:
            return "`starts_with`";

        case TokenType::ends_with_:
            return "`ends_with`";

        case TokenType::trim_:
            return "`trim`";

        case TokenType::uppercase_:
            return "`uppercase`";

        case TokenType::lowercase_:
            return "`lowercase`";

        case TokenType::replace_:
            return "`replace`";

        case TokenType::split_:
            return "`split`";

        case TokenType::join_:
            return "`join`";


        // Pointer
        case TokenType::pointer_:
            return "`pointer`";

        case TokenType::address_of_:
            return "`address_of`";

        case TokenType::dereference_:
            return "`dereference`";


        // OOP
        case TokenType::class_:
            return "`class`";

        case TokenType::struct_:
            return "`struct`";

        case TokenType::object_:
            return "`object`";

        case TokenType::new_:
            return "`new`";

        case TokenType::delete_:
            return "`delete`";

        case TokenType::this_:
            return "`this`";

        case TokenType::allocate_:
            return "`allocate`";

        case TokenType::free_:
            return "`free`";


        // Access modifiers
        case TokenType::public_:
            return "`public`";

        case TokenType::private_:
            return "`private`";

        case TokenType::protected_:
            return "`protected`";


        // Constructor / inheritance
        case TokenType::constructor_:
            return "`constructor`";

        case TokenType::extends_:
            return "`extends`";

        case TokenType::base_:
            return "`base`";

        case TokenType::super_:
            return "`super`";


        // Namespace
        case TokenType::namespace_:
            return "`namespace`";

        case TokenType::using_:
            return "`using`";

        case TokenType::import_:
            return "`import`";

        case TokenType::include_:
            return "`include`";


        // Enum
        case TokenType::enum_:
            return "`enum`";


        // Switch
        case TokenType::choose_:
            return "`choose`";

        case TokenType::option_:
            return "`option`";

        case TokenType::other_:
            return "`other`";


        // Error handling
        case TokenType::try_:
            return "`try`";

        case TokenType::catch_:
            return "`catch`";

        case TokenType::finally_:
            return "`finally`";

        case TokenType::throw_:
            return "`throw`";


        // Special symbols
        case TokenType::semicolon:
            return "`;`";

        case TokenType::double_colon:
            return "`::`";

        case TokenType::arrow:
            return "`->`";


        // Creation
        case TokenType::create_:
            return "`Create`";
    }

    assert(false);
    return "";
}


inline optional<int> bin_prec(const TokenType type)
{
    switch (type) {

        // Logical OR
        case TokenType::or_:
            return 0;

        // Logical AND
        case TokenType::and_:
            return 1;

        // Equality
        case TokenType::eqeq:
        case TokenType::neq:
            return 2;

        // Comparison
        case TokenType::gt:
        case TokenType::lt:
        case TokenType::gte:
        case TokenType::lte:
            return 3;

        //shift
        case TokenType::shl_:
        case TokenType::shr_:
            return 4;

        // English / symbolic addition and subtraction
        case TokenType::plus_:
        case TokenType::minus_:
            return 5;

        // English / symbolic multiplication and division
        case TokenType::times_:
        case TokenType::divided_by_:
        case TokenType::remainder_:
            return 6;

        default:
            return {};
    }
}

struct Token {
    TokenType type;
    int line;
    optional<string> value {};
};

class Tokenizer {
public:
    explicit Tokenizer(string src)
        : m_src(move(src))
    {
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;
        string buf;
        int line_count = 1;
        while (peek().has_value()) {
            if (isalpha(peek().value()) || peek().value() == '_')  {
                buf.push_back(consume());
                while (peek().has_value() &&(isalnum(peek().value()) || peek().value() == '_')) {
                    buf.push_back(consume());
                }
                if (buf == "Start") {
                    tokens.push_back({ TokenType::start_, line_count });
                    buf.clear();
                }
                else if (buf == "Let") {
                    tokens.push_back({ TokenType::let_, line_count });
                    buf.clear();
                }
                else if (buf == "If") {
                    tokens.push_back({ TokenType::if_, line_count });
                    buf.clear();
                }

                else if (buf == "Else") {
                    if (peek().has_value() && isspace(peek().value())) {
                        size_t saved_index = m_index;

                        while (peek().has_value() && isspace(peek().value())) {
                            consume();
                        }

                        string next_word;

                        while (peek().has_value() &&
                               (isalpha(peek().value()) || peek().value() == '_')) {
                            next_word.push_back(consume());
                               }

                        if (next_word == "if") {
                            tokens.push_back({ TokenType::else_if_, line_count });
                            buf.clear();
                        }
                        else {
                            m_index = saved_index;
                            tokens.push_back({ TokenType::ident, line_count, buf });
                            buf.clear();
                        }
                    }
                    else {
                        tokens.push_back({ TokenType::ident, line_count, buf });
                        buf.clear();
                    }
                }

                else if (buf == "Otherwise") {
                    tokens.push_back({ TokenType::otherwise_, line_count });
                    buf.clear();
                }
                else if (buf == "While") {
                    tokens.push_back({TokenType::while_, line_count});
                    buf.clear();
                }
                else if (buf == "For") {
                    tokens.push_back({TokenType::for_, line_count});
                    buf.clear();
                }
                else if (buf == "from") {
                    tokens.push_back({TokenType::from_, line_count});
                    buf.clear();
                }
                else if (buf == "stop" || buf == "Stop") {
                    tokens.push_back({TokenType::stop_, line_count});
                    buf.clear();
                }

                else if (buf == "exit" || buf == "Exit")
                {
                    tokens.push_back({TokenType::exit_, line_count});
                    buf.clear();
                }

                else if (buf == "next") {
                    tokens.push_back({TokenType::next_, line_count});
                    buf.clear();
                }
                else if (buf == "Ask") {
                    tokens.push_back({TokenType::ask_, line_count});
                    buf.clear();
                }
                else if (buf == "Give") {
                    tokens.push_back({TokenType::give_, line_count});
                    buf.clear();
                }
                else if (buf == "true") {
                    tokens.push_back({TokenType::true_, line_count});
                    buf.clear();
                }
                else if (buf == "false") {
                    tokens.push_back({TokenType::false_, line_count});
                    buf.clear();
                }
                else if (buf == "Create") {
                    tokens.push_back({TokenType::create_, line_count});
                    buf.clear();
                }
                else if (buf == "Define") {
                    tokens.push_back({TokenType::define_, line_count});
                    buf.clear();
                }
                else if (buf == "with") {
                    tokens.push_back({TokenType::with_, line_count});
                    buf.clear();
                }
                else if (buf == "Call") {
                    tokens.push_back({TokenType::call_, line_count});
                    buf.clear();
                }
                else if (buf == "Show") {
                    tokens.push_back({TokenType::show_, line_count});
                    buf.clear();
                }
                else if (buf == "choose" || buf == "Choose") {
                    tokens.push_back({TokenType::choose_, line_count});
                    buf.clear();
                }
                else if (buf == "option" || buf == "Option")
                {
                    tokens.push_back({TokenType::option_, line_count});
                    buf.clear();
                }
                else if (buf == "other" || buf == "Other")
                {
                    tokens.push_back({TokenType::other_, line_count});
                    buf.clear();
                }
                else if (buf == "Do") {
                    tokens.push_back({TokenType::do_, line_count});
                    buf.clear();
                }
                else if (buf == "const") {
                    tokens.push_back({TokenType::const_, line_count});
                    buf.clear();
                }
                else if (buf == "static") {
                    tokens.push_back({TokenType::static_, line_count});
                    buf.clear();
                }
                else if (buf == "class") {
                    tokens.push_back({TokenType::class_, line_count});
                    buf.clear();
                }
                else if (buf == "struct" || buf == "Struct") {
                    tokens.push_back({TokenType::struct_, line_count});
                    buf.clear();
                }
                else if (buf == "public") {
                    tokens.push_back({TokenType::public_, line_count});
                    buf.clear();
                }
                else if (buf == "private") {
                    tokens.push_back({TokenType::private_, line_count});
                    buf.clear();
                }
                else if (buf == "namespace") {
                    tokens.push_back({TokenType::namespace_, line_count});
                    buf.clear();
                }
                else if (buf == "using") {
                    tokens.push_back({TokenType::using_, line_count});
                    buf.clear();
                }
                else if (buf == "enum" || buf == "Enum")
                {
                    tokens.push_back({TokenType::enum_, line_count});
                    buf.clear();
                }

                else if (buf == "End") {
                    tokens.push_back({ TokenType::end_, line_count });
                    buf.clear();
                }
                else if (buf == "Set") {
                    tokens.push_back({ TokenType::set_, line_count });
                    buf.clear();
                }
                else if (buf == "to") {
                    tokens.push_back({ TokenType::to_, line_count });
                    buf.clear();
                }
                else if (buf == "is") {
                    tokens.push_back({ TokenType::is_, line_count });
                    buf.clear();
                }
                else if (buf == "function") {
                    tokens.push_back({ TokenType::function_, line_count });
                    buf.clear();
                }
                else if (buf == "greater") {
                    tokens.push_back({ TokenType::greater_, line_count });
                    buf.clear();
                }
                else if (buf == "less") {
                    tokens.push_back({ TokenType::less_, line_count });
                    buf.clear();
                }
                else if (buf == "equal") {
                    tokens.push_back({ TokenType::equal_, line_count });
                    buf.clear();
                }
                else if (buf == "than") {
                    tokens.push_back({ TokenType::than_, line_count });
                    buf.clear();
                }
                else if (buf == "plus") {
                    tokens.push_back({ TokenType::plus_, line_count });
                    buf.clear();
                }
                else if (buf == "minus") {
                    tokens.push_back({ TokenType::minus_, line_count });
                    buf.clear();
                }
                else if (buf == "times") {
                    tokens.push_back({ TokenType::times_, line_count });
                    buf.clear();
                }
                else if (buf == "plus_assign") {
                    tokens.push_back({ TokenType::plus_assign_, line_count });
                    buf.clear();
                }
                else if (buf == "minus_assign") {
                    tokens.push_back({ TokenType::minus_assign_, line_count });
                    buf.clear();
                }
                else if (buf == "multiply_assign") {
                    tokens.push_back({ TokenType::multiply_assign_, line_count });
                    buf.clear();
                }
                else if (buf == "divide_assign") {
                    tokens.push_back({ TokenType::divide_assign_, line_count });
                    buf.clear();
                }
                else if (buf == "mod_assign") {
                    tokens.push_back({ TokenType::mod_assign_, line_count });
                    buf.clear();
                }

                else if (buf == "divided") {
                    if (peek().has_value() && isspace(peek().value())) {
                        size_t saved_index = m_index;

                        while (peek().has_value() && isspace(peek().value())) {
                            consume();
                        }

                        string next_word;

                        while (peek().has_value() &&
                               (isalpha(peek().value()) || peek().value() == '_')) {
                            next_word.push_back(consume());
                               }

                        if (next_word == "by") {
                            tokens.push_back({ TokenType::divided_by_, line_count });
                            buf.clear();
                        }
                        else {
                            m_index = saved_index;
                            tokens.push_back({ TokenType::ident, line_count, buf });
                            buf.clear();
                        }
                    }
                    else {
                        tokens.push_back({ TokenType::ident, line_count, buf });
                        buf.clear();
                    }
                }

                else if (buf == "remainder") {
                    tokens.push_back({ TokenType::remainder_, line_count });
                    buf.clear();
                }
                else if (buf == "number") {
                    tokens.push_back({ TokenType::number_, line_count });
                    buf.clear();
                }
                else if (buf == "decimal") {
                    tokens.push_back({ TokenType::decimal_, line_count });
                    buf.clear();
                }
                else if (buf == "text") {
                    tokens.push_back({ TokenType::text_, line_count });
                    buf.clear();
                }
                else if (buf == "boolean") {
                    tokens.push_back({ TokenType::boolean_, line_count });
                    buf.clear();
                }
                else if (buf == "store") {
                    tokens.push_back({ TokenType::store_, line_count });
                    buf.clear();
                }
                else if (buf == "in") {
                    tokens.push_back({ TokenType::in_, line_count });
                    buf.clear();
                }

                else if (buf == "read") {
                    tokens.push_back({TokenType::read_, line_count});
                    buf.clear();
                }
                else if (buf == "print") {
                    tokens.push_back({TokenType::print_, line_count});
                    buf.clear();
                }

                else if (buf == "and") {
                    tokens.push_back({ TokenType::and_, line_count });
                    buf.clear();
                }
                else if (buf == "or") {
                    tokens.push_back({ TokenType::or_, line_count });
                    buf.clear();
                }
                else if (buf == "not") {
                    tokens.push_back({ TokenType::not_, line_count });
                    buf.clear();
                }

                else if (buf == "length") {
                    tokens.push_back({TokenType::length_, line_count});
                    buf.clear();
                }
                else if (buf == "substring") {
                    tokens.push_back({TokenType::substring_, line_count});
                    buf.clear();
                }
                else if (buf == "contains") {
                    tokens.push_back({TokenType::contains_, line_count});
                    buf.clear();
                }
                else if (buf == "starts_with") {
                    tokens.push_back({TokenType::starts_with_, line_count});
                    buf.clear();
                }
                else if (buf == "ends_with") {
                    tokens.push_back({TokenType::ends_with_, line_count});
                    buf.clear();
                }
                else if (buf == "trim") {
                    tokens.push_back({TokenType::trim_, line_count});
                    buf.clear();
                }
                else if (buf == "uppercase") {
                    tokens.push_back({TokenType::uppercase_, line_count});
                    buf.clear();
                }
                else if (buf == "lowercase") {
                    tokens.push_back({TokenType::lowercase_, line_count});
                    buf.clear();
                }

                // Missing data type
                else if (buf == "character") {
                    tokens.push_back({TokenType::character_, line_count});
                    buf.clear();
                }

                // Missing literal
                else if (buf == "null") {
                    tokens.push_back({TokenType::null_, line_count});
                    buf.clear();
                }

                // Missing loop keyword
                else if (buf == "continue") {
                    tokens.push_back({TokenType::continue_, line_count});
                    buf.clear();
                }

                // Missing function keywords
                else if (buf == "return") {
                    tokens.push_back({TokenType::return_, line_count});
                    buf.clear();
                }
                else if (buf == "parameter") {
                    tokens.push_back({TokenType::parameter_, line_count});
                    buf.clear();
                }
                else if (buf == "argument") {
                    tokens.push_back({TokenType::argument_, line_count});
                    buf.clear();
                }

                // Array keywords
                else if (buf == "array") {
                    tokens.push_back({TokenType::array_, line_count});
                    buf.clear();
                }
                else if (buf == "index") {
                    tokens.push_back({TokenType::index_, line_count});
                    buf.clear();
                }
                else if (buf == "sizeof") {
                    tokens.push_back({TokenType::sizeof_, line_count});
                    buf.clear();
                }
                else if (buf == "typeof") {
                    tokens.push_back({TokenType::typeof_, line_count});
                    buf.clear();
                }

                // String operations
                else if (buf == "replace") {
                    tokens.push_back({TokenType::replace_, line_count});
                    buf.clear();
                }
                else if (buf == "split") {
                    tokens.push_back({TokenType::split_, line_count});
                    buf.clear();
                }
                else if (buf == "join") {
                    tokens.push_back({TokenType::join_, line_count});
                    buf.clear();
                }

                // Pointer keywords
                else if (buf == "pointer") {
                    tokens.push_back({TokenType::pointer_, line_count});
                    buf.clear();
                }
                else if (buf == "address_of") {
                    tokens.push_back({TokenType::address_of_, line_count});
                    buf.clear();
                }
                else if (buf == "dereference") {
                    tokens.push_back({TokenType::dereference_, line_count});
                    buf.clear();
                }

                // OOP keywords
                else if (buf == "object") {
                    tokens.push_back({TokenType::object_, line_count});
                    buf.clear();
                }
                else if (buf == "new") {
                    tokens.push_back({TokenType::new_, line_count});
                    buf.clear();
                }
                else if (buf == "delete") {
                    tokens.push_back({TokenType::delete_, line_count});
                    buf.clear();
                }
                else if (buf == "this") {
                    tokens.push_back({TokenType::this_, line_count});
                    buf.clear();
                }
                else if (buf == "allocate") {
                    tokens.push_back({TokenType::allocate_, line_count});
                    buf.clear();
                }
                else if (buf == "free") {
                    tokens.push_back({TokenType::free_, line_count});
                    buf.clear();
                }

                // Access modifier
                else if (buf == "protected") {
                    tokens.push_back({TokenType::protected_, line_count});
                    buf.clear();
                }

                // Constructor / inheritance
                else if (buf == "constructor") {
                    tokens.push_back({TokenType::constructor_, line_count});
                    buf.clear();
                }
                else if (buf == "extends") {
                    tokens.push_back({TokenType::extends_, line_count});
                    buf.clear();
                }
                else if (buf == "base") {
                    tokens.push_back({TokenType::base_, line_count});
                    buf.clear();
                }
                else if (buf == "super") {
                    tokens.push_back({TokenType::super_, line_count});
                    buf.clear();
                }

                // Namespace
                else if (buf == "import") {
                    tokens.push_back({TokenType::import_, line_count});
                    buf.clear();
                }
                else if (buf == "include") {
                    tokens.push_back({TokenType::include_, line_count});
                    buf.clear();
                }

                // Error handling
                else if (buf == "try") {
                    tokens.push_back({TokenType::try_, line_count});
                    buf.clear();
                }
                else if (buf == "catch") {
                    tokens.push_back({TokenType::catch_, line_count});
                    buf.clear();
                }
                else if (buf == "finally") {
                    tokens.push_back({TokenType::finally_, line_count});
                    buf.clear();
                }
                else if (buf == "throw") {
                    tokens.push_back({TokenType::throw_, line_count});
                    buf.clear();
                }
                else {
                    tokens.push_back({ TokenType::ident, line_count, buf });
                    buf.clear();
                }
            }
            else if (isdigit(peek().value())) {
                bool has_dot = false;

                buf.push_back(consume());

                while (peek().has_value()) {

                    if (isdigit(peek().value())) {
                        buf.push_back(consume());
                    }
                    else if (peek().value() == '.'&& !has_dot && peek(1).has_value() && isdigit(peek(1).value()))
                    {
                        has_dot = true;
                        buf.push_back(consume());
                    }
                    else {
                        break;
                    }
                }

                if (has_dot)
                    tokens.push_back({ TokenType::float_lit, line_count, buf });
                else
                    tokens.push_back({ TokenType::int_lit, line_count, buf });

                buf.clear();
            }
            else if (peek().value() == '"') {
                consume();

                while (peek().has_value() && peek().value() != '"')
                    buf.push_back(consume());

                if (!peek().has_value()) {
                    cerr << "Unterminated string on line "
                         << line_count << endl;
                    exit(EXIT_FAILURE);
                }

                consume();

                tokens.push_back({TokenType::string_lit, line_count, buf});
                buf.clear();
            }
            else if (peek().value() == '\'') {
                consume();

                if (!peek().has_value()) {
                    cerr << "Invalid character literal on line "
                         << line_count << endl;
                    exit(EXIT_FAILURE);
                }

                buf.push_back(consume());

                if (!peek().has_value() || peek().value() != '\'') {
                    cerr << "Invalid character literal on line "
                         << line_count << endl;
                    exit(EXIT_FAILURE);
                }

                consume();

                tokens.push_back({ TokenType::char_lit, line_count, buf });
                buf.clear();
            }
            else if (peek().value() == '/' &&peek(1).has_value() &&peek(1).value() == '/')
            {
                consume();
                consume();

                while (peek().has_value() && peek().value() != '\n')
                {
                    consume();
                }
            }
            else if (peek().value() == '/' &&peek(1).has_value() &&peek(1).value() == '*')
            {
                consume();
                consume();
                bool closed = false;
                while (peek().has_value()){
                    if (peek().value() == '*' &&peek(1).has_value() &&peek(1).value() == '/')
                    {
                        consume();
                        consume();
                        closed = true;
                        break;
                    }
                    if (peek().value() == '\n') {
                        line_count++;
                    }
                    consume();
                }
                if (!closed){
                    cerr << "Unterminated comment on line "
                         << line_count << endl;
                    exit(EXIT_FAILURE);
                }
            }

            else if (peek().value() == '(') {
                consume();
                tokens.push_back({ TokenType::open_paren, line_count });
            }
            else if (peek().value() == ')') {
                consume();
                tokens.push_back({ TokenType::close_paren, line_count });
            }

            else if (peek().value() == '=') {
                consume();

                if (peek().has_value() && peek().value() == '=') {
                    consume();
                    tokens.push_back({TokenType::eqeq, line_count});
                }
                else {
                    cerr << "Single '=' is not allowed. Use 'Set ... to ...' on line "
                         << line_count << endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if (peek().value() == '>') {
                consume();

                if (peek().has_value() && peek().value() == '>') {
                    consume();
                    tokens.push_back({ TokenType::shr_, line_count });
                }
                else if (peek().has_value() && peek().value() == '=') {
                    consume();
                    tokens.push_back({ TokenType::gte, line_count });
                }
                else {
                    tokens.push_back({ TokenType::gt, line_count });
                }
            }
            else if (peek().value() == '<') {
                consume();

                if (peek().has_value() && peek().value() == '<') {
                    consume();
                    tokens.push_back({ TokenType::shl_, line_count });
                }
                else if (peek().has_value() && peek().value() == '=') {
                    consume();
                    tokens.push_back({ TokenType::lte, line_count });
                }
                else {
                    tokens.push_back({ TokenType::lt, line_count });
                }
            }
            else if (peek().value() == '!') {
                consume();
                if (peek().has_value() && peek().value() == '=') {
                    consume();
                    tokens.push_back({ TokenType::neq, line_count });
                } else {
                    tokens.push_back({ TokenType::not_, line_count });
                }
            }
            else if (peek().value() == '&') {
                consume();
                if (peek().has_value() && peek().value() == '&') {
                    consume();
                    tokens.push_back({ TokenType::and_, line_count });
                } else {
                    cerr << "Expected '&' after '&' on line " << line_count << endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if (peek().value() == '|') {
                consume();
                if (peek().has_value() && peek().value() == '|') {
                    consume();
                    tokens.push_back({ TokenType::or_, line_count });
                } else {
                    cerr << "Expected '|' after '|' on line " << line_count << endl;
                    exit(EXIT_FAILURE);
                }
            }

            else if (peek().value() == ',') {
                consume();
                tokens.push_back({TokenType::comma, line_count});
            }
            else if (peek().value() == ':') {
                consume();
                tokens.push_back({TokenType::colon, line_count});
            }
            else if (peek().value() == '.') {
                consume();
                tokens.push_back({TokenType::dot, line_count});
            }
            else if (peek().value() == '?') {
                consume();
                tokens.push_back({TokenType::question, line_count});
            }
            else if (peek().value() == '[') {
                consume();
                tokens.push_back({TokenType::open_square, line_count});
            }
            else if (peek().value() == ']') {
                consume();
                tokens.push_back({TokenType::close_square, line_count});
            }




            else if (peek().value() == '{') {
                consume();
                tokens.push_back({ TokenType::open_curly, line_count });
            }
            else if (peek().value() == '}') {
                consume();
                tokens.push_back({ TokenType::close_curly, line_count });
            }
            else if (peek().value() == '\n') {
                consume();
                line_count++;
            }
            else if (isspace(peek().value())) {
                consume();
            }
            else {
                cerr << "Invalid token '" << peek().value()
                << "' on line " << line_count << endl;
                exit(EXIT_FAILURE);
            }
        }
        m_index = 0;
        return tokens;
    }

private:
    [[nodiscard]] optional<char> peek(const size_t offset = 0) const
    {
        if (m_index + offset >= m_src.length()) {
            return {};
        }
        return m_src.at(m_index + offset);
    }

    char consume()
    {
        return m_src.at(m_index++);
    }

    const string m_src;
    size_t m_index = 0;
};
