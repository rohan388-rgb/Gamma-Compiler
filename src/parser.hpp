
#pragma once
#include <cassert>
#include <variant>

#include "arena.hpp"
#include "tokenization.hpp"

using namespace std;

struct NodeTermIntLit {
    Token int_lit;
};
struct NodeTermFloatLit
{
    Token float_lit;
};

struct NodeTermCharLit
{
    Token char_lit;
};

struct NodeTermIdent {
    Token ident;
};

struct NodeExpr;

struct NodeTermParen {
    NodeExpr* expr;
};

struct NodeBinExprAdd {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprMulti {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprSub {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprDiv {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprRemainder {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprGT {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprLT {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprGTE {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprLTE {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprEQ {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprNEQ {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprAND {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprOR {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprBitAnd {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprBitOr {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprBitXor {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprShl {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprShr {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExpr {
    variant<
        NodeBinExprAdd*,
        NodeBinExprSub*,
        NodeBinExprMulti*,
        NodeBinExprDiv*,
        NodeBinExprRemainder*,

        NodeBinExprGT*,
        NodeBinExprLT*,
        NodeBinExprGTE*,
        NodeBinExprLTE*,

        NodeBinExprEQ*,
        NodeBinExprNEQ*,

        NodeBinExprAND*,
        NodeBinExprOR*,

        NodeBinExprBitAnd*,
        NodeBinExprBitOr*,
        NodeBinExprBitXor*,
        NodeBinExprShl*,
        NodeBinExprShr*
    > var;
};

struct NodeTermBool {
    bool value;
};
struct NodeTermNull {
};

struct NodeTermLength {
    NodeExpr* expr;
};

struct NodeTermIndex {
    NodeExpr* array;
    NodeExpr* index;
};

struct NodeTermFieldAccess
{
    NodeExpr* object;
    Token field;
};

struct NodeTermSizeOf {
    NodeExpr* expr;
};
struct NodeTermTypeOf {
    NodeExpr* expr;
};

struct NodeTermContains {
    NodeExpr* text;
    NodeExpr* value;
};

struct NodeTermStartsWith {
    NodeExpr* text;
    NodeExpr* value;
};

struct NodeTermEndsWith {
    NodeExpr* text;
    NodeExpr* value;
};

struct NodeTermTrim {
    NodeExpr* expr;
};

struct NodeTermUppercase {
    NodeExpr* expr;
};

struct NodeTermLowercase {
    NodeExpr* expr;
};

struct NodeTermReplace {
    NodeExpr* text;
    NodeExpr* old_value;
    NodeExpr* new_value;
};

struct NodeTermSplit {
    NodeExpr* text;
    NodeExpr* delimiter;
};

struct NodeTermJoin {
    NodeExpr* array;
    NodeExpr* delimiter;
};

struct NodeTermSubstring {
    NodeExpr* text;
    NodeExpr* start;
    NodeExpr* length;
};

struct NodeTermAddressOf {
    NodeExpr* expr;
};

struct NodeTermDereference {
    NodeExpr* expr;
};

struct NodeTermNew {
    NodeExpr* expr;
};

struct NodeTermDelete {
    NodeExpr* expr;
};

struct NodeTermThis {
};
struct NodeTermFree {
    NodeExpr* expr;
};

struct NodeTermObject {
    NodeExpr* expr;
};

struct NodeTermPointer {
    NodeExpr* expr;
};

struct NodeTermAllocate {
    NodeExpr* expr;
};

struct NodeTermRead {
};

struct NodeTermPrint {
};

struct NodeTermString
{
    Token string;
};

struct NodeTermArray {
    vector<NodeExpr*> elements;
};

struct NodeCreateFunctionCall {
    Token name;
    vector<NodeExpr*> args;
};
struct NodeStmtCall
{
    NodeCreateFunctionCall* call;
};

struct NodeTerm {
    variant<
    NodeTermIntLit*,
    NodeTermFloatLit*,
    NodeTermCharLit*,
    NodeTermIdent*,
    NodeTermParen*,
    NodeTermBool*,
    NodeTermNull*,
    NodeTermLength*,
    NodeTermIndex*,
    NodeTermFieldAccess*,
    NodeTermSizeOf*,
    NodeTermTypeOf*,
    NodeTermString*,
    NodeTermStartsWith*,
    NodeTermEndsWith*,
    NodeTermTrim*,
    NodeTermContains*,
    NodeTermUppercase*,
    NodeTermLowercase*,
    NodeTermReplace*,
    NodeTermSplit*,
    NodeTermJoin*,
    NodeTermSubstring*,
    NodeTermAddressOf*,
    NodeTermDereference*,
    NodeTermNew*,
    NodeTermDelete*,
    NodeTermThis*,
    NodeTermFree*,
    NodeTermObject*,
    NodeTermPointer*,
    NodeTermAllocate*,
    NodeTermRead*,
    NodeTermPrint*,
    NodeTermArray*,
    NodeCreateFunctionCall*
> var;
};

struct NodeUnaryNot {
    NodeExpr* expr;
};

struct NodeUnaryBitNot {
    NodeExpr* expr;
};
struct NodeQuestion
{
    NodeExpr* condition;
    NodeExpr* true_expr;
    NodeExpr* false_expr;
};

struct NodeArrow
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeDoubleColon
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeExpr {
    variant<
    NodeTerm*,
    NodeBinExpr*,
    NodeUnaryNot*,
    NodeUnaryBitNot*,
    NodeQuestion*,
    NodeArrow*,
    NodeDoubleColon*
    > var;
};
struct NodeStmtQuit {
    NodeExpr* expr;
};

struct NodeStmtLet {
    Token declaration;
    Token type;
    Token ident;
    NodeExpr* expr {};
};

struct NodeStmt;

struct NodeScope {
    vector<NodeStmt*> stmts;
};

struct NodeIfPred;

struct NodeIfPredOrWhen {
    NodeExpr* expr {};
    NodeScope* scope {};
    optional<NodeIfPred*> pred;
};

struct NodeIfPredOtherwise {
    NodeScope* scope;
};

struct NodeIfPred {
    variant<NodeIfPredOrWhen*, NodeIfPredOtherwise*> var;
};

struct NodeStmtIf {
    NodeExpr* expr {};
    NodeScope* scope {};
    optional<NodeIfPred*> pred;
};

struct NodeStmtAssign {
    Token ident;
    Token op;
    NodeExpr* expr {};
    NodeExpr* index {};
    NodeExpr* target_expr {};
    Token field;
    bool has_field = false;
};

struct NodeStmtAsk
{
    NodeExpr* prompt {};
    Token ident;
};

struct NodeStmtWhile {
    NodeExpr* expr;
    NodeScope* scope;
};

struct NodeStmtDoWhile {
    NodeExpr* expr;
    NodeScope* scope;
};

struct NodeStmtFor
{
    Token ident;
    NodeExpr* start {};
    NodeExpr* end {};
    NodeScope* scope {};
};
struct NodeStmtPrint {
    vector<NodeExpr*> exprs;
};

struct NodeStmtGive {
    NodeExpr* expr;
};

struct NodeStmtReturn {
    NodeExpr* expr;
};

struct NodeStmtStop {
};

struct NodeStmtExit {
};

struct NodeStmtNext {
};

struct NodeStmtContinue {
};

struct NodeCreateFunctionParam
{
    Token type;
    Token ident;
};

struct NodeCreateFunction
{
    Token name;
    vector<NodeCreateFunctionParam> params;
    NodeScope* scope {};
};

struct NodeStmtCreate
{
    NodeCreateFunction* function;
};


struct NodeOption
{
    NodeExpr* value {};
    NodeScope* scope {};
};

struct NodeOther
{
    NodeScope* scope {};
};

struct NodeStmtChoose
{
    NodeExpr* expr {};
    vector<NodeOption> cases;
    optional<NodeOther> default_case;
};

struct NodeStructField
{
    Token ident;
    NodeExpr* value {};
};

struct NodeStruct
{
    Token name;
    vector<NodeStructField> fields;
};

struct NodeStmtStruct
{
    NodeStruct* structure;
};

struct NodeClassField
{
    Token ident;
    NodeExpr* value {};
};

struct NodeClassFieldAccess
{
    Token access;
    Token ident;
    bool is_method {};
};
struct NodeClassMethod
{
    Token name;
    Token access;
    vector<NodeCreateFunctionParam> params;
    NodeScope* scope {};
};
struct NodeClassConstructor
{
    Token base;
    vector<NodeCreateFunctionParam> params;
    NodeScope* scope {};
};
struct NodeSuperCall
{
    vector<NodeExpr*> args;
};

struct NodeClass
{
    Token name;
    Token base;

    vector<NodeClassField> fields;
    vector<NodeClassMethod> methods;
    vector<NodeClassFieldAccess> access_fields;

    optional<NodeClassConstructor> constructor;
};

struct NodeStmtClass
{
    NodeClass* class_;
};

struct NodeEnum {
    Token name;
    vector<Token> values;
};

struct NodeStmtEnum {
    NodeEnum* enum_;
};

struct NodeStmtThrow {
    NodeExpr* expr;
};

struct NodeStmtTry
{
    NodeScope* try_scope {};
    Token catch_ident;
    NodeScope* catch_scope {};
    NodeScope* finally_scope {};
};

struct NodeNamespace
{
    Token name;
    NodeScope* scope {};
};

struct NodeStmtNamespace
{
    NodeNamespace* ns {};
};

struct NodeUsing
{
    Token name;
};

struct NodeStmtUsing
{
    NodeUsing* using_;
};

struct NodeImport
{
    Token name;
};

struct NodeStmtImport
{
    NodeImport* import_;
};

struct NodeInclude
{
    Token name;
};

struct NodeStmtInclude
{
    NodeInclude* include_;
};

struct NodeStmt {
    variant<
        NodeStmtQuit*,
        NodeStmtLet*,
        NodeScope*,
        NodeStmtIf*,
        NodeStmtAssign*,
        NodeStmtWhile*,
        NodeStmtDoWhile*,
        NodeStmtFor*,
        NodeStmtCreate*,
        NodeStmtCall*,
        NodeStmtChoose*,
        NodeStmtAsk*,
        NodeStmtStruct*,
        NodeStmtClass*,
        NodeStmtEnum*,
        NodeStmtThrow*,
        NodeStmtTry*,
        NodeStmtNamespace*,
        NodeStmtUsing*,
        NodeStmtImport*,
        NodeStmtInclude*,
        NodeStmtPrint*,
        NodeStmtGive*,
        NodeStmtReturn*,
        NodeStmtStop*,
        NodeStmtNext*,
        NodeStmtExit*,
        NodeStmtContinue*,
        NodeSuperCall*
    > var;
};

struct NodeProg {
    vector<NodeStmt*> stmts;
};





class Parser {
public:
    explicit Parser(vector<Token> tokens)
        : m_tokens(move(tokens))
        , m_allocator(1024 * 1024 * 4) // 4 mb
    {
    }

    void error_expected(const string& msg) const
    {
        cerr << "\n========== PARSER ERROR ==========\n";

        if (peek().has_value())
        {
            cerr << "Expected : " << msg << '\n';
            cerr << "Found    : " << to_string(peek()->type);

            if (peek()->value.has_value())
                cerr << " (" << peek()->value.value() << ")";

            cerr << '\n';
            cerr << "Line     : " << peek()->line << '\n';
        }
        else
        {
            cerr << "Unexpected end of file.\n";
        }

        cerr << "==================================\n";

        exit(EXIT_FAILURE);
    }

    optional<NodeStmt*> parse_call_stmt()
    {
        if (!try_consume(TokenType::call_))
            return {};

        auto call = m_allocator.emplace<NodeCreateFunctionCall>();

        call->name = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::with_);
        try_consume(TokenType::argument_);

        if (peek().has_value() && peek()->type != TokenType::dot)
        {
            auto expr = parse_expr();

            if (!expr)
                error_expected("function argument");

            call->args.push_back(expr.value());

            while (try_consume(TokenType::comma))
            {
                auto next_expr = parse_expr();

                if (!next_expr)
                    error_expected("function argument");

                call->args.push_back(next_expr.value());
            }
        }

        try_consume_err(TokenType::dot);

        auto stmt_call = m_allocator.emplace<NodeStmtCall>();

        stmt_call->call = call;

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmt_call;

        return stmt;
    }
    optional<NodeTerm*> parse_term() // NOLINT(*-no-recursion)
    {
        if (auto int_lit = try_consume(TokenType::int_lit)) {
            auto term_int_lit = m_allocator.emplace<NodeTermIntLit>(int_lit.value());
            auto term = m_allocator.emplace<NodeTerm>(term_int_lit);
            return term;
        }

        if (auto float_lit = try_consume(TokenType::float_lit))
        {
            auto term_float_lit =
                m_allocator.emplace<NodeTermFloatLit>(float_lit.value());

            auto term =
                m_allocator.emplace<NodeTerm>(term_float_lit);

            return term;
        }

        if (auto char_lit = try_consume(TokenType::char_lit))
        {
            auto term_char_lit =
                m_allocator.emplace<NodeTermCharLit>(char_lit.value());

            auto term =
                m_allocator.emplace<NodeTerm>(term_char_lit);

            return term;
        }


        if (auto str = try_consume(TokenType::string_lit))
        {
            auto term_string =
                m_allocator.emplace<NodeTermString>(str.value());

            auto term =
                m_allocator.emplace<NodeTerm>(term_string);

            return term;
        }

        // Function call as an expression
        if (try_consume(TokenType::call_))
        {
            auto call = m_allocator.emplace<NodeCreateFunctionCall>();

            call->name = try_consume_err(TokenType::ident);

            try_consume_err(TokenType::with_);
            try_consume(TokenType::argument_);

            if (peek().has_value() && peek()->type != TokenType::dot)
            {
                auto expr = parse_expr();

                if (!expr)
                    error_expected("function argument");

                call->args.push_back(expr.value());

                while (try_consume(TokenType::comma))
                {
                    auto next_expr = parse_expr();

                    if (!next_expr)
                        error_expected("function argument");

                    call->args.push_back(next_expr.value());
                }
            }

            auto term = m_allocator.emplace<NodeTerm>(call);

            return term;
        }

        if (auto ident = parse_identifier())
        {
            auto expr_ident =
                m_allocator.emplace<NodeTermIdent>(ident.value());

            auto term =
                m_allocator.emplace<NodeTerm>(expr_ident);

            // Struct field access:
            // p.age
            // Struct field access:
            // p.age
            size_t saved_index = m_index;

            if (try_consume(TokenType::dot))
            {
                auto field = try_consume(TokenType::ident);

                if (field.has_value())
                {
                    auto field_access =
                        m_allocator.emplace<NodeTermFieldAccess>();

                    field_access->object =
                        m_allocator.emplace<NodeExpr>(term);

                    field_access->field = field.value();

                    auto field_term =
                        m_allocator.emplace<NodeTerm>(field_access);

                    return field_term;
                }

                // The dot was actually a statement terminator.
                m_index = saved_index;
            }

            // Array indexing:
            // numbers[0]
            if (try_consume(TokenType::open_square))
            {
                auto index = parse_expr();

                if (!index)
                    error_expected("array index");

                try_consume_err(TokenType::close_square);

                auto array_expr =
                    m_allocator.emplace<NodeExpr>(term);

                auto term_index =
                    m_allocator.emplace<NodeTermIndex>();

                term_index->array = array_expr;
                term_index->index = index.value();

                auto index_term =
                    m_allocator.emplace<NodeTerm>(term_index);

                return index_term;
            }

            return term;
        }

        if (try_consume(TokenType::true_)) {
            auto term_bool = m_allocator.emplace<NodeTermBool>(true);
            auto term = m_allocator.emplace<NodeTerm>(term_bool);
            return term;
        }

        if (try_consume(TokenType::false_)) {
            auto term_bool = m_allocator.emplace<NodeTermBool>(false);
            auto term = m_allocator.emplace<NodeTerm>(term_bool);
            return term;
        }

        if (try_consume(TokenType::null_)) {
            auto term_null = m_allocator.emplace<NodeTermNull>();
            auto term = m_allocator.emplace<NodeTerm>(term_null);
            return term;
        }

        if (try_consume(TokenType::length_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `length`");

            auto term_length = m_allocator.emplace<NodeTermLength>();
            term_length->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_length);
            return term;
        }

        if (try_consume(TokenType::index_)) {
            auto array = parse_expr();

            if (!array)
                error_expected("array expression after `index`");

            try_consume_err(TokenType::open_square);

            auto index = parse_expr();

            if (!index)
                error_expected("index expression");

            try_consume_err(TokenType::close_square);

            auto term_index = m_allocator.emplace<NodeTermIndex>();
            term_index->array = array.value();
            term_index->index = index.value();

            auto term = m_allocator.emplace<NodeTerm>(term_index);
            return term;
        }
        if (try_consume(TokenType::sizeof_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `sizeof`");

            auto term_sizeof = m_allocator.emplace<NodeTermSizeOf>();
            term_sizeof->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_sizeof);
            return term;
        }
        if (try_consume(TokenType::typeof_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `typeof`");

            auto term_typeof = m_allocator.emplace<NodeTermTypeOf>();
            term_typeof->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_typeof);
            return term;
        }

        if (try_consume(TokenType::contains_)) {
            auto text = parse_expr();

            if (!text)
                error_expected("text expression after `contains`");

            try_consume_err(TokenType::comma);

            auto value = parse_expr();

            if (!value)
                error_expected("value expression after `contains`");

            auto term_contains = m_allocator.emplace<NodeTermContains>();
            term_contains->text = text.value();
            term_contains->value = value.value();

            auto term = m_allocator.emplace<NodeTerm>(term_contains);
            return term;
        }

        if (try_consume(TokenType::starts_with_)) {
            auto text = parse_expr();

            if (!text)
                error_expected("text expression after `starts_with`");

            try_consume_err(TokenType::comma);

            auto value = parse_expr();

            if (!value)
                error_expected("value expression after `starts_with`");

            auto term_starts_with = m_allocator.emplace<NodeTermStartsWith>();
            term_starts_with->text = text.value();
            term_starts_with->value = value.value();

            auto term = m_allocator.emplace<NodeTerm>(term_starts_with);
            return term;
        }
        if (try_consume(TokenType::ends_with_)) {
            auto text = parse_expr();

            if (!text)
                error_expected("text expression after `ends_with`");

            try_consume_err(TokenType::comma);

            auto value = parse_expr();

            if (!value)
                error_expected("value expression after `ends_with`");

            auto term_ends_with = m_allocator.emplace<NodeTermEndsWith>();
            term_ends_with->text = text.value();
            term_ends_with->value = value.value();

            auto term = m_allocator.emplace<NodeTerm>(term_ends_with);
            return term;
        }
        if (try_consume(TokenType::trim_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `trim`");

            auto term_trim = m_allocator.emplace<NodeTermTrim>();
            term_trim->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_trim);
            return term;
        }
        if (try_consume(TokenType::uppercase_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `uppercase`");

            auto term_uppercase = m_allocator.emplace<NodeTermUppercase>();
            term_uppercase->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_uppercase);
            return term;
        }
        if (try_consume(TokenType::lowercase_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `lowercase`");

            auto term_lowercase = m_allocator.emplace<NodeTermLowercase>();
            term_lowercase->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_lowercase);
            return term;
        }

        if (try_consume(TokenType::replace_)) {
            auto text = parse_expr();

            if (!text)
                error_expected("text expression after `replace`");

            try_consume_err(TokenType::comma);

            auto old_value = parse_expr();

            if (!old_value)
                error_expected("old value after `replace`");

            try_consume_err(TokenType::comma);

            auto new_value = parse_expr();

            if (!new_value)
                error_expected("new value after `replace`");

            auto term_replace = m_allocator.emplace<NodeTermReplace>();
            term_replace->text = text.value();
            term_replace->old_value = old_value.value();
            term_replace->new_value = new_value.value();

            auto term = m_allocator.emplace<NodeTerm>(term_replace);
            return term;
        }

        if (try_consume(TokenType::split_)) {
            auto text = parse_expr();

            if (!text)
                error_expected("text expression after `split`");

            try_consume_err(TokenType::comma);

            auto delimiter = parse_expr();

            if (!delimiter)
                error_expected("delimiter after `split`");

            auto term_split = m_allocator.emplace<NodeTermSplit>();
            term_split->text = text.value();
            term_split->delimiter = delimiter.value();

            auto term = m_allocator.emplace<NodeTerm>(term_split);
            return term;
        }

        if (try_consume(TokenType::join_)) {
            auto array = parse_expr();

            if (!array)
                error_expected("array expression after `join`");

            try_consume_err(TokenType::comma);

            auto delimiter = parse_expr();

            if (!delimiter)
                error_expected("delimiter after `join`");

            auto term_join = m_allocator.emplace<NodeTermJoin>();
            term_join->array = array.value();
            term_join->delimiter = delimiter.value();

            auto term = m_allocator.emplace<NodeTerm>(term_join);
            return term;
        }

        if (try_consume(TokenType::substring_)) {
            auto text = parse_expr();

            if (!text)
                error_expected("text expression after `substring`");

            try_consume_err(TokenType::comma);

            auto start = parse_expr();

            if (!start)
                error_expected("start index after `substring`");

            try_consume_err(TokenType::comma);

            auto length = parse_expr();

            if (!length)
                error_expected("length after `substring`");

            auto term_substring = m_allocator.emplace<NodeTermSubstring>();
            term_substring->text = text.value();
            term_substring->start = start.value();
            term_substring->length = length.value();

            auto term = m_allocator.emplace<NodeTerm>(term_substring);
            return term;
        }

        if (try_consume(TokenType::address_of_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `address_of`");

            auto term_address_of = m_allocator.emplace<NodeTermAddressOf>();
            term_address_of->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_address_of);
            return term;
        }

        if (try_consume(TokenType::dereference_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `dereference`");

            auto term_dereference = m_allocator.emplace<NodeTermDereference>();
            term_dereference->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_dereference);
            return term;
        }

        if (try_consume(TokenType::new_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `new`");

            auto term_new = m_allocator.emplace<NodeTermNew>();
            term_new->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_new);
            return term;
        }

        if (try_consume(TokenType::delete_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `delete`");

            auto term_delete = m_allocator.emplace<NodeTermDelete>();
            term_delete->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_delete);
            return term;
        }

        if (try_consume(TokenType::this_)) {
            auto term_this = m_allocator.emplace<NodeTermThis>();

            auto term = m_allocator.emplace<NodeTerm>(term_this);
            return term;
        }

        if (try_consume(TokenType::free_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `free`");

            auto term_free = m_allocator.emplace<NodeTermFree>();
            term_free->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_free);
            return term;
        }

        if (try_consume(TokenType::object_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `object`");

            auto term_object = m_allocator.emplace<NodeTermObject>();
            term_object->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_object);
            return term;
        }

        if (try_consume(TokenType::pointer_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `pointer`");

            auto term_pointer = m_allocator.emplace<NodeTermPointer>();
            term_pointer->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_pointer);
            return term;
        }

        if (try_consume(TokenType::allocate_)) {
            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `allocate`");

            auto term_allocate = m_allocator.emplace<NodeTermAllocate>();
            term_allocate->expr = expr.value();

            auto term = m_allocator.emplace<NodeTerm>(term_allocate);
            return term;
        }

        if (try_consume(TokenType::read_)) {
            auto term_read = m_allocator.emplace<NodeTermRead>();

            auto term = m_allocator.emplace<NodeTerm>(term_read);
            return term;
        }

        if (try_consume(TokenType::print_)) {
            auto term_print = m_allocator.emplace<NodeTermPrint>();

            auto term = m_allocator.emplace<NodeTerm>(term_print);
            return term;
        }

        // Array literal
        if (try_consume(TokenType::open_square)) {
            auto term_array = m_allocator.emplace<NodeTermArray>();

            if (!try_consume(TokenType::close_square)) {
                while (true) {
                    auto expr = parse_expr();

                    if (!expr.has_value()) {
                        error_expected("array element");
                    }

                    term_array->elements.push_back(expr.value());

                    if (try_consume(TokenType::close_square)) {
                        break;
                    }

                    try_consume_err(TokenType::comma);
                }
            }

            auto term = m_allocator.emplace<NodeTerm>(term_array);
            return term;
        }



        if (const auto open_paren = try_consume(TokenType::open_paren)) {
            auto expr = parse_expr();
            if (!expr.has_value()) {
                error_expected("expression");
            }
            try_consume_err(TokenType::close_paren);
            auto term_paren = m_allocator.emplace<NodeTermParen>(expr.value());
            auto term = m_allocator.emplace<NodeTerm>(term_paren);
            return term;
        }


        return {};
    }

    optional<TokenType> parse_comparison_operator()
    {
        if (!try_consume(TokenType::is_))
            return {};

        if (try_consume(TokenType::greater_))
        {
            try_consume_err(TokenType::than_);

            if (try_consume(TokenType::or_))
            {
                try_consume_err(TokenType::equal_);
                try_consume_err(TokenType::to_);
                return TokenType::gte;
            }

            return TokenType::gt;
        }

        if (try_consume(TokenType::less_))
        {
            try_consume_err(TokenType::than_);

            if (try_consume(TokenType::or_))
            {
                try_consume_err(TokenType::equal_);
                try_consume_err(TokenType::to_);
                return TokenType::lte;
            }

            return TokenType::lt;
        }

        if (try_consume(TokenType::equal_))
        {
            try_consume_err(TokenType::to_);
            return TokenType::eqeq;
        }

        if (try_consume(TokenType::not_))
        {
            try_consume_err(TokenType::equal_);
            try_consume_err(TokenType::to_);
            return TokenType::neq;
        }

        error_expected("comparison operator");

        return {};
    }

    optional<NodeExpr*> parse_expr(const int min_prec = 0) // NOLINT(*-no-recursion)
{
        optional<NodeExpr*> expr_lhs;

        if (try_consume(TokenType::not_))
        {
            auto operand = parse_expr(7);

            if (!operand.has_value())
            {
                error_expected("expression after `not`");
            }

            auto unary_not = m_allocator.emplace<NodeUnaryNot>();
            unary_not->expr = operand.value();

            expr_lhs = m_allocator.emplace<NodeExpr>();
            expr_lhs.value()->var = unary_not;
        }
        else if (try_consume(TokenType::bit_not_))
        {
            auto operand = parse_expr(7);

            if (!operand.has_value())
            {
                error_expected("expression after `bit_not`");
            }

            auto unary_bit_not = m_allocator.emplace<NodeUnaryBitNot>();
            unary_bit_not->expr = operand.value();

            expr_lhs = m_allocator.emplace<NodeExpr>();
            expr_lhs.value()->var = unary_bit_not;
        }
        else
        {
            optional<NodeTerm*> term_lhs = parse_term();

            if (!term_lhs.has_value())
            {
                return {};
            }

            expr_lhs = m_allocator.emplace<NodeExpr>(term_lhs.value());
        }
        while (true) {

            optional<Token> curr_tok = peek();
            optional<int> prec;
            optional<TokenType> english_comparison;
            if (curr_tok.has_value() && curr_tok->type == TokenType::question)
            {
                if (min_prec > 0)
                    break;

                consume();

                auto true_expr = parse_expr();

                if (!true_expr)
                    error_expected("expression after `?`");

                try_consume_err(TokenType::colon);

                auto false_expr = parse_expr();

                if (!false_expr)
                    error_expected("expression after `:`");

                auto question = m_allocator.emplace<NodeQuestion>();

                question->condition = expr_lhs.value();
                question->true_expr = true_expr.value();
                question->false_expr = false_expr.value();

                auto question_expr = m_allocator.emplace<NodeExpr>();
                question_expr->var = question;

                expr_lhs = question_expr;

                continue;
            }

            if (curr_tok.has_value() && curr_tok->type == TokenType::arrow)
            {
                if (min_prec > 2)
                    break;

                consume();

                auto rhs = parse_expr(3);

                if (!rhs)
                    error_expected("expression after `->`");

                auto arrow = m_allocator.emplace<NodeArrow>();

                arrow->lhs = expr_lhs.value();
                arrow->rhs = rhs.value();

                auto arrow_expr = m_allocator.emplace<NodeExpr>();
                arrow_expr->var = arrow;

                expr_lhs = arrow_expr;

                continue;
            }

            if (curr_tok.has_value() && curr_tok->type == TokenType::double_colon)
            {
                if (min_prec > 2)
                    break;

                consume();

                auto rhs = parse_expr(3);

                if (!rhs)
                    error_expected("expression after `::`");

                auto double_colon = m_allocator.emplace<NodeDoubleColon>();

                double_colon->lhs = expr_lhs.value();
                double_colon->rhs = rhs.value();

                auto double_colon_expr = m_allocator.emplace<NodeExpr>();
                double_colon_expr->var = double_colon;

                expr_lhs = double_colon_expr;

                continue;
            }
            if (curr_tok.has_value() && curr_tok->type == TokenType::is_) {
                if (min_prec > 3)
                    break;
                size_t saved_index = m_index;
                english_comparison = parse_comparison_operator();
                if (!english_comparison.has_value()) {m_index = saved_index;
                    break;
                }
                prec = 3;
            }
            else if (curr_tok.has_value()) {

                if (curr_tok->type == TokenType::bit_or_) {
                    prec = 1;
                }
                else if (curr_tok->type == TokenType::bit_xor_) {
                    prec = 2;
                }
                else if (curr_tok->type == TokenType::bit_and_) {
                    prec = 3;
                }
                else {
                    prec = bin_prec(curr_tok->type);
                }

                if (!prec.has_value() || prec < min_prec) {
                    break;
                }
            }
            else {
                break;
            }
            TokenType type;

            if (english_comparison.has_value()) {
                type = english_comparison.value();
            }
            else {
                type = consume().type;
            }
            const int next_min_prec = prec.value() + 1;
            auto expr_rhs = parse_expr(next_min_prec);
            if (!expr_rhs.has_value()) {
                error_expected("expression");
            }
            auto expr = m_allocator.emplace<NodeBinExpr>();
            auto expr_lhs2 = m_allocator.emplace<NodeExpr>();
            if (type == TokenType::plus_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto add = m_allocator.emplace<NodeBinExprAdd>(expr_lhs2, expr_rhs.value());
                expr->var = add;
            }
            else if (type == TokenType::times_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto multi = m_allocator.emplace<NodeBinExprMulti>(expr_lhs2, expr_rhs.value());
                expr->var = multi;
            }
            else if (type == TokenType::minus_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto sub = m_allocator.emplace<NodeBinExprSub>(expr_lhs2, expr_rhs.value());
                expr->var = sub;
            }
            else if (type == TokenType::divided_by_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto div = m_allocator.emplace<NodeBinExprDiv>(expr_lhs2, expr_rhs.value());
                expr->var = div;
            }
            else if (type == TokenType::remainder_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto rem = m_allocator.emplace<NodeBinExprRemainder>(expr_lhs2,expr_rhs.value()
                );
                expr->var = rem;
            }
            else if(type==TokenType::gt)
            {
                expr_lhs2->var = expr_lhs.value()->var;
                auto node = m_allocator.emplace<NodeBinExprGT>(expr_lhs2, expr_rhs.value());
                expr->var = node;
            }
            else if (type == TokenType::lt) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto lt = m_allocator.emplace<NodeBinExprLT>(expr_lhs2, expr_rhs.value());
                expr->var = lt;
            }
            else if (type == TokenType::gte) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto gte = m_allocator.emplace<NodeBinExprGTE>(expr_lhs2, expr_rhs.value());
                expr->var = gte;
            }
            else if (type == TokenType::lte) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto lte = m_allocator.emplace<NodeBinExprLTE>(expr_lhs2, expr_rhs.value());
                expr->var = lte;
            }
            else if (type == TokenType::eqeq) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto eq = m_allocator.emplace<NodeBinExprEQ>(expr_lhs2, expr_rhs.value());
                expr->var = eq;
            }
            else if (type == TokenType::neq) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto neq = m_allocator.emplace<NodeBinExprNEQ>(expr_lhs2, expr_rhs.value());
                expr->var = neq;
            }
            else if (type == TokenType::and_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto andExpr = m_allocator.emplace<NodeBinExprAND>(expr_lhs2, expr_rhs.value());
                expr->var = andExpr;
            }
            else if (type == TokenType::or_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto orExpr = m_allocator.emplace<NodeBinExprOR>(expr_lhs2, expr_rhs.value());
                expr->var = orExpr;
            }
            else if (type == TokenType::bit_and_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto node = m_allocator.emplace<NodeBinExprBitAnd>(
                    expr_lhs2,
                    expr_rhs.value()
                );
                expr->var = node;
            }
            else if (type == TokenType::bit_or_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto node = m_allocator.emplace<NodeBinExprBitOr>(
                    expr_lhs2,
                    expr_rhs.value()
                );
                expr->var = node;
            }
            else if (type == TokenType::bit_xor_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto node = m_allocator.emplace<NodeBinExprBitXor>(
                    expr_lhs2,
                    expr_rhs.value()
                );
                expr->var = node;
            }
            else if (type == TokenType::shl_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto node = m_allocator.emplace<NodeBinExprShl>(
                    expr_lhs2,
                    expr_rhs.value()
                );
                expr->var = node;
            }
            else if (type == TokenType::shr_) {
                expr_lhs2->var = expr_lhs.value()->var;
                auto node = m_allocator.emplace<NodeBinExprShr>(
                    expr_lhs2,
                    expr_rhs.value()
                );
                expr->var = node;
            }

            else {
                assert(false); // Unreachable;
            }
            auto bin_expr = m_allocator.emplace<NodeExpr>();
            bin_expr->var = expr;

            expr_lhs = bin_expr;
        }
        return expr_lhs;
    }

    optional<NodeScope*> parse_scope() // NOLINT(*-no-recursion)
    {
        if (!try_consume(TokenType::open_curly).has_value()) {
            return {};
        }
        auto scope = m_allocator.emplace<NodeScope>();
        while (auto stmt = parse_stmt()) {
            scope->stmts.push_back(stmt.value());
        }
        try_consume_err(TokenType::close_curly);
        return scope;
    }

    optional<NodeIfPred*> parse_if_pred() // NOLINT(*-no-recursion)
    {
        if (try_consume(TokenType::else_if_)) {
            const auto orwhen = m_allocator.alloc<NodeIfPredOrWhen>();
            if (const auto expr = parse_expr()) {
                orwhen->expr = expr.value();
            }
            else {
                error_expected("expression");
            }
            if (const auto scope = parse_scope()) {
                orwhen->scope = scope.value();
            }
            else {
                error_expected("scope");
            }
            orwhen->pred = parse_if_pred();
            auto pred = m_allocator.emplace<NodeIfPred>(orwhen);
            return pred;
        }
        if (try_consume(TokenType::otherwise_)) {
            auto otherwise = m_allocator.alloc<NodeIfPredOtherwise>();

            if (const auto scope = parse_scope()) {
                otherwise->scope = scope.value();
            }
            else {
                error_expected("scope");
            }
            auto pred = m_allocator.emplace<NodeIfPred>(otherwise);
            return pred;
        }
        return {};
    }

    optional<NodeStmt*> parse_for_stmt()
    {
        if (!try_consume(TokenType::for_))
            return {};

        auto stmt_for = m_allocator.emplace<NodeStmtFor>();

        stmt_for->ident = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::from_);

        auto start = parse_expr();

        if (!start)
            error_expected("for start value");

        stmt_for->start = start.value();

        try_consume_err(TokenType::to_);

        auto end = parse_expr();

        if (!end)
            error_expected("for end value");

        stmt_for->end = end.value();

        auto scope = parse_scope();

        if (!scope)
            error_expected("scope");

        stmt_for->scope = scope.value();

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmt_for;

        return stmt;
    }

    optional<vector<NodeCreateFunctionParam>> parse_function_parameters()
    {
        vector<NodeCreateFunctionParam> params;

        if (peek().has_value() && peek()->type == TokenType::open_curly)
            return params;

        try_consume(TokenType::parameter_);

        auto type = parse_type();

        if (!type.has_value())
            error_expected("parameter type");

        NodeCreateFunctionParam param;
        param.type = type.value();
        param.ident = try_consume_err(TokenType::ident);

        params.push_back(param);

        while (try_consume(TokenType::comma))
        {
            NodeCreateFunctionParam next_param;

            try_consume(TokenType::parameter_);

            auto next_type = parse_type();

            if (!next_type.has_value())
                error_expected("parameter type");

            next_param.type = next_type.value();
            next_param.ident = try_consume_err(TokenType::ident);

            params.push_back(next_param);
        }

        return params;
    }

    optional<NodeStmt*> parse_Create()
    {
        if (!try_consume(TokenType::define_))
            return {};

        try_consume_err(TokenType::function_);

        auto fn = m_allocator.emplace<NodeCreateFunction>();

        fn->name = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::with_);

        auto params = parse_function_parameters();

        if (!params.has_value())
            error_expected("function parameters");

        fn->params = params.value();

        auto scope = parse_scope();

        if (!scope)
            error_expected("scope");

        fn->scope = scope.value();

        auto stmtFn = m_allocator.emplace<NodeStmtCreate>();

        stmtFn->function = fn;

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmtFn;

        return stmt;
    }

    optional<NodeStmt*> parse_choose()
    {
        if (!try_consume(TokenType::choose_))
            return {};

        auto stmt_choose = m_allocator.emplace<NodeStmtChoose>();

        try_consume_err(TokenType::open_paren);

        auto expr = parse_expr();

        if (!expr)
            error_expected("expression");

        stmt_choose->expr = expr.value();

        try_consume_err(TokenType::close_paren);

        try_consume_err(TokenType::open_curly);

        while (peek().has_value())
        {
            if (peek()->type == TokenType::close_curly)
                break;

            if (try_consume(TokenType::option_))
            {
                NodeOption c;

                auto value = parse_expr();

                if (!value)
                    error_expected("case value");

                c.value = value.value();

                try_consume_err(TokenType::colon);

                auto scope = parse_scope();

                if (!scope)
                    error_expected("scope");

                c.scope = scope.value();

                stmt_choose->cases.push_back(c);

                continue;
            }

            if (try_consume(TokenType::other_))
            {
                NodeOther d;

                try_consume_err(TokenType::colon);

                auto scope = parse_scope();

                if (!scope)
                    error_expected("scope");

                d.scope = scope.value();

                stmt_choose->default_case = d;

                continue;
            }

            error_expected("case/default");
        }

        try_consume_err(TokenType::close_curly);

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmt_choose;

        return stmt;
    }

    optional<NodeStmt*> parse_struct()
    {
        if (!try_consume(TokenType::struct_))
            return {};

        auto node = m_allocator.emplace<NodeStruct>();

        node->name = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::open_curly);

        while (peek().has_value())
        {
            if (peek()->type == TokenType::close_curly)
                break;

            NodeStructField field;

            field.ident = try_consume_err(TokenType::ident);

            if (try_consume(TokenType::assign_))
            {
                auto value = parse_expr();

                if (!value)
                    error_expected("field value");

                field.value = value.value();
            }

            try_consume_err(TokenType::dot);

            node->fields.push_back(field);
        }

        try_consume_err(TokenType::close_curly);

        auto stmtStruct = m_allocator.emplace<NodeStmtStruct>();

        stmtStruct->structure = node;

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmtStruct;

        return stmt;
    }

    optional<NodeStmt*> parse_class()
    {
        if (!try_consume(TokenType::class_))
            return {};

        auto cls = m_allocator.emplace<NodeClass>();

        cls->name = try_consume_err(TokenType::ident);

        if (try_consume(TokenType::extends_))
        {
            cls->base = try_consume_err(TokenType::ident);
        }

        try_consume_err(TokenType::open_curly);

        while (peek().has_value())
        {
            if (peek()->type == TokenType::close_curly)
                break;
            if (peek()->type == TokenType::public_
    || peek()->type == TokenType::private_
    || peek()->type == TokenType::protected_)
{
    Token access = consume();

    // Access modifier before a method
    if (peek().has_value() && peek()->type == TokenType::function_)
    {
        consume();

        NodeClassMethod method;

        method.access = access;
        method.name = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::with_);

        auto params = parse_function_parameters();

        if (!params.has_value())
            error_expected("function parameters");

        method.params = params.value();

        auto scope = parse_scope();

        if (!scope)
            error_expected("method scope");

        method.scope = scope.value();

        cls->methods.push_back(method);

        continue;
    }

    // Access modifier before fields
    while (true)
    {
        NodeClassFieldAccess field;

        field.access = access;
        field.ident = try_consume_err(TokenType::ident);

        cls->access_fields.push_back(field);

        if (!try_consume(TokenType::comma))
            break;
    }

    try_consume_err(TokenType::dot);
    continue;
}

            if (peek()->type == TokenType::function_)
            {
                consume();

                NodeClassMethod method;

                method.name = try_consume_err(TokenType::ident);

                try_consume_err(TokenType::with_);

                auto params = parse_function_parameters();

                if (!params.has_value())
                    error_expected("function parameters");

                method.params = params.value();

                auto scope = parse_scope();

                if (!scope)
                    error_expected("method scope");

                method.scope = scope.value();

                cls->methods.push_back(method);

                continue;
            }

            if (peek()->type == TokenType::constructor_)
            {
                consume();

                NodeClassConstructor constructor;

                try_consume(TokenType::with_);

                if (try_consume(TokenType::base_))
                    constructor.base = try_consume_err(TokenType::ident);

                auto params = parse_function_parameters();

                if (!params.has_value())
                    error_expected("constructor parameters");

                constructor.params = params.value();

                auto scope = parse_scope();

                if (!scope)
                    error_expected("constructor scope");

                constructor.scope = scope.value();

                cls->constructor = constructor;

                continue;
            }

            if (peek()->type == TokenType::ident)
            {
                NodeClassField field;

                field.ident = consume();

                if (try_consume(TokenType::assign_))
                {
                    auto value = parse_expr();

                    if (!value.has_value())
                        error_expected("field value");

                    field.value = value.value();
                }

                try_consume_err(TokenType::dot);

                cls->fields.push_back(field);

                continue;
            }


            error_expected("class member");
        }

        try_consume_err(TokenType::close_curly);

        auto stmtClass = m_allocator.emplace<NodeStmtClass>();

        stmtClass->class_ = cls;

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmtClass;

        return stmt;
    }

    optional<NodeStmt*> parse_namespace()
    {
        if (!try_consume(TokenType::namespace_))
            return {};

        auto ns = m_allocator.emplace<NodeNamespace>();

        ns->name = try_consume_err(TokenType::ident);

        auto scope = parse_scope();

        if (!scope)
            error_expected("scope");

        ns->scope = scope.value();

        auto stmtNs = m_allocator.emplace<NodeStmtNamespace>();

        stmtNs->ns = ns;

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmtNs;

        return stmt;
    }

    optional<NodeStmt*> parse_using()
    {
        if (!try_consume(TokenType::using_))
            return {};

        auto using_node = m_allocator.emplace<NodeUsing>();

        using_node->name = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::dot);

        auto stmt_using = m_allocator.emplace<NodeStmtUsing>();
        stmt_using->using_ = using_node;

        auto stmt = m_allocator.emplace<NodeStmt>();
        stmt->var = stmt_using;

        return stmt;
    }

    optional<NodeStmt*> parse_import()
    {
        if (!try_consume(TokenType::import_))
            return {};

        auto import_node = m_allocator.emplace<NodeImport>();

        import_node->name = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::dot);

        auto stmt_import = m_allocator.emplace<NodeStmtImport>();
        stmt_import->import_ = import_node;

        auto stmt = m_allocator.emplace<NodeStmt>();
        stmt->var = stmt_import;

        return stmt;
    }

    optional<NodeStmt*> parse_include()
    {
        if (!try_consume(TokenType::include_))
            return {};

        auto include_node = m_allocator.emplace<NodeInclude>();

        include_node->name = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::dot);

        auto stmt_include = m_allocator.emplace<NodeStmtInclude>();
        stmt_include->include_ = include_node;

        auto stmt = m_allocator.emplace<NodeStmt>();
        stmt->var = stmt_include;

        return stmt;
    }

    optional<NodeStmt*> parse_assign_stmt()
    {
        if (!try_consume(TokenType::set_))
            return {};

        auto stmt_assign = m_allocator.emplace<NodeStmtAssign>();

        if (peek().has_value() &&
    peek()->type == TokenType::dereference_)
        {
            consume();

            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `dereference`");

            stmt_assign->ident = Token{
                .type = TokenType::dereference_
            };

            stmt_assign->target_expr = expr.value();
        }
        else
        {
            stmt_assign->ident =try_consume_err(TokenType::ident);

            if (try_consume(TokenType::dot))
            {
                stmt_assign->field =
                    try_consume_err(TokenType::ident);

                stmt_assign->has_field = true;
            }
        }

        NodeExpr* index = nullptr;

        if (try_consume(TokenType::open_square))
        {
            auto index_expr = parse_expr();

            if (!index_expr)
                error_expected("array index");

            try_consume_err(TokenType::close_square);

            index = index_expr.value();
        }
        else if (try_consume(TokenType::index_))
        {
            auto index_expr = parse_expr();

            if (!index_expr)
                error_expected("array index");

            index = index_expr.value();
        }

        stmt_assign->index = index;

        auto op = peek();

        if (!op.has_value()
            || (op->type != TokenType::to_
                && op->type != TokenType::assign_
                && op->type != TokenType::plus_assign_
                && op->type != TokenType::minus_assign_
                && op->type != TokenType::multiply_assign_
                && op->type != TokenType::divide_assign_
                && op->type != TokenType::mod_assign_))
        {
            error_expected("assignment operator");
        }

        stmt_assign->op = consume();
        if (auto expr = parse_expr())
        {
            stmt_assign->expr = expr.value();
        }
        else
        {
            error_expected("expression");
        }

        try_consume_err(TokenType::dot);

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmt_assign;

        return stmt;
    }

    optional<Token> parse_type()
    {
        if (auto type = try_consume(TokenType::number_))
            return type;
        if (auto type = try_consume(TokenType::decimal_))
            return type;
        if (auto type = try_consume(TokenType::text_))
            return type;
        if (auto type = try_consume(TokenType::boolean_))
            return type;
        if (auto type = try_consume(TokenType::character_))
            return type;
        if (auto type = try_consume(TokenType::array_))
            return type;
        if (auto type = try_consume(TokenType::pointer_))
            return type;
        if (auto type = try_consume(TokenType::ident))
            return type;
        return {};
    }

    optional<Token> parse_identifier()
    {
        if (auto ident = try_consume(TokenType::ident))
            return ident;

        return {};
    }
    optional<NodeStmt*> parse_do_while_stmt()
    {
        if (!try_consume(TokenType::do_))
            return {};
        auto stmt_do_while = m_allocator.emplace<NodeStmtDoWhile>();
        auto scope = parse_scope();
        if (!scope)
            error_expected("scope");
        stmt_do_while->scope = scope.value();
        try_consume_err(TokenType::while_);
        auto expr = parse_expr();
        if (!expr)
            error_expected("expression");
        stmt_do_while->expr = expr.value();
        try_consume_err(TokenType::dot);
        auto stmt = m_allocator.emplace<NodeStmt>();
        stmt->var = stmt_do_while;
        return stmt;
    }

    optional<NodeStmt*> parse_create_stmt()
    {
        if (!try_consume(TokenType::create_))
            return {};

        auto stmt_create = m_allocator.emplace<NodeStmtLet>();


        stmt_create->declaration = Token{
            .type = TokenType::create_
        };

        auto type = parse_type();

        if (!type.has_value())
            error_expected("variable type");

        stmt_create->type = type.value();

        stmt_create->ident = try_consume_err(TokenType::ident);

        if (try_consume(TokenType::to_))
        {
            auto expr = parse_expr();

            if (!expr)
                error_expected("initial value");

            stmt_create->expr = expr.value();
        }

        try_consume_err(TokenType::dot);

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmt_create;

        return stmt;
    }

    optional<NodeStmt*> parse_ask_stmt()
    {
        if (!try_consume(TokenType::ask_))
            return {};

        auto stmt_ask = m_allocator.emplace<NodeStmtAsk>();

        auto prompt = parse_expr(2);

        if (!prompt)
            error_expected("prompt");

        stmt_ask->prompt = prompt.value();

        try_consume_err(TokenType::and_);

        try_consume_err(TokenType::store_);

        try_consume_err(TokenType::in_);

        stmt_ask->ident = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::dot);

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = stmt_ask;

        return stmt;
    }

    optional<NodeStmt*> parse_enum()
    {
        if (!try_consume(TokenType::enum_))
            return {};

        auto node = m_allocator.emplace<NodeEnum>();

        node->name = try_consume_err(TokenType::ident);

        try_consume_err(TokenType::open_curly);

        while (peek().has_value()
            && peek()->type != TokenType::close_curly)
        {
            node->values.push_back(
                try_consume_err(TokenType::ident)
            );

            if (!try_consume(TokenType::comma))
                break;
        }

        try_consume_err(TokenType::close_curly);

        auto stmt_enum = m_allocator.emplace<NodeStmtEnum>();
        stmt_enum->enum_ = node;

        auto stmt = m_allocator.emplace<NodeStmt>();
        stmt->var = stmt_enum;

        return stmt;
    }

    optional<NodeStmt*> parse_try()
    {
        if (!try_consume(TokenType::try_))
            return {};

        auto node = m_allocator.emplace<NodeStmtTry>();

        auto try_scope = parse_scope();

        if (!try_scope)
            error_expected("try scope");

        node->try_scope = try_scope.value();

        bool has_handler = false;

        if (try_consume(TokenType::catch_))
        {
            has_handler = true;

            if (peek().has_value() && peek()->type != TokenType::open_curly)
            {
                node->catch_ident = try_consume_err(TokenType::ident);
            }

            auto catch_scope = parse_scope();

            if (!catch_scope)
                error_expected("catch scope");

            node->catch_scope = catch_scope.value();
        }

        if (try_consume(TokenType::finally_))
        {
            has_handler = true;

            auto finally_scope = parse_scope();

            if (!finally_scope)
                error_expected("finally scope");

            node->finally_scope = finally_scope.value();
        }

        if (!has_handler)
            error_expected("`catch` or `finally` after `try`");

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = node;

        return stmt;
    }

    optional<NodeStmt*> parse_super_stmt()
    {
        if (!try_consume(TokenType::super_))
            return {};

        auto super_call = m_allocator.emplace<NodeSuperCall>();

        if (try_consume(TokenType::with_))
        {
            if (peek().has_value() && peek()->type != TokenType::dot)
            {
                auto expr = parse_expr();

                if (!expr)
                    error_expected("super argument");

                super_call->args.push_back(expr.value());

                while (try_consume(TokenType::comma))
                {
                    auto next_expr = parse_expr();

                    if (!next_expr)
                        error_expected("super argument");

                    super_call->args.push_back(next_expr.value());
                }
            }
        }

        try_consume_err(TokenType::dot);

        auto stmt = m_allocator.emplace<NodeStmt>();

        stmt->var = super_call;

        return stmt;
    }

    optional<NodeStmt*> parse_stmt() // NOLINT(*-no-recursion)
    {
        if (auto stmt = parse_call_stmt())
        {
            return stmt;
        }

        if (auto stmt = parse_super_stmt())
        {
            return stmt;
        }

        if (auto stmt = parse_Create())
        {
            return stmt;
        }

        if (auto stmt = parse_create_stmt())
        {
            return stmt;
        }

        if (auto stmt = parse_ask_stmt())
        {
            return stmt;
        }

        if (auto stmt = parse_using())
        {
            return stmt;
        }

        if (auto stmt = parse_import())
        {
            return stmt;
        }
        if (auto stmt = parse_include())
        {
            return stmt;
        }

        if (peek().has_value() && peek().value().type == TokenType::end_ && peek(1).has_value()
            && peek(1).value().type == TokenType::open_paren) {
            consume();
            consume();
            auto stmt_quit = m_allocator.emplace<NodeStmtQuit>();
            if (const auto node_expr = parse_expr()) {
                stmt_quit->expr = node_expr.value();
            }
            else {
                error_expected("expression");
            }
            try_consume_err(TokenType::close_paren);
            try_consume_err(TokenType::dot);
            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_quit;
            return stmt;
        }
        auto declaration = peek();

        if (declaration.has_value()
            && (declaration->type == TokenType::let_
                || declaration->type == TokenType::const_
                || declaration->type == TokenType::static_
                || declaration->type == TokenType::var_))
        {
            consume();

            auto stmt_let = m_allocator.emplace<NodeStmtLet>();

            stmt_let->declaration = declaration.value();

            auto type = parse_type();

            if (!type.has_value())
                error_expected("variable type");

            stmt_let->type = type.value();

            stmt_let->ident = try_consume_err(TokenType::ident);

            if (try_consume(TokenType::to_))
            {
                auto expr = parse_expr();

                if (!expr)
                    error_expected("initial value");

                stmt_let->expr = expr.value();
            }

            try_consume_err(TokenType::dot);

            auto stmt = m_allocator.emplace<NodeStmt>();

            stmt->var = stmt_let;

            return stmt;
        }
        if (auto stmt = parse_assign_stmt())
        {
            return stmt;
        }
        if (peek().has_value() && peek().value().type == TokenType::open_curly) {
            if (auto scope = parse_scope()) {
                auto stmt = m_allocator.emplace<NodeStmt>(scope.value());
                return stmt;
            }
            error_expected("scope");
        }
        if (try_consume(TokenType::if_)) {
            auto stmt_if = m_allocator.emplace<NodeStmtIf>();
            if (const auto expr = parse_expr()) {
                stmt_if->expr = expr.value();
            }
            else {
                error_expected("expression");
            }
            if (const auto scope = parse_scope()) {
                stmt_if->scope = scope.value();
            }
            else {
                error_expected("scope");
            }
            stmt_if->pred = parse_if_pred();
            auto stmt = m_allocator.emplace<NodeStmt>(stmt_if);
            return stmt;
        }

        if (auto stmt = parse_for_stmt()){
            return stmt;
        }
        if (auto stmt = parse_choose()){
            return stmt;
        }
        if (auto stmt = parse_struct()){
            return stmt;
        }
        if (auto stmt = parse_class()){
            return stmt;
        }
        if (auto stmt = parse_enum())
        {
            return stmt;
        }
        if (auto stmt = parse_try())
        {
            return stmt;
        }
        if (auto stmt = parse_namespace()){
            return stmt;
        }

        if (auto stmt = parse_do_while_stmt())
        {
            return stmt;
        }

        if (try_consume(TokenType::while_)) {
            auto stmt_while = m_allocator.emplace<NodeStmtWhile>();
            if (const auto expr = parse_expr()) {
                stmt_while->expr = expr.value();
            }
            else {
                error_expected("expression");
            }
            if (const auto scope = parse_scope()) {
                stmt_while->scope = scope.value();
            }
            else {
                error_expected("scope");
            }
            auto stmt = m_allocator.emplace<NodeStmt>(stmt_while);
            return stmt;
        }
        if (try_consume(TokenType::show_))
        {
            auto stmt_print = m_allocator.emplace<NodeStmtPrint>();
            auto expr = parse_expr();
            if (!expr)
                error_expected("expression");
            stmt_print->exprs.push_back(expr.value());
            while (try_consume(TokenType::comma))
            {
                auto next_expr = parse_expr();
                if (!next_expr)
                    error_expected("expression after comma");

                stmt_print->exprs.push_back(next_expr.value());
            }
            try_consume_err(TokenType::dot);
            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_print;
            return stmt;
        }


        if (peek().has_value()
            && peek()->type == TokenType::give_)
        {
            consume();

            auto stmt_give = m_allocator.emplace<NodeStmtGive>();


            if (auto expr = parse_expr())
                stmt_give->expr = expr.value();
            else
                error_expected("expression");

            try_consume_err(TokenType::dot);

            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_give;

            return stmt;
        }

        if (peek().has_value()
    && peek()->type == TokenType::return_)
        {
            consume();

            auto stmt_return = m_allocator.emplace<NodeStmtReturn>();

            if (auto expr = parse_expr())
                stmt_return->expr = expr.value();
            else
                error_expected("expression");

            try_consume_err(TokenType::dot);

            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_return;

            return stmt;
        }

        if (peek().has_value()
    && peek()->type == TokenType::throw_)
        {
            consume();

            auto stmt_throw = m_allocator.emplace<NodeStmtThrow>();

            auto expr = parse_expr();

            if (!expr)
                error_expected("expression after `throw`");

            stmt_throw->expr = expr.value();

            try_consume_err(TokenType::dot);

            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_throw;

            return stmt;
        }
        if (peek().has_value()
            && peek()->type == TokenType::stop_)
        {
            consume();

            try_consume_err(TokenType::dot);

            auto stmt_stop = m_allocator.emplace<NodeStmtStop>();


            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_stop;

            return stmt;
        }

        if (peek().has_value()
            && peek()->type == TokenType::exit_)
        {
            consume();

            try_consume_err(TokenType::dot);

            auto stmt_exit = m_allocator.emplace<NodeStmtExit>();

            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_exit;

            return stmt;
        }

        if (peek().has_value()
            && peek()->type == TokenType::next_)
        {
            consume();

            try_consume_err(TokenType::dot);

            auto stmt_next = m_allocator.emplace<NodeStmtNext>();

            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_next;;

            return stmt;
        }

        if (peek().has_value()
            && peek()->type == TokenType::continue_)
        {
            consume();

            try_consume_err(TokenType::dot);

            auto stmt_continue = m_allocator.emplace<NodeStmtContinue>();

            auto stmt = m_allocator.emplace<NodeStmt>();
            stmt->var = stmt_continue;

            return stmt;
        }
        return {};
    }

    optional<NodeProg> parse_prog()
    {
        NodeProg prog;

        try_consume_err(TokenType::start_);

        while (peek().has_value()
            && peek()->type != TokenType::end_)
        {
            if (auto stmt = parse_stmt())
            {
                prog.stmts.push_back(stmt.value());
            }
            else
            {
                error_expected("statement");
            }
        }

        try_consume_err(TokenType::end_);

        return prog;
    }
private:
    [[nodiscard]] optional<Token> peek(const int offset = 0) const
    {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        }
        return m_tokens.at(m_index + offset);
    }

    Token consume()
    {
        return m_tokens.at(m_index++);
    }

    Token try_consume_err(const TokenType type)
    {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        }
        error_expected(to_string(type));
        return {};
    }

    optional<Token> try_consume(const TokenType type)
    {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        }
        return {};
    }

    const vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};

