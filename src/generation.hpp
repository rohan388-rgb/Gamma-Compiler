

#pragma once

#include <algorithm>
#include <cassert>
#include <unordered_map>

#include "parser.hpp"

using namespace std;

class Generator {
public:
    explicit Generator(NodeProg prog)
        : m_prog(move(prog))
    {
    }

    void gen_term(const NodeTerm* term)
    {
        struct TermVisitor {
            Generator& gen;

            void operator()(const NodeTermIntLit* term_int_lit) const
            {
                gen.m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen.push("rax");
            }
            void operator()(const NodeTermFloatLit* term_float_lit) const
            {
                const string value =
                    term_float_lit->float_lit.value.value();

                const string label =
                    gen.add_float(value);

                gen.m_output
                    << "    mov rax, [rel "
                    << label
                    << "]\n";

                gen.push("rax");
            }

            void operator()(const NodeTermCharLit* term_char_lit) const
            {
                const string value =
                    term_char_lit->char_lit.value.value();

                if (value.size() != 1)
                {
                    cerr << "Invalid character literal\n";
                    exit(EXIT_FAILURE);
                }

                const unsigned char character =
                    static_cast<unsigned char>(value[0]);

                gen.m_output
                    << "    mov rax, "
                    << static_cast<int>(character)
                    << "\n";

                gen.push("rax");
            }

            void operator()(const NodeTermIdent* term_ident) const
            {
                const string name =
                    term_ident->ident.value.value();

                // Check enum value first.
                auto enum_value =
                    gen.m_enum_values.find(name);

                if (enum_value != gen.m_enum_values.end())
                {
                    gen.m_output
                        << "    mov rax, "
                        << enum_value->second
                        << "\n";

                    gen.push("rax");
                    return;
                }

                // Otherwise, treat it as a normal variable.
                auto var = gen.find_var(name);

                if (!var.has_value())
                {
                    cerr << "Undeclared identifier: "
                         << name
                         << endl;
                    exit(EXIT_FAILURE);
                }

                gen.m_output
                    << "    push qword [rbp - "
                    << gen.stack_offset(**var)
                    << "]\n";

                gen.m_stack_size++;
            }

            void operator()(const NodeTermParen* term_paren) const
            {
                gen.gen_expr(term_paren->expr);
            }

            void operator()(const NodeTermBool* term_bool) const
            {
                gen.m_output << "    mov rax, "
                             << (term_bool->value ? 1 : 0)
                             << "\n";

                gen.push("rax");
            }

            void operator()(const NodeTermNull*) const
            {
                gen.m_output << "    xor rax, rax\n";
                gen.push("rax");
            }

            void operator()(const NodeTermArray* term_array) const
{
    const size_t element_count = term_array->elements.size();

    // Array layout:
    // [length][element0][element1]...[elementN]
    //
    // Each slot = 8 bytes.
    const size_t total_size = (element_count + 1) * 8;

    // mmap(addr, length, prot, flags, fd, offset)

    // addr = NULL
    gen.m_output
        << "    xor rdi, rdi\n";

    // length = total array size
    gen.m_output
        << "    mov rsi, "
        << total_size
        << "\n";

    // PROT_READ | PROT_WRITE
    gen.m_output
        << "    mov rdx, 3\n";

    // MAP_PRIVATE | MAP_ANONYMOUS
    gen.m_output
        << "    mov r10, 0x22\n";

    // fd = -1
    gen.m_output
        << "    mov r8, -1\n";

    // offset = 0
    gen.m_output
        << "    xor r9, r9\n";

    // mmap syscall
    gen.m_output
        << "    mov rax, 9\n";

    gen.m_output
        << "    syscall\n";

    // Save allocated array pointer.
    gen.m_output
        << "    mov rbx, rax\n";

    // Store array length.
    gen.m_output
        << "    mov qword [rbx], "
        << element_count
        << "\n";

    // Store elements.
    for (size_t i = 0; i < element_count; ++i)
    {
        gen.gen_expr(term_array->elements[i]);
        gen.pop("rax");

        gen.m_output
            << "    mov qword [rbx + "
            << ((i + 1) * 8)
            << "], rax\n";
    }

    // Return array pointer.
    gen.push("rbx");
}

            void operator()(const NodeTermIndex* term_index) const
            {
                // Generate index first
                gen.gen_expr(term_index->index);
                gen.pop("rcx");

                // Generate array pointer
                gen.gen_expr(term_index->array);
                gen.pop("rax");

                // Address = array + 8 + index * 8
                gen.m_output << "    lea rax, [rax + rcx * 8 + 8]\n";

                // Load element
                gen.m_output << "    mov rax, [rax]\n";

                gen.push("rax");
            }

            void operator()(const NodeTermFieldAccess* field_access) const
{
    const auto* object =
        field_access->object;

    auto term =
        std::get_if<NodeTerm*>(&object->var);

    if (term == nullptr || *term == nullptr)
    {
        cerr << "Invalid struct object\n";
        exit(EXIT_FAILURE);
    }

    auto ident =
        std::get_if<NodeTermIdent*>(&(*term)->var);

    if (ident == nullptr || *ident == nullptr)
    {
        cerr << "Struct field access requires an object variable\n";
        exit(EXIT_FAILURE);
    }

    const string object_name =
        (*ident)->ident.value.value();

    auto var =
        gen.find_var(object_name);

    if (!var.has_value())
    {
        cerr << "Undeclared identifier: "
             << object_name
             << endl;

        exit(EXIT_FAILURE);
    }

    if ((*var)->type != ValueType::Struct)
    {
        cerr << object_name
             << " is not a struct\n";

        exit(EXIT_FAILURE);
    }

    const string struct_name =
        (*var)->type_name;

    auto struct_it =
        gen.m_structs.find(struct_name);

    if (struct_it == gen.m_structs.end())
    {
        cerr << "Unknown struct type: "
             << struct_name
             << endl;

        exit(EXIT_FAILURE);
    }

    const string field_name =
        field_access->field.value.value();

    auto field_it =
        struct_it->second.field_offsets.find(field_name);

    if (field_it == struct_it->second.field_offsets.end())
    {
        cerr << "Unknown field "
             << field_name
             << " in struct "
             << struct_name
             << endl;

        exit(EXIT_FAILURE);
    }

    const size_t offset =
        field_it->second;

    gen.m_output
        << "    mov rax, [rbp - "
        << gen.stack_offset(**var)
        << "]\n";

    if (offset != 0)
    {
        gen.m_output
            << "    add rax, "
            << offset
            << "\n";
    }

    gen.m_output
        << "    mov rax, [rax]\n";

    gen.push("rax");
}

            void operator()(const NodeTermLength* term_length) const
            {
                gen.gen_expr(term_length->expr);

                gen.pop("rax");

                // First 8 bytes of an array store its length.
                gen.m_output << "    mov rax, [rax]\n";

                gen.push("rax");
            }

            void operator()(const NodeTermSizeOf* term_sizeof) const
            {
                // Generate the expression first.
                gen.gen_expr(term_sizeof->expr);

                gen.pop("rax");

                // Current runtime values occupy one 8-byte slot.
                gen.m_output << "    mov rax, 8\n";

                gen.push("rax");
            }

            void operator()(const NodeTermTypeOf* term_typeof) const
            {

                gen.gen_expr(term_typeof->expr);
                gen.pop("rax");

                // TODO:

                gen.m_output << "    xor rax, rax\n";

                gen.push("rax");
            }

            void operator()(const NodeTermContains* term_contains) const
            {
                gen.gen_expr(term_contains->value);
                gen.pop("rsi");

                gen.gen_expr(term_contains->text);
                gen.pop("rdi");

                gen.m_output << "    call string_contains\n";

                gen.push("rax");
            }

            void operator()(const NodeTermStartsWith* term_starts_with) const
            {
                gen.gen_expr(term_starts_with->value);
                gen.pop("rsi");

                gen.gen_expr(term_starts_with->text);
                gen.pop("rdi");

                gen.m_output << "    call string_starts_with\n";

                gen.push("rax");
            }

            void operator()(const NodeTermEndsWith* term_ends_with) const
            {
                gen.gen_expr(term_ends_with->value);
                gen.pop("rsi");

                gen.gen_expr(term_ends_with->text);
                gen.pop("rdi");

                gen.m_output << "    call string_ends_with\n";

                gen.push("rax");
            }

            void operator()(const NodeTermTrim* term_trim) const
            {
                gen.gen_expr(term_trim->expr);

                gen.pop("rdi");

                gen.m_output << "    call string_trim\n";

                gen.push("rax");
            }

            void operator()(const NodeTermUppercase* term_uppercase) const
            {
                gen.gen_expr(term_uppercase->expr);
                gen.pop("rdi");

                gen.m_output << "    call string_uppercase\n";

                gen.push("rax");
            }

            void operator()(const NodeTermLowercase* term_lowercase) const
            {
                gen.gen_expr(term_lowercase->expr);
                gen.pop("rdi");

                gen.m_output << "    call string_lowercase\n";

                gen.push("rax");
            }

            void operator()(const NodeTermReplace* term_replace) const
            {
                gen.gen_expr(term_replace->new_value);
                gen.pop("rdx");

                gen.gen_expr(term_replace->old_value);
                gen.pop("rsi");

                gen.gen_expr(term_replace->text);
                gen.pop("rdi");

                gen.m_output << "    call string_replace\n";

                gen.push("rax");
            }

            void operator()(const NodeTermSubstring* term_substring) const
            {
                gen.gen_expr(term_substring->length);
                gen.pop("rdx");

                gen.gen_expr(term_substring->start);
                gen.pop("rsi");

                gen.gen_expr(term_substring->text);
                gen.pop("rdi");

                gen.m_output << "    call string_substring\n";

                gen.push("rax");
            }

            void operator()(const NodeTermSplit* term_split) const
            {
                gen.gen_expr(term_split->delimiter);
                gen.pop("rsi");

                gen.gen_expr(term_split->text);
                gen.pop("rdi");

                gen.m_output << "    call string_split\n";

                gen.push("rax");
            }

            void operator()(const NodeTermJoin* term_join) const
            {
                gen.gen_expr(term_join->delimiter);
                gen.pop("rsi");

                gen.gen_expr(term_join->array);
                gen.pop("rdi");

                gen.m_output << "    call string_join\n";

                gen.push("rax");
            }

            void operator()(const NodeTermAddressOf* term_address_of) const
            {
                const auto* expr = term_address_of->expr;

                auto term_ptr = std::get_if<NodeTerm*>(&expr->var);

                if (term_ptr == nullptr || *term_ptr == nullptr)
                {
                    cerr << "Address-of requires a variable\n";
                    exit(EXIT_FAILURE);
                }

                auto ident =
                    std::get_if<NodeTermIdent*>(&(*term_ptr)->var);

                if (ident == nullptr || *ident == nullptr)
                {
                    cerr << "Address-of requires a variable\n";
                    exit(EXIT_FAILURE);
                }

                auto var =
                    gen.find_var((*ident)->ident.value.value());

                if (!var.has_value())
                {
                    cerr << "Undeclared identifier: "
                         << (*ident)->ident.value.value()
                         << endl;
                    exit(EXIT_FAILURE);
                }

                gen.m_output
                    << "    lea rax, [rbp - "
                    << gen.stack_offset(**var)
                    << "]\n";

                gen.push("rax");
            }

            void operator()(const NodeTermDereference* term_dereference) const
            {
                gen.gen_expr(term_dereference->expr);

                gen.pop("rax");

                gen.m_output << "    mov rax, [rax]\n";

                gen.push("rax");
            }

            void operator()(const NodeTermPointer* term_pointer) const
            {
                gen.gen_expr(term_pointer->expr);

                // Pointer expression already evaluates to an address.
                // Keep that address as the result.
            }

            void operator()(const NodeTermAllocate* term_allocate) const
            {
                gen.gen_expr(term_allocate->expr);
                gen.pop("rsi");

                gen.m_output << "    add rsi, 8\n";
                gen.m_output << "    push rsi\n";

                gen.m_output << "    xor rdi, rdi\n";
                gen.m_output << "    mov rdx, 3\n";
                gen.m_output << "    mov r10, 0x22\n";
                gen.m_output << "    mov r8, -1\n";
                gen.m_output << "    xor r9, r9\n";
                gen.m_output << "    mov rax, 9\n";
                gen.m_output << "    syscall\n";

                gen.m_output << "    pop rdx";

                gen.m_output << "    mov qword [rax], rdx\n";
                gen.m_output << "    add rax, 8\n";

                gen.push("rax");
            }

            void operator()(const NodeTermFree* term_free) const
            {
                gen.gen_expr(term_free->expr);
                gen.pop("rdi");

                gen.m_output << "    sub rdi, 8\n";
                gen.m_output << "    mov rsi, [rdi]\n";

                gen.m_output << "    mov rax, 11\n";
                gen.m_output << "    syscall\n";

                gen.push("rax");
            }

            void operator()(const NodeTermNew* term_new) const
            {
                gen.gen_expr(term_new->expr);
                gen.pop("rsi");

                gen.m_output << "    add rsi, 8\n";
                gen.m_output << "    push rsi\n";

                gen.m_output << "    xor rdi, rdi\n";
                gen.m_output << "    mov rdx, 3\n";
                gen.m_output << "    mov r10, 0x22\n";
                gen.m_output << "    mov r8, -1\n";
                gen.m_output << "    xor r9, r9\n";
                gen.m_output << "    mov rax, 9\n";
                gen.m_output << "    syscall\n";

                gen.m_output << "    pop rdx\n";
                gen.m_output << "    mov qword [rax], rdx\n";
                gen.m_output << "    add rax, 8\n";

                gen.push("rax");
            }


            void operator()(const NodeTermDelete* term_delete) const
            {
                gen.gen_expr(term_delete->expr);
                gen.pop("rdi");

                gen.m_output << "    sub rdi, 8\n";
                gen.m_output << "    mov rsi, [rdi]\n";

                gen.m_output << "    mov rax, 11\n";
                gen.m_output << "    syscall\n";

                gen.push("rax");
            }

            void operator()(const NodeTermThis*) const
            {
                // RDI will hold the current object address
                // when class methods are implemented.
                gen.m_output << "    mov rax, rdi\n";

                gen.push("rax");
            }

            void operator()(const NodeTermObject* term_object) const
            {
                gen.gen_expr(term_object->expr);

                // The expression already evaluates to the object address.
                // Keep that address as the result.
            }

            void operator()(const NodeTermRead*) const
            {
                // Read an integer from standard input.
                gen.m_output << "    call read_int\n";

                gen.push("rax");
            }
            void operator()(const NodeTermPrint*) const
            {
                // `print` has no operand in the current AST.
                // Keep it as a zero-valued expression until
                // the language gives this term an explicit argument.
                gen.m_output << "    xor rax, rax\n";

                gen.push("rax");
            }
            void operator()(const NodeStmtAsk* stmt_ask) const
            {
                // Show prompt if one was provided.
                if (stmt_ask->prompt)
                {
                    gen.gen_expr(stmt_ask->prompt);
                    gen.pop("rax");

                    gen.m_output << "    call print_string\n";
                }

                auto var = gen.find_var(stmt_ask->ident.value.value());

                if (!var.has_value())
                {
                    cerr << "Undeclared identifier: "
                         << stmt_ask->ident.value.value()
                         << endl;
                    exit(EXIT_FAILURE);
                }

                switch ((*var)->type)
                {
                    case ValueType::Number:
                        gen.m_output << "    call read_int\n";
                        break;

                    case ValueType::Decimal:
                        gen.m_output << "    call read_decimal\n";
                        break;

                    case ValueType::Text:
                        gen.m_output << "    call read_string\n";
                        break;

                    default:
                        cerr << "Ask is not supported for this variable type\n";
                        exit(EXIT_FAILURE);
                }

                gen.m_output
                    << "    mov qword [rbp - "
                    << gen.stack_offset(**var)
                    << "], rax\n";
            }

            void operator()(const NodeTermString* term_string) const
            {

                string label =
                    gen.add_string(term_string->string.value.value());

                gen.m_output
                    << "    lea rax, [rel "
                    << label
                    << "]\n";

                gen.push("rax");
            }

            void operator()(const NodeCreateFunctionCall* call) const
            {
                static const char* arg_registers[] =
                {
                    "rdi",
                    "rsi",
                    "rdx",
                    "rcx",
                    "r8",
                    "r9"
                };

                if (call->args.size() > 6)
                {
                    cerr << "Functions with more than 6 arguments are not supported yet\n";
                    exit(EXIT_FAILURE);
                }

                for (size_t i = 0; i < call->args.size(); ++i)
                {
                    gen.gen_expr(call->args[i]);
                    gen.pop(arg_registers[i]);
                }

                gen.m_output
                    << "    call "
                    << call->name.value.value()
                    << "\n";

                gen.push("rax");
            }
        };
        TermVisitor visitor({ .gen = *this });
        visit(visitor, term->var);
    }

    void gen_decimal_assign_op(const string& op)
    {
        m_output << "    movq xmm0, rdx\n";
        m_output << "    movq xmm1, rax\n";

        if (op == "add")
            m_output << "    addsd xmm0, xmm1\n";
        else if (op == "sub")
            m_output << "    subsd xmm0, xmm1\n";
        else if (op == "mul")
            m_output << "    mulsd xmm0, xmm1\n";
        else if (op == "div")
            m_output << "    divsd xmm0, xmm1\n";

        else if (op == "and" || op == "or")
        {
            m_output << "    xorpd xmm2, xmm2\n";

            m_output << "    ucomisd xmm0, xmm2\n";
            m_output << "    setne al\n";

            m_output << "    ucomisd xmm1, xmm2\n";
            m_output << "    setne bl\n";

            if (op == "and")
                m_output << "    and al, bl\n";
            else
                m_output << "    or al, bl\n";

            m_output << "    movzx rax, al\n";
            push("rax");
            return;
        }
        else if (op == "rem")
        {
            m_output << "    sub rsp, 16\n";
            m_output << "    mov [rsp], rdx\n";
            m_output << "    mov [rsp + 8], rax\n";

            m_output << "    fld qword [rsp + 8]\n";
            m_output << "    fld qword [rsp]\n";

            string label = create_label();

            m_output << label << ":\n";
            m_output << "    fprem\n";
            m_output << "    fnstsw ax\n";
            m_output << "    test ah, 4\n";
            m_output << "    jnz " << label << "\n";

            m_output << "    fstp qword [rsp]\n";
            m_output << "    fstp qword [rsp + 8]\n";

            m_output << "    mov rdx, [rsp + 8]\n";
            m_output << "    add rsp, 16\n";
        }

        m_output << "    movq rdx, xmm0\n";
    }
    void gen_decimal_bin_op(const NodeExpr* lhs, const NodeExpr* rhs, const string& op)
{
    gen_expr(rhs);
    gen_expr(lhs);

    pop("rax");
    pop("rbx");

    if (op == "rem")
    {
        m_output << "    sub rsp, 16\n";
        m_output << "    mov [rsp], rax\n";
        m_output << "    mov [rsp + 8], rbx\n";
        m_output << "    fld qword [rsp + 8]\n";
        m_output << "    fld qword [rsp]\n";

        string label = create_label();
        m_output << label << ":\n";
        m_output << "    fprem\n";
        m_output << "    fnstsw ax\n";
        m_output << "    test ah, 4\n";
        m_output << "    jnz " << label << "\n";

        m_output << "    fstp qword [rsp]\n";
        m_output << "    fstp qword [rsp + 8]\n";
        m_output << "    mov rax, [rsp]\n";
        m_output << "    add rsp, 16\n";

        push("rax");
        return;
    }

    m_output << "    movq xmm0, rax\n";
    m_output << "    movq xmm1, rbx\n";

    if (op == "add")
        m_output << "    addsd xmm0, xmm1\n";
    else if (op == "sub")
        m_output << "    subsd xmm0, xmm1\n";
    else if (op == "mul")
        m_output << "    mulsd xmm0, xmm1\n";
    else if (op == "div")
        m_output << "    divsd xmm0, xmm1\n";
    else
    {
        m_output << "    ucomisd xmm0, xmm1\n";

        if (op == "gt")
            m_output << "    seta al\n";
        else if (op == "lt")
            m_output << "    setb al\n";
        else if (op == "gte")
            m_output << "    setae al\n";
        else if (op == "lte")
            m_output << "    setbe al\n";
        else if (op == "eq")
            m_output << "    sete al\n";
        else if (op == "neq")
            m_output << "    setne al\n";

        m_output << "    movzx rax, al\n";
        push("rax");
        return;
    }

    m_output << "    movq rax, xmm0\n";
    push("rax");
}

    void gen_bin_expr(const NodeBinExpr* bin_expr)
    {
        struct BinExprVisitor {
            Generator& gen;

            void operator()(const NodeBinExprSub* sub) const
            {
                if (gen.has_decimal_expr(sub->lhs) || gen.has_decimal_expr(sub->rhs))
                {
                    gen.gen_decimal_bin_op(sub->lhs, sub->rhs, "sub");
                    return;
                }
                gen.gen_expr(sub->rhs);
                gen.gen_expr(sub->lhs);
                gen.pop("rax");
                gen.pop("rbx");
                gen.m_output << "    sub rax, rbx\n";
                gen.push("rax");
            }

            void operator()(const NodeBinExprAdd* add) const
            {
                if (gen.has_decimal_expr(add->lhs) || gen.has_decimal_expr(add->rhs))
                {
                    gen.gen_decimal_bin_op(add->lhs, add->rhs, "add");
                    return;
                }
                gen.gen_expr(add->rhs);
                gen.gen_expr(add->lhs);
                gen.pop("rax");
                gen.pop("rbx");
                gen.m_output << "    add rax, rbx\n";
                gen.push("rax");
            }

            void operator()(const NodeBinExprMulti* multi) const
            {
                if (gen.has_decimal_expr(multi->lhs) || gen.has_decimal_expr(multi->rhs))
                {
                    gen.gen_decimal_bin_op(multi->lhs, multi->rhs, "mul");
                    return;
                }
                gen.gen_expr(multi->rhs);
                gen.gen_expr(multi->lhs);
                gen.pop("rax");
                gen.pop("rbx");
                gen.m_output << "    mul rbx\n";
                gen.push("rax");
            }

            void operator()(const NodeBinExprDiv* div) const
            {
                if (gen.has_decimal_expr(div->lhs) || gen.has_decimal_expr(div->rhs))
                {
                    gen.gen_decimal_bin_op(div->lhs, div->rhs, "div");
                    return;
                }
                gen.gen_expr(div->rhs);
                gen.gen_expr(div->lhs);
                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    xor rdx, rdx\n";
                gen.m_output << "    div rbx\n";
                gen.push("rax");
            }

            void operator()(const NodeBinExprRemainder* rem) const
            {
                if (gen.has_decimal_expr(rem->lhs) || gen.has_decimal_expr(rem->rhs))
                {
                    gen.gen_decimal_bin_op(rem->lhs, rem->rhs, "rem");
                    return;
                }
                gen.gen_expr(rem->rhs);
                gen.gen_expr(rem->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    xor rdx, rdx\n";
                gen.m_output << "    div rbx\n";

                gen.push("rdx");
            }

            void operator()(const NodeBinExprGT* gt) const
            {
                if (gen.has_decimal_expr(gt->lhs) || gen.has_decimal_expr(gt->rhs))
                {
                    gen.gen_decimal_bin_op(gt->lhs, gt->rhs, "gt");
                    return;
                }
                gen.gen_expr(gt->rhs);
                gen.gen_expr(gt->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    cmp rax, rbx\n";
                gen.m_output << "    setg al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }
            void operator()(const NodeBinExprLT* lt) const
            {
                if (gen.has_decimal_expr(lt->lhs) || gen.has_decimal_expr(lt->rhs))
                {
                    gen.gen_decimal_bin_op(lt->lhs, lt->rhs, "lt");
                    return;
                }
                gen.gen_expr(lt->rhs);
                gen.gen_expr(lt->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    cmp rax, rbx\n";
                gen.m_output << "    setl al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }
            void operator()(const NodeBinExprGTE* gte) const
            {
                if (gen.has_decimal_expr(gte->lhs) || gen.has_decimal_expr(gte->rhs))
                {
                    gen.gen_decimal_bin_op(gte->lhs, gte->rhs, "gte");
                    return;
                }
                gen.gen_expr(gte->rhs);
                gen.gen_expr(gte->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    cmp rax, rbx\n";
                gen.m_output << "    setge al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }
            void operator()(const NodeBinExprLTE* lte) const
            {
                if (gen.has_decimal_expr(lte->lhs) || gen.has_decimal_expr(lte->rhs))
                {
                    gen.gen_decimal_bin_op(lte->lhs, lte->rhs, "lte");
                    return;
                }
                gen.gen_expr(lte->rhs);
                gen.gen_expr(lte->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    cmp rax, rbx\n";
                gen.m_output << "    setle al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }
            void operator()(const NodeBinExprEQ* eq) const
            {
                if (gen.has_decimal_expr(eq->lhs) || gen.has_decimal_expr(eq->rhs))
                {
                    gen.gen_decimal_bin_op(eq->lhs, eq->rhs, "eq");
                    return;
                }
                gen.gen_expr(eq->rhs);
                gen.gen_expr(eq->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    cmp rax, rbx\n";
                gen.m_output << "    sete al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }
            void operator()(const NodeBinExprNEQ* neq) const
            {
                if (gen.has_decimal_expr(neq->lhs) || gen.has_decimal_expr(neq->rhs))
                {
                    gen.gen_decimal_bin_op(neq->lhs, neq->rhs, "neq");
                    return;
                }
                gen.gen_expr(neq->rhs);
                gen.gen_expr(neq->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    cmp rax, rbx\n";
                gen.m_output << "    setne al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }
            void operator()(const NodeBinExprAND* andExpr) const
            {
                if (gen.has_decimal_expr(andExpr->lhs) ||gen.has_decimal_expr(andExpr->rhs))
                {
                    gen.gen_decimal_bin_op(andExpr->lhs, andExpr->rhs, "and");
                    return;
                }
                gen.gen_expr(andExpr->rhs);
                gen.gen_expr(andExpr->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    and rax, rbx\n";
                gen.m_output << "    cmp rax, 0\n";
                gen.m_output << "    setne al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }

            void operator()(const NodeBinExprBitAnd* bitAnd) const
            {
                gen.gen_expr(bitAnd->rhs);
                gen.gen_expr(bitAnd->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    and rax, rbx\n";

                gen.push("rax");
            }

            void operator()(const NodeBinExprBitOr* bitOr) const
            {
                gen.gen_expr(bitOr->rhs);
                gen.gen_expr(bitOr->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    or rax, rbx\n";

                gen.push("rax");
            }

            void operator()(const NodeBinExprShl* shl) const
            {
                gen.gen_expr(shl->rhs);
                gen.gen_expr(shl->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    mov rcx, rax\n";
                gen.m_output << "    mov rax, rbx\n";
                gen.m_output << "    shl rax, cl\n";

                gen.push("rax");
            }

            void operator()(const NodeBinExprShr* shr) const
            {
                gen.gen_expr(shr->rhs);
                gen.gen_expr(shr->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    mov rcx, rax\n";
                gen.m_output << "    mov rax, rbx\n";
                gen.m_output << "    shr rax, cl\n";

                gen.push("rax");
            }

            void operator()(const NodeBinExprBitXor* bitXor) const
            {
                gen.gen_expr(bitXor->rhs);
                gen.gen_expr(bitXor->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    xor rax, rbx\n";

                gen.push("rax");
            }

            void operator()(const NodeBinExprOR* orExpr) const
            {
                if (gen.has_decimal_expr(orExpr->lhs) ||
                    gen.has_decimal_expr(orExpr->rhs))
                {
                    gen.gen_decimal_bin_op(orExpr->lhs, orExpr->rhs, "or");
                    return;
                }

                gen.gen_expr(orExpr->rhs);
                gen.gen_expr(orExpr->lhs);

                gen.pop("rax");
                gen.pop("rbx");

                gen.m_output << "    or rax, rbx\n";
                gen.m_output << "    cmp rax, 0\n";
                gen.m_output << "    setne al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }
        };

        BinExprVisitor visitor { .gen = *this };
        visit(visitor, bin_expr->var);
    }

    void gen_expr(const NodeExpr* expr)
    {
        struct ExprVisitor {
            Generator& gen;

            void operator()(const NodeTerm* term) const
            {
                gen.gen_term(term);
            }

            void operator()(const NodeBinExpr* bin_expr) const
            {
                gen.gen_bin_expr(bin_expr);
            }

            void operator()(const NodeQuestion* question) const
            {
                const string false_label = gen.create_label();
                const string end_label = gen.create_label();

                // Generate condition
                gen.gen_expr(question->condition);
                gen.pop("rax");

                gen.m_output << "    test rax, rax\n";
                gen.m_output << "    jz " << false_label << "\n";

                // True expression
                gen.gen_expr(question->true_expr);
                gen.m_output << "    jmp " << end_label << "\n";

                // False expression
                gen.m_output << false_label << ":\n";
                gen.gen_expr(question->false_expr);

                gen.m_output << end_label << ":\n";
            }

            void operator()(const NodeArrow* arrow) const
            {
                gen.gen_expr(arrow->lhs);
                gen.pop("rax");

                gen.gen_expr(arrow->rhs);
            }
            void operator()(const NodeDoubleColon* double_colon) const
            {
                gen.gen_expr(double_colon->lhs);
                gen.pop("rax");

                gen.gen_expr(double_colon->rhs);
            }

            void operator()(const NodeUnaryBitNot* bit_not) const
            {
                gen.gen_expr(bit_not->expr);

                gen.pop("rax");

                gen.m_output << "    not rax\n";

                gen.push("rax");
            }

            void operator()(const NodeUnaryNot* unary_not) const
            {
                gen.gen_expr(unary_not->expr);

                gen.pop("rax");

                gen.m_output << "    test rax, rax\n";
                gen.m_output << "    sete al\n";
                gen.m_output << "    movzx rax, al\n";

                gen.push("rax");
            }
        };

        ExprVisitor visitor { .gen = *this };
        visit(visitor, expr->var);
    }

    void gen_scope(const NodeScope* scope)
    {
        begin_scope();
        for (const NodeStmt* stmt : scope->stmts) {
            gen_stmt(stmt);
        }
        end_scope();
    }

    void gen_if_pred(const NodeIfPred* pred, const string& end_label)
    {
        struct PredVisitor {
            Generator& gen;
            const string& end_label;

            void operator()(const NodeIfPredOrWhen* orwhen) const
            {
                gen.m_output << "    ;;  orwhen\n";
                gen.gen_expr( orwhen->expr);
                gen.pop("rax");
                const string label = gen.create_label();
                gen.m_output << "    test rax, rax\n";
                gen.m_output << "    jz " << label << "\n";
                gen.gen_scope( orwhen->scope);
                gen.m_output << "    jmp " << end_label << "\n";
                if ( orwhen->pred.has_value()) {
                    gen.m_output << label << ":\n";
                    gen.gen_if_pred( orwhen->pred.value(), end_label);
                }
            }

            void operator()(const NodeIfPredOtherwise* otherwise) const
            {
                gen.m_output << "     ;; otherwise\n";
                gen.gen_scope(otherwise->scope);
            }
        };

        PredVisitor visitor { .gen = *this, .end_label = end_label };
        visit(visitor, pred->var);
    }

    void gen_stmt(const NodeStmt* stmt)
    {
        struct StmtVisitor {
            Generator& gen;

            void operator()(const NodeStmtQuit* stmt_quit) const
            {
                gen.m_output << "    ;; quit\n";
                gen.gen_expr(stmt_quit->expr);
                gen.m_output << "    mov rax, 60\n";
                gen.pop("rdi");
                gen.m_output << "    syscall\n";
                gen.m_output << "    ;; /quit\n";
            }

            void operator()(const NodeStmtLet* stmt_let) const
            {
                gen.m_output << "    ;; let\n";

                const string name = stmt_let->ident.value.value();

                // Check duplicate variable in current visible scope
                if (ranges::find_if(
                        as_const(gen.m_vars),
                        [&](const Var& var)
                        {
                            return var.name == name;
                        })
                    != gen.m_vars.cend())
                {
                    cerr << "Identifier already used: "
                         << name
                         << endl;
                    exit(EXIT_FAILURE);
                }

                // Determine variable type
                ValueType type = ValueType::Unknown;
                ValueType element_type = ValueType::Unknown;

                switch (stmt_let->type.type)
                {
                    case TokenType::number_:
                        type = ValueType::Number;
                        break;

                    case TokenType::decimal_:
                        type = ValueType::Decimal;
                        break;

                    case TokenType::text_:
                        type = ValueType::Text;
                        break;

                    case TokenType::boolean_:
                        type = ValueType::Boolean;
                        break;

                    case TokenType::character_:
                        type = ValueType::Character;
                        break;

                    case TokenType::array_:
                        type = ValueType::Array;
                        break;

                    case TokenType::pointer_:
                        type = ValueType::Pointer;
                        break;

                    default:
                    {
                        const string custom_type =
                            stmt_let->type.value.value();

                        if (gen.m_structs.contains(custom_type))
                        {
                            type = ValueType::Struct;
                        }
                        else
                        {
                            cerr << "Unknown type: "
                                 << custom_type
                                 << endl;

                            exit(EXIT_FAILURE);
                        }

                        break;
                    }
                }

                // Generate initial value
                    if (type == ValueType::Struct &&
                        stmt_let->expr &&
                        gen.is_struct_expr(stmt_let->expr))
                    {
                        // Use the struct pointer returned by the expression/function.
                        gen.gen_expr(stmt_let->expr);
                    }
                    else if (type == ValueType::Struct)
                    {
                        const string struct_name =
                            stmt_let->type.value.value();

                        const auto& info =
                            gen.m_structs.at(struct_name);

                        const string alloc_failed_label =
                            gen.create_label();

                        const string alloc_done_label =
                            gen.create_label();

                        // mmap(addr, length, prot, flags, fd, offset)

                        // addr = NULL
                        gen.m_output
                            << "    xor rdi, rdi\n";

                        // length = struct size
                        gen.m_output
                            << "    mov rsi, "
                            << info.size
                            << "\n";

                        // PROT_READ | PROT_WRITE
                        gen.m_output
                            << "    mov rdx, 3\n";

                        // MAP_PRIVATE | MAP_ANONYMOUS
                        gen.m_output
                            << "    mov r10, 0x22\n";

                        // fd = -1
                        gen.m_output
                            << "    mov r8, -1\n";

                        // offset = 0
                        gen.m_output
                            << "    xor r9, r9\n";

                        // syscall number = mmap
                        gen.m_output
                            << "    mov rax, 9\n";

                        gen.m_output
                            << "    syscall\n";

                        // Check mmap failure
                        gen.m_output
                            << "    cmp rax, -1\n";

                        gen.m_output
                            << "    je "
                            << alloc_failed_label
                            << "\n";

                        // Initialize all struct fields to 0.
                        for (size_t offset = 0;
                             offset < info.size;
                             offset += 8)
                        {
                            gen.m_output
                                << "    mov qword [rax + "
                                << offset
                                << "], 0\n";
                        }

                        // Store struct pointer as variable value.
                        gen.push("rax");

                        gen.m_output
                            << "    jmp "
                            << alloc_done_label
                            << "\n";

                        // mmap failed
                        gen.m_output
                            << alloc_failed_label
                            << ":\n";

                        gen.m_output
                            << "    mov rax, 60\n";

                        gen.m_output
                            << "    mov rdi, 1\n";

                        gen.m_output
                            << "    syscall\n";

                        gen.m_output
                            << alloc_done_label
                            << ":\n";
                    }
                    else if (stmt_let->expr)
                    {
                        gen.gen_expr(stmt_let->expr);

                        // Infer array element type from the initializer.
                        if (type == ValueType::Array)
                        {
                            if (const auto* term =
                                    get_if<NodeTerm*>(&stmt_let->expr->var))
                            {
                                if (*term != nullptr)
                                {
                                    if (holds_alternative<NodeTermSplit*>(
                                            (*term)->var))
                                    {
                                        element_type = ValueType::Text;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        gen.push("0");
                    }

                // Store variable information
                gen.m_vars.push_back({
                .name = name,
                .type = type,
                .element_type = element_type,
                .stack_loc = gen.m_stack_size - 1,
                .type_name =
                    type == ValueType::Struct
                        ? stmt_let->type.value.value()
                        : "",
                .is_parameter = false,
                .is_constant =
                    stmt_let->declaration.type == TokenType::const_
            });

                gen.m_output << "    ;; /let\n";
            }
            void operator()(const NodeStmtAssign* stmt_assign) const
            {
                if (stmt_assign->target_expr != nullptr)
                {
                    gen.gen_expr(stmt_assign->expr);
                    gen.pop("rax");

                    gen.gen_expr(stmt_assign->target_expr);
                    gen.pop("rbx");

                    if (stmt_assign->op.type == TokenType::assign_
                        || stmt_assign->op.type == TokenType::to_)
                    {
                        gen.m_output << "    mov qword [rbx], rax\n";
                        return;
                    }

                    gen.m_output << "    mov rdx, [rbx]\n";

                    switch (stmt_assign->op.type)
                    {
                        case TokenType::plus_assign_:
                            gen.m_output << "    add rdx, rax\n";
                            break;

                        case TokenType::minus_assign_:
                            gen.m_output << "    sub rdx, rax\n";
                            break;

                        case TokenType::multiply_assign_:
                            gen.m_output << "    imul rdx, rax\n";
                            break;

                        case TokenType::divide_assign_:
                            gen.m_output << "    mov r8, rax\n";
                            gen.m_output << "    mov rax, rdx\n";
                            gen.m_output << "    cqo\n";
                            gen.m_output << "    idiv r8\n";
                            gen.m_output << "    mov rdx, rax\n";
                            break;

                        case TokenType::mod_assign_:
                            gen.m_output << "    mov r8, rax\n";
                            gen.m_output << "    mov rax, rdx\n";
                            gen.m_output << "    cqo\n";
                            gen.m_output << "    idiv r8\n";
                            gen.m_output << "    mov rdx, rdx\n";
                            break;

                        default:
                            cerr << "Unsupported assignment operator for dereferenced pointer\n";
                            exit(EXIT_FAILURE);
                    }

                    gen.m_output << "    mov qword [rbx], rdx\n";
                    return;
                }
            const string name = stmt_assign->ident.value.value();

            auto var = gen.find_var(name);

                if (stmt_assign->has_field)
{
    if ((*var)->type != ValueType::Struct)
    {
        cerr << name
             << " is not a struct\n";

        exit(EXIT_FAILURE);
    }

    const string struct_name =
        (*var)->type_name;

    auto struct_it =
        gen.m_structs.find(struct_name);

    if (struct_it == gen.m_structs.end())
    {
        cerr << "Unknown struct type: "
             << struct_name
             << endl;

        exit(EXIT_FAILURE);
    }

    const string field_name =
        stmt_assign->field.value.value();

    auto field_it =
        struct_it->second.field_offsets.find(field_name);

    if (field_it == struct_it->second.field_offsets.end())
    {
        cerr << "Unknown field "
             << field_name
             << " in struct "
             << struct_name
             << endl;

        exit(EXIT_FAILURE);
    }

    const size_t offset =
        field_it->second;

    // Generate new value.
                    // Infer and remember the field type from the assigned expression.
                    if (gen.is_text_expr(stmt_assign->expr))
                    {
                        struct_it->second.field_types[field_name] =
                            ValueType::Text;
                    }
                    else if (gen.is_decimal_expr(stmt_assign->expr))
                    {
                        struct_it->second.field_types[field_name] =
                            ValueType::Decimal;
                    }
                    else
                    {
                        struct_it->second.field_types[field_name] =
                            ValueType::Number;
                    }

                    gen.gen_expr(stmt_assign->expr);
                    gen.pop("rax");

    // Load object address.
    gen.m_output
        << "    mov rbx, [rbp - "
        << gen.stack_offset(**var)
        << "]\n";

    if (offset != 0)
    {
        gen.m_output
            << "    add rbx, "
            << offset
            << "\n";
    }

    if (stmt_assign->op.type == TokenType::assign_
        || stmt_assign->op.type == TokenType::to_)
    {
        gen.m_output
            << "    mov qword [rbx], rax\n";

        return;
    }

    // Compound assignment.
    gen.m_output
        << "    mov rdx, [rbx]\n";

    switch (stmt_assign->op.type)
    {
        case TokenType::plus_assign_:
            gen.m_output
                << "    add rdx, rax\n";
            break;

        case TokenType::minus_assign_:
            gen.m_output
                << "    sub rdx, rax\n";
            break;

        case TokenType::multiply_assign_:
            gen.m_output
                << "    imul rdx, rax\n";
            break;

        case TokenType::divide_assign_:
            gen.m_output
                << "    mov r8, rax\n";
            gen.m_output
                << "    mov rax, rdx\n";
            gen.m_output
                << "    cqo\n";
            gen.m_output
                << "    idiv r8\n";
            gen.m_output
                << "    mov rdx, rax\n";
            break;

        case TokenType::mod_assign_:
            gen.m_output
                << "    mov r8, rax\n";
            gen.m_output
                << "    mov rax, rdx\n";
            gen.m_output
                << "    cqo\n";
            gen.m_output
                << "    idiv r8\n";
            gen.m_output
                << "    mov rdx, rdx\n";
            break;

        default:
            cerr
                << "Unsupported struct field assignment\n";
            exit(EXIT_FAILURE);
    }

    gen.m_output
        << "    mov qword [rbx], rdx\n";

    return;
}

            if (!var.has_value())
            {
                cerr << "Undeclared identifier: "
                     << name
                     << endl;
                exit(EXIT_FAILURE);
            }

            if ((*var)->is_constant)
            {
                cerr << "Cannot assign to constant variable: "
                     << name
                     << endl;
                exit(EXIT_FAILURE);
            }

            /*
             * ---------------------------------------------------------
             * Array element assignment
             *
             * Example:
             * Set numbers[2] to 50.
             * Set numbers[2] plus_assign 5.
             * ---------------------------------------------------------
             */
            if (stmt_assign->index != nullptr)
            {
                // Generate new value first.
                gen.gen_expr(stmt_assign->expr);
                gen.pop("rax");

                // Preserve the value because generating the index
                // will use RAX.
                gen.m_output << "    mov r12, rax\n";

                // Generate index.
                gen.gen_expr(stmt_assign->index);
                gen.pop("rcx");

                // Get array address.
                gen.m_output
                    << "    mov rbx, [rbp - "
                    << gen.stack_offset(**var)
                    << "]\n";

                // Restore the value.
                gen.m_output << "    mov rax, r12\n";

                // Address of array element:
                // array + 8 + index * 8
                gen.m_output
                    << "    lea rdx, [rbx + rcx * 8 + 8]\n";


                // Normal assignment.

                if (stmt_assign->op.type == TokenType::assign_
            || stmt_assign->op.type == TokenType::to_)
                {
                    gen.m_output << "    mov qword [rdx], rax\n";
                    return;
                }

                // Compound assignment.
                gen.m_output << "    mov rdx, [rbx]\n";

                switch (stmt_assign->op.type)
                {
                    case TokenType::plus_assign_:
                gen.m_output << "    add rdx, rax\n";
                break;

            case TokenType::minus_assign_:
                gen.m_output << "    sub rdx, rax\n";
                break;

            case TokenType::multiply_assign_:
                gen.m_output << "    imul rdx, rax\n";
                break;

            case TokenType::divide_assign_:
                gen.m_output << "    mov r8, rax\n";
                gen.m_output << "    mov rax, rdx\n";
                gen.m_output << "    cqo\n";
                gen.m_output << "    idiv r8\n";
                gen.m_output << "    mov rdx, rax\n";
                break;

            case TokenType::mod_assign_:
                gen.m_output << "    mov r8, rax\n";
                gen.m_output << "    mov rax, rdx\n";
                gen.m_output << "    cqo\n";
                gen.m_output << "    idiv r8\n";
                gen.m_output << "    mov rdx, rdx\n";
                break;

            default:
                cerr << "Unsupported assignment operator for array element\n";
                exit(EXIT_FAILURE);
        }

        gen.m_output << "    mov [rbx], rdx\n";
                return;
            }

            gen.gen_expr(stmt_assign->expr);
            gen.pop("rax");

            // Normal assignment.
                if (stmt_assign->op.type == TokenType::assign_
        || stmt_assign->op.type == TokenType::to_)
            {
                gen.m_output
                    << "    mov qword [rbp - "
                    << gen.stack_offset(**var)
                    << "], rax\n";

                return;
            }

            // Load current value.
                gen.m_output
                    << "    mov rdx, [rbp - "
                    << gen.stack_offset(**var)
                    << "]\n";
                if ((*var)->type == ValueType::Decimal)
                {
                    switch (stmt_assign->op.type)
                    {
                        case TokenType::plus_assign_:
                            gen.gen_decimal_assign_op("add");
                            break;

                        case TokenType::minus_assign_:
                            gen.gen_decimal_assign_op("sub");
                            break;

                        case TokenType::multiply_assign_:
                            gen.gen_decimal_assign_op("mul");
                            break;

                        case TokenType::divide_assign_:
                            gen.gen_decimal_assign_op("div");
                            break;

                        case TokenType::mod_assign_:
                            gen.gen_decimal_assign_op("rem");
                            break;

                        default:
                            cerr << "Unsupported decimal assignment operator\n";
                            exit(EXIT_FAILURE);
                    }

                    gen.m_output
                        << "    mov qword [rbp - "
                        << gen.stack_offset(**var)
                        << "], rdx\n";

                    return;
                }
            switch (stmt_assign->op.type)
            {
                case TokenType::plus_assign_:
                    gen.m_output << "    add rdx, rax\n";
                    break;

                case TokenType::minus_assign_:
                    gen.m_output << "    sub rdx, rax\n";
                    break;

                case TokenType::multiply_assign_:
                    gen.m_output << "    imul rdx, rax\n";
                    break;

                case TokenType::divide_assign_:
                    gen.m_output << "    mov r8, rax\n";
                    gen.m_output << "    mov rax, rdx\n";
                    gen.m_output << "    cqo\n";
                    gen.m_output << "    idiv r8\n";
                    gen.m_output << "    mov rdx, rax\n";
                    break;

                case TokenType::mod_assign_:
                    gen.m_output << "    mov r8, rax\n";
                    gen.m_output << "    mov rax, rdx\n";
                    gen.m_output << "    cqo\n";
                    gen.m_output << "    idiv r8\n";
                    gen.m_output << "    mov rdx, rdx\n";
                    break;

                default:
                    cerr << "Unsupported assignment operator\n";
                    exit(EXIT_FAILURE);
            }

            gen.m_output
                << "    mov qword [rbp - "
                << gen.stack_offset(**var)
                << "], rdx\n";
        }

            void operator()(const NodeScope* scope) const
            {
                gen.m_output << "    ;; scope\n";
                gen.gen_scope(scope);
                gen.m_output << "    ;; /scope\n";
            }

            void operator()(const NodeStmtIf* stmt_if) const
            {
                gen.m_output << "    ;; if\n";
                gen.gen_expr(stmt_if->expr);
                gen.pop("rax");
                const string label = gen.create_label();
                gen.m_output << "    test rax, rax\n";
                gen.m_output << "    jz " << label << "\n";
                gen.gen_scope(stmt_if->scope);
                if (stmt_if->pred.has_value()) {
                    const string end_label = gen.create_label();
                    gen.m_output << "    jmp " << end_label << "\n";
                    gen.m_output << label << ":\n";
                    gen.gen_if_pred(stmt_if->pred.value(), end_label);
                    gen.m_output << end_label << ":\n";
                }
                else {
                    gen.m_output << label << ":\n";
                }
                gen.m_output << "    ;; /if\n";
            }
            void operator()(const NodeStmtWhile* stmt_while) const
            {
                const string start_label = gen.create_label();
                const string end_label = gen.create_label();

                gen.push_loop(start_label, end_label);

                gen.m_output << start_label << ":\n";

                gen.gen_expr(stmt_while->expr);

                gen.pop("rax");

                gen.m_output << "    test rax, rax\n";
                gen.m_output << "    jz " << end_label << "\n";

                gen.gen_scope(stmt_while->scope);

                gen.m_output << "    jmp " << start_label << "\n";

                gen.m_output << end_label << ":\n";

                gen.pop_loop();
            }

            void operator()(const NodeStmtDoWhile* stmt_do_while) const
            {
                const string start_label = gen.create_label();
                const string continue_label = gen.create_label();
                const string end_label = gen.create_label();

                gen.push_loop(continue_label, end_label);

                gen.m_output << start_label << ":\n";

                gen.gen_scope(stmt_do_while->scope);

                gen.gen_scope(stmt_do_while->scope);

                gen.m_output << continue_label << ":\n";

                gen.gen_expr(stmt_do_while->expr);

                gen.gen_expr(stmt_do_while->expr);

                gen.pop("rax");

                gen.m_output << "    test rax, rax\n";
                gen.m_output << "    jnz " << start_label << "\n";

                gen.m_output << end_label << ":\n";

                gen.pop_loop();
            }


            void operator()(const NodeStmtFor* stmt_for) const
            {
                auto var = gen.find_var(stmt_for->ident.value.value());

                // If the loop variable does not already exist,
                // create it automatically as a number variable.
                if (!var.has_value())
                {
                    gen.gen_expr(stmt_for->start);
                    gen.pop("rax");

                    gen.m_output
                        << "    push rax\n";

                    gen.m_stack_size++;

                    Var loop_var;
                    loop_var.name = stmt_for->ident.value.value();
                    loop_var.type = ValueType::Number;
                    loop_var.stack_loc = gen.m_stack_size - 1;

                    gen.m_vars.push_back(loop_var);

                    var = gen.find_var(stmt_for->ident.value.value());
                }

                const string start_label = gen.create_label();
                const string continue_label = gen.create_label();
                const string end_label = gen.create_label();

                gen.push_loop(continue_label, end_label);

                // Initialize loop variable
                gen.gen_expr(stmt_for->start);
                gen.pop("rax");

                gen.m_output
                    << "    mov [rbp - "
                    << gen.stack_offset(**var)
                    << "], rax\n";

                gen.m_output << start_label << ":\n";

                // Save current loop variable
                gen.m_output
                    << "    mov rax, [rbp - "
                    << gen.stack_offset(**var)
                    << "]\n";

                gen.m_output << "    push rax\n";

                // Generate end value
                gen.gen_expr(stmt_for->end);

                gen.pop("rbx");   // end value
                gen.pop("rax");   // current loop value

                gen.m_output << "    cmp rax, rbx\n";
                gen.m_output << "    jg " << end_label << "\n";

                // Loop body
                gen.gen_scope(stmt_for->scope);

                // Continue point: increment loop variable
                gen.m_output << continue_label << ":\n";

                gen.m_output
                    << "    inc qword [rbp - "
                    << gen.stack_offset(**var)
                    << "]\n";

                gen.m_output << "    jmp " << start_label << "\n";

                gen.m_output << end_label << ":\n";

                gen.pop_loop();
            }
            void operator()(const NodeStmtChoose* stmt_choose) const
            {
                const string end_label = gen.create_label();

                // Evaluate Choose expression once.
                gen.gen_expr(stmt_choose->expr);
                gen.pop("rax");

                // Keep the Choose value safe while generating cases.
                gen.m_output << "    mov r12, rax\n";

                // exit. inside this Choose jumps here.
                gen.m_choose_exit_labels.push_back(end_label);

                vector<string> case_labels;

                for (size_t i = 0; i < stmt_choose->cases.size(); ++i)
                {
                    case_labels.push_back(gen.create_label());
                }

                const string default_label = gen.create_label();

                // -----------------------------
                // Check which option matches
                // -----------------------------
                for (size_t i = 0; i < stmt_choose->cases.size(); ++i)
                {
                    gen.gen_expr(stmt_choose->cases[i].value);
                    gen.pop("rbx");

                    gen.m_output
                        << "    cmp r12, rbx\n";

                    gen.m_output
                        << "    je "
                        << case_labels[i]
                        << "\n";
                }

                // No option matched.
                gen.m_output
                    << "    jmp "
                    << default_label
                    << "\n";

                // -----------------------------
                // Option bodies
                // -----------------------------
                for (size_t i = 0; i < stmt_choose->cases.size(); ++i)
                {
                    gen.m_output
                        << case_labels[i]
                        << ":\n";

                    gen.gen_scope(
                        stmt_choose->cases[i].scope
                    );

                    gen.m_output
                        << "    jmp "
                        << end_label
                        << "\n";
                }

                // -----------------------------
                // Other
                // -----------------------------
                gen.m_output
                    << default_label
                    << ":\n";

                if (stmt_choose->default_case.has_value())
                {
                    gen.gen_scope(
                        stmt_choose->default_case->scope
                    );
                }

                gen.m_choose_exit_labels.pop_back();

                gen.m_output
                    << end_label
                    << ":\n";
            }
            void operator()(const NodeStmtStruct* stmt_struct) const
            {
                const auto* structure =
                    stmt_struct->structure;

                const string name =
                    structure->name.value.value();

                Generator::StructInfo info;

                size_t offset = 0;

                for (const auto& field : structure->fields)
                {
                    const string field_name =
                        field.ident.value.value();

                    info.field_offsets[field_name] =
                        offset;

                    ValueType field_type = ValueType::Unknown;

                    if (field.value != nullptr)
                    {
                        if (gen.is_text_expr(field.value))
                            field_type = ValueType::Text;
                        else if (gen.is_decimal_expr(field.value))
                            field_type = ValueType::Decimal;
                        else
                            field_type = ValueType::Number;
                    }

                    info.field_types[field_name] = field_type;

                    offset += 8;
                }

                info.size = offset;

                gen.m_structs[name] = info;
            }
            void operator()(const NodeStmtClass* stmt_class) const
            {
                gen.m_output << "    ;; class "<< stmt_class->class_->name.value.value()<< "\n";
                for (const auto& field : stmt_class->class_->fields)
                {
                    gen.m_output << "    ;; field "<< field.ident.value.value()<< "\n";
                }
                for (const auto& method : stmt_class->class_->methods)
                {
                    gen.m_output << "    ;; method "<< method.name.value.value()<< "\n";

                    if (method.scope)
                        gen.gen_scope(method.scope);
                }
                gen.m_output << "    ;; end class\n";
            }

            void operator()(const NodeStmtEnum* stmt_enum) const
            {
                const auto* e = stmt_enum->enum_;

                for (size_t i = 0; i < e->values.size(); ++i)
                {
                    const string name =
                        e->values[i].value.value();

                    gen.m_enum_values[name] =
                        static_cast<long long>(i);
                }
            }
            void operator()(const NodeStmtThrow* stmt_throw) const
            {
                (void)stmt_throw;
            }
            void operator()(const NodeStmtTry* stmt_try) const
            {
                (void)stmt_try;
            }
            void operator()(const NodeStmtUsing* stmt_using) const
            {
                (void)stmt_using;
            }
            void operator()(const NodeStmtImport* stmt_import) const
            {
                (void)stmt_import;
            }
            void operator()(const NodeStmtInclude* stmt_include) const
            {
                (void)stmt_include;
            }
            void operator()(const NodeStmtPrint* stmt_print) const
            {
                for (const NodeExpr* expr : stmt_print->exprs)
                {
                    gen.gen_expr(expr);

                    if (gen.is_text_expr(expr))
                    {
                        gen.pop("rax");
                        gen.m_output << "    call print_string\n";
                    }
                    else if (gen.is_decimal_expr(expr))
                    {
                        gen.pop("rax");
                        gen.m_output << "    movq xmm0, rax\n";
                        gen.m_output << "    call print_decimal\n";
                    }
                    else
                    {
                        gen.pop("rdi");
                        gen.m_output << "    call print_int\n";
                    }
                }
            }
            void operator()(const NodeStmtContinue* stmt_continue) const
            {
                (void)stmt_continue;

                if (gen.m_continue_labels.empty())
                {
                    cerr << "continue outside loop\n";
                    exit(EXIT_FAILURE);
                }

                gen.m_output
                    << "    jmp "
                    << gen.m_continue_labels.back()
                    << "\n";
            }
            void operator()(const NodeSuperCall* super_call) const
            {
                (void)super_call;
            }
            void operator()(const NodeStmtNext* stmt_next) const
            {
                (void)stmt_next;

                if (gen.m_continue_labels.empty())
                {
                    cerr << "next outside loop\n";
                    exit(EXIT_FAILURE);
                }

                gen.m_output
                    << "    jmp "
                    << gen.m_continue_labels.back()
                    << "\n";
            }
            void operator()(const NodeStmtExit*) const
            {
                if (gen.m_choose_exit_labels.empty())
                {
                    cerr << "exit outside choose\n";
                    exit(EXIT_FAILURE);
                }

                gen.m_output
                    << "    jmp "
                    << gen.m_choose_exit_labels.back()
                    << "\n";
            }
            void operator()(const NodeStmtNamespace* stmt_namespace) const
            {
                gen.m_output << "    ;; namespace "<< stmt_namespace->ns->name.value.value()<< "\n";
                gen.gen_scope(stmt_namespace->ns->scope);
                gen.m_output << "    ;; end namespace\n";
            }
            void operator()(const NodeStmtStop*) const
            {
                if (gen.m_break_labels.empty()) {
                    cerr << "stop outside loop\n";
                    exit(EXIT_FAILURE);
                }

                gen.m_output << "    jmp "
                             << gen.m_break_labels.back()
                             << "\n";
            }
            void operator()(const NodeStmtGive* stmt_give) const
            {
                if (!gen.m_current_function.empty())
                {
                    if (gen.is_struct_expr(stmt_give->expr))
                    {
                        const auto* term =
                            get_if<NodeTerm*>(&stmt_give->expr->var);

                        if (term != nullptr && *term != nullptr)
                        {
                            const auto* ident =
                                get_if<NodeTermIdent*>(&(*term)->var);

                            if (ident != nullptr)
                            {
                                auto var =
                                    gen.find_var(
                                        (*ident)->ident.value.value()
                                    );

                                if (var.has_value() &&
                                    (*var)->type == ValueType::Struct)
                                {
                                    gen.m_function_return_structs[
                                        gen.m_current_function
                                    ] = (*var)->type_name;
                                }
                            }
                        }
                    }
                    else if (gen.is_decimal_expr(stmt_give->expr))
                    {
                        gen.m_function_return_types[gen.m_current_function] =
                            ValueType::Decimal;
                    }
                    else if (gen.is_text_expr(stmt_give->expr))
                    {
                        gen.m_function_return_types[gen.m_current_function] =
                            ValueType::Text;
                    }
                }

                gen.gen_expr(stmt_give->expr);

                gen.pop("rax");

                gen.m_output << "    mov rsp, rbp\n";
                gen.m_output << "    pop rbp\n";
                gen.m_output << "    ret\n";
            }

            void operator()(const NodeStmtReturn* stmt_return) const
            {
                if (!gen.m_current_function.empty())
                {
                    if (gen.is_struct_expr(stmt_return->expr))
                    {
                        const auto* term =
                            get_if<NodeTerm*>(&stmt_return->expr->var);

                        if (term != nullptr && *term != nullptr)
                        {
                            const auto* ident =
                                get_if<NodeTermIdent*>(&(*term)->var);

                            if (ident != nullptr)
                            {
                                auto var =
                                    gen.find_var(
                                        (*ident)->ident.value.value()
                                    );

                                if (var.has_value() &&
                                    (*var)->type == ValueType::Struct)
                                {
                                    gen.m_function_return_structs[
                                        gen.m_current_function
                                    ] = (*var)->type_name;
                                }
                            }
                        }
                    }
                    else if (gen.is_decimal_expr(stmt_return->expr))
                    {
                        gen.m_function_return_types[gen.m_current_function] =
                            ValueType::Decimal;
                    }
                    else if (gen.is_text_expr(stmt_return->expr))
                    {
                        gen.m_function_return_types[gen.m_current_function] =
                            ValueType::Text;
                    }
                }
                gen.gen_expr(stmt_return->expr);

                gen.pop("rax");

                gen.m_output << "    mov rsp, rbp\n";
                gen.m_output << "    pop rbp\n";
                gen.m_output << "    ret\n";
            }
           void operator()(const NodeStmtCreate* stmt_create) const
        {
            const auto* fn = stmt_create->function;

            static const char* arg_registers[] =
            {
                "rdi",
                "rsi",
                "rdx",
                "rcx",
                "r8",
                "r9"
            };

            if (fn->params.size() > 6)
            {
                cerr << "Functions with more than 6 parameters are not supported yet\n";
                exit(EXIT_FAILURE);
            }

            const string function_label =fn->name.value.value();
                        gen.m_current_function = function_label;

            const string skip_label =
                gen.create_label();

            // Skip function body during normal program execution.
            gen.m_output
                << "    jmp "
                << skip_label
        << "\n";

        // Function entry.
        gen.m_output
            << function_label
            << ":\n";

        gen.m_output << "    push rbp\n";
        gen.m_output << "    mov rbp, rsp\n";

        // Create a scope for parameters.
        gen.begin_scope();

        // Store parameters in stack slots.
        for (size_t i = 0; i < fn->params.size(); ++i)
        {
            ValueType type = ValueType::Unknown;

            switch (fn->params[i].type.type)
            {
                case TokenType::number_:
                    type = ValueType::Number;
                    break;

                case TokenType::decimal_:
                    type = ValueType::Decimal;
                    break;

                case TokenType::text_:
                    type = ValueType::Text;
                    break;

                case TokenType::boolean_:
                    type = ValueType::Boolean;
                    break;

                case TokenType::character_:
                    type = ValueType::Character;
                    break;

                case TokenType::array_:
                    type = ValueType::Array;
                    break;

                default:
                {
                    const string custom_type =
                        fn->params[i].type.value.value();

                    if (gen.m_structs.contains(custom_type))
                    {
                        type = ValueType::Struct;
                    }
                    else
                    {
                        cerr << "Unknown parameter type: "
                             << custom_type
                             << endl;

                        exit(EXIT_FAILURE);
                    }

                    break;
                }
            }

        gen.m_output
            << "    push "
            << arg_registers[i]
            << "\n";

        gen.m_stack_size++;

        gen.m_vars.push_back({
            .name = fn->params[i].ident.value.value(),
            .type = type,
            .stack_loc = gen.m_stack_size - 1,
            .type_name =
                type == ValueType::Struct
                    ? fn->params[i].type.value.value()
                    : "",
            .is_parameter = true,
            .is_constant = false
        });
    }

        // Generate function body.
        gen.gen_scope(fn->scope);
                gen.m_current_function.clear();


        gen.end_scope();

        gen.m_output << "    mov rsp, rbp\n";
        gen.m_output << "    pop rbp\n";
        gen.m_output << "    ret\n";

        // Resume normal program execution.
        gen.m_output
            << skip_label
            << ":\n";
    }
                void operator()(const NodeStmtCall* stmt_call) const
            {
                static const char* arg_registers[] =
                {
                    "rdi",
                    "rsi",
                    "rdx",
                    "rcx",
                    "r8",
                    "r9"
                };

                const auto* call = stmt_call->call;

                if (call->args.size() > 6)
                {
                    cerr << "Functions with more than 6 arguments are not supported yet\n";
                    exit(EXIT_FAILURE);
                }

                for (size_t i = 0; i < call->args.size(); ++i)
                {
                    gen.gen_expr(call->args[i]);
                    gen.pop(arg_registers[i]);
                }

                gen.m_output
                    << "    call "
                    << call->name.value.value()
                    << "\n";
            }
            void operator()(const NodeStmtAsk* stmt_ask) const
            {
                // Show prompt.
                if (stmt_ask->prompt)
                {
                    gen.gen_expr(stmt_ask->prompt);
                    gen.pop("rax");

                    gen.m_output << "    call print_string\n";
                }

                auto var =
                    gen.find_var(stmt_ask->ident.value.value());

                if (!var.has_value())
                {
                    cerr << "Undeclared identifier: "
                         << stmt_ask->ident.value.value()
                         << endl;
                    exit(EXIT_FAILURE);
                }

                switch ((*var)->type)
                {
                    case ValueType::Number:
                    case ValueType::Boolean:
                    case ValueType::Character:
                        gen.m_output << "    call read_int\n";
                        break;

                    case ValueType::Decimal:
                        gen.m_output << "    call read_decimal\n";
                        break;

                    case ValueType::Text:
                        gen.m_output << "    call read_string\n";
                        break;

                    default:
                        cerr << "Input is not supported for this variable type\n";
                        exit(EXIT_FAILURE);
                }

                // Store returned value/pointer in the variable.
                gen.m_output
                    << "    mov qword [rbp - "
                    << gen.stack_offset(**var)
                    << "], rax\n";
            }


        };

        StmtVisitor visitor { .gen = *this };
        visit(visitor, stmt->var);
    }

    [[nodiscard]] string gen_prog()
    {
        m_output << "section .text\n";
        m_output << "global _start\n";
        m_output << "extern print_int\n";
        m_output << "extern print_string\n";
        m_output << "extern read_int\n";
        m_output << "extern read_string\n";
        m_output << "extern read_decimal\n";
        m_output << "extern print_decimal\n";
        m_output << "extern string_contains\n";
        m_output << "extern string_starts_with\n";
        m_output << "extern string_ends_with\n";
        m_output << "extern string_trim\n";
        m_output << "extern string_uppercase\n";
        m_output << "extern string_lowercase\n";
        m_output << "extern string_replace\n";
        m_output << "extern string_substring\n";
        m_output << "extern string_split\n";
        m_output << "extern string_join\n";
        m_output << "_start:\n";

        m_output << "    push rbp\n";
        m_output << "    mov rbp, rsp\n";

        for (const NodeStmt* stmt : m_prog.stmts)
        {
            gen_stmt(stmt);
        }

        m_output << "\n";
        m_output << "    ;; default quit\n";
        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 0\n";
        m_output << "    syscall\n";

        // String data is generated after statements
        // because gen_stmt() discovers string literals.
        m_output << "\n";
        m_output << "section .data\n";

        for (const auto& str : m_strings)
        {
            m_output
                << str.first
                << " db \""
                << str.second
                << "\",0\n";
        }
        for (const auto& flt : m_floats)
        {
            m_output
                << flt.first
                << " dq "
                << flt.second
                << "\n";
        }

        return m_output.str();
    }

private:
    void push(const string& reg)
    {
        m_output << "    push " << reg << "\n";
        m_stack_size++;
    }

    void pop(const string& reg)
    {
        m_output << "    pop " << reg << "\n";
        m_stack_size--;
    }

    void begin_scope()
    {
        m_scopes.push_back(m_vars.size());
    }

    void end_scope()
    {
        const size_t pop_count = m_vars.size() - m_scopes.back();
        if (pop_count != 0) {
            m_output << "    add rsp, " << pop_count * 8 << "\n";
        }
        m_stack_size -= pop_count;
        for (size_t i = 0; i < pop_count; i++) {
            m_vars.pop_back();
        }
        m_scopes.pop_back();
    }

    string create_label()
    {
        stringstream ss;
        ss << "label" << m_label_count++;
        return ss.str();
    }

    [[nodiscard]] string peek_break_label() const
    {
        if (m_break_labels.empty()) {
            cerr << "No active break label\n";
            exit(EXIT_FAILURE);
        }

        return m_break_labels.back();
    }
    [[nodiscard]] string peek_continue_label() const
    {
        if (m_continue_labels.empty()) {
            cerr << "No active continue label\n";
            exit(EXIT_FAILURE);
        }

        return m_continue_labels.back();
    }
    [[nodiscard]] string new_label(const string& prefix)
    {
        stringstream ss;
        ss << prefix << "_" << m_label_count++;
        return ss.str();
    }

    void push_loop(
    const string& continue_label,
    const string& end
)
    {
        m_continue_labels.push_back(continue_label);
        m_break_labels.push_back(end);
    }

    void pop_loop()
    {
        m_continue_labels.pop_back();
        m_break_labels.pop_back();
    }

    enum class ValueType
    {
        Number,
        Decimal,
        Boolean,
        Character,
        Text,
        Array,
        Pointer,
        Object,
        Null,
        Unknown,
        Struct,
    };

    struct Var
    {
        string name;
        ValueType type = ValueType::Unknown;

        // Element type for arrays
        ValueType element_type = ValueType::Unknown;

        size_t stack_loc = 0;

        string type_name;

        bool is_parameter = false;
        bool is_constant = false;
    };

    string add_float(const string& value)
    {
        string label = "flt" + to_string(m_floats.size());

        m_floats.push_back({label, value});

        return label;
    }




    string add_string(const string& value)
    {
        for (const auto& s : m_strings)
        {
            if (s.second == value)
                return s.first;
        }

        string label = "str" + to_string(m_strings.size());

        m_strings.push_back({label, value});

        return label;
    }

    [[nodiscard]] bool is_struct_expr(const NodeExpr* expr)
    {
        if (const auto* term = get_if<NodeTerm*>(&expr->var))
        {
            if (*term == nullptr)
                return false;

            // Struct variable
            if (const auto* ident =
                    get_if<NodeTermIdent*>(&(*term)->var))
            {
                auto var =
                    find_var((*ident)->ident.value.value());

                return var.has_value() &&
                       (*var)->type == ValueType::Struct;
            }

            // Function call returning a struct
            if (const auto* call =
                    get_if<NodeCreateFunctionCall*>(&(*term)->var))
            {
                return m_function_return_structs.contains(
                    (*call)->name.value.value()
                );
            }
        }

        return false;
    }

       [[nodiscard]] bool is_text_expr(const NodeExpr* expr)
    {
        if (const auto* term = get_if<NodeTerm*>(&expr->var))
        {
            if (*term == nullptr)
                return false;

            // Actual string literal
            if (holds_alternative<NodeTermString*>((*term)->var))
                return true;

            // String variable
            if (const auto* ident =
                    get_if<NodeTermIdent*>(&(*term)->var))
            {
                auto var =
                    find_var((*ident)->ident.value.value());

                return var.has_value() &&
                       (*var)->type == ValueType::Text;
            }

            // Parenthesized expression
            if (const auto* paren =
                    get_if<NodeTermParen*>(&(*term)->var))
            {
                return is_text_expr((*paren)->expr);
            }

            // String-producing operations
            if (holds_alternative<NodeTermSubstring*>((*term)->var))
                return true;

            if (holds_alternative<NodeTermTrim*>((*term)->var))
                return true;

            if (holds_alternative<NodeTermUppercase*>((*term)->var))
                return true;

            if (holds_alternative<NodeTermLowercase*>((*term)->var))
                return true;

            if (holds_alternative<NodeTermReplace*>((*term)->var))
                return true;

            if (holds_alternative<NodeTermJoin*>((*term)->var))
                return true;


            // Function call returning text
            if (const auto* call =
                    get_if<NodeCreateFunctionCall*>(&(*term)->var))
            {
                auto it =
                    m_function_return_types.find(
                        (*call)->name.value.value()
                    );

                return it != m_function_return_types.end() &&
                       it->second == ValueType::Text;
            }

            // Struct field access returning text
            if (const auto* field_access =
                    get_if<NodeTermFieldAccess*>(&(*term)->var))
            {
                const auto* object_term =
                    get_if<NodeTerm*>(&(*field_access)->object->var);

                if (object_term != nullptr && *object_term != nullptr)
                {
                    const auto* ident =
                        get_if<NodeTermIdent*>(&(*object_term)->var);

                    if (ident != nullptr)
                    {
                        auto var =
                            find_var((*ident)->ident.value.value());

                        if (var.has_value() &&
                            (*var)->type == ValueType::Struct)
                        {
                            auto struct_it =
                                m_structs.find((*var)->type_name);

                            if (struct_it != m_structs.end())
                            {
                                auto field_it =
                                    struct_it->second.field_types.find(
                                    (*field_access)->field.value.value()
                                    );

                                if (field_it !=
                                    struct_it->second.field_types.end())
                                {
                                    return field_it->second ==
                                           ValueType::Text;
                                }
                            }
                        }
                    }
                }
            }

            // Indexed array element
                if (const auto* index =
                    get_if<NodeTermIndex*>(&(*term)->var))
            {
                    if (const auto* array_term =
                get_if<NodeTerm*>(&(*index)->array->var))
                {
                    if (*array_term != nullptr)
                    {
                        if (const auto* ident =
                                get_if<NodeTermIdent*>(&(*array_term)->var))
                        {
                            auto var =
                                find_var(
                                    (*ident)->ident.value.value()
                                );

                            return var.has_value() &&
                                   (*var)->type == ValueType::Array &&
                                   (*var)->element_type == ValueType::Text;
                        }
                    }
                }
            }

            // Integers, decimals, chars, booleans, null,
            // pointers, arrays, etc. are NOT text.
            return false;
        }

        return false;
    }

        [[nodiscard]] bool is_decimal_expr(const NodeExpr* expr)
    {
        if (const auto* term = get_if<NodeTerm*>(&expr->var))
        {
            if (*term == nullptr)
                return false;

            if (holds_alternative<NodeTermFloatLit*>((*term)->var))
                return true;

            if (const auto* ident =
                    get_if<NodeTermIdent*>(&(*term)->var))
            {
                auto var =
                    find_var((*ident)->ident.value.value());

                return var.has_value() &&
                       (*var)->type == ValueType::Decimal;
            }

            if (const auto* paren =
        get_if<NodeTermParen*>(&(*term)->var))
            {
                return is_decimal_expr((*paren)->expr);
            }

            if (const auto* call =
        get_if<NodeCreateFunctionCall*>(&(*term)->var))
            {
                auto it =
                    m_function_return_types.find(
                        (*call)->name.value.value()
                    );

                return it != m_function_return_types.end() &&
                       it->second == ValueType::Decimal;
            }
            return false;
        }

        if (const auto* bin =
                get_if<NodeBinExpr*>(&expr->var))
        {
            if (*bin == nullptr)
                return false;

            struct BinTypeVisitor
            {
                Generator& gen;

                bool operator()(const NodeBinExprAdd* node) const
                {
                    return gen.is_decimal_expr(node->lhs) ||
                           gen.is_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprSub* node) const
                {
                    return gen.is_decimal_expr(node->lhs) ||
                           gen.is_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprMulti* node) const
                {
                    return gen.is_decimal_expr(node->lhs) ||
                           gen.is_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprDiv* node) const
                {
                    return gen.is_decimal_expr(node->lhs) ||
                           gen.is_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprRemainder* node) const
                {
                    return gen.is_decimal_expr(node->lhs) ||
                           gen.is_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprGT*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprLT*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprGTE*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprLTE*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprEQ*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprNEQ*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprAND*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprOR*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprBitAnd*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprBitOr*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprBitXor*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprShl*) const
                {
                    return false;
                }

                bool operator()(const NodeBinExprShr*) const
                {
                    return false;
                }
            };

            BinTypeVisitor visitor{*this};
            return visit(visitor, (*bin)->var);
        }

        return false;
    }



    [[nodiscard]] optional<Var*> find_var(const string& name)
    {
        for (auto it = m_vars.rbegin(); it != m_vars.rend(); ++it)
        {
            if (it->name == name)
                return &(*it);
        }

        return {};
    }

        [[nodiscard]] bool has_decimal_expr(const NodeExpr* expr)
    {
        if (const auto* term = get_if<NodeTerm*>(&expr->var))
        {
            if (*term == nullptr)
                return false;

            if (holds_alternative<NodeTermFloatLit*>((*term)->var))
                return true;

            if (const auto* ident =
                    get_if<NodeTermIdent*>(&(*term)->var))
            {
                auto var =
                    find_var((*ident)->ident.value.value());

                return var.has_value() &&
                       (*var)->type == ValueType::Decimal;
            }

            if (const auto* paren =
                    get_if<NodeTermParen*>(&(*term)->var))
            {
                return has_decimal_expr((*paren)->expr);
            }

            return false;
        }

        if (const auto* bin =
                get_if<NodeBinExpr*>(&expr->var))
        {
            if (*bin == nullptr)
                return false;

            struct DecimalVisitor
            {
                Generator& gen;

                bool operator()(const NodeBinExprAdd* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprSub* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprMulti* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprDiv* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprRemainder* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprGT* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprLT* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprGTE* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprLTE* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprEQ* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprNEQ* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprAND* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprOR* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprBitAnd* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprBitOr* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprBitXor* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprShl* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }

                bool operator()(const NodeBinExprShr* node) const
                {
                    return gen.has_decimal_expr(node->lhs) ||
                           gen.has_decimal_expr(node->rhs);
                }
            };

            DecimalVisitor visitor{*this};
            return visit(visitor, (*bin)->var);
        }

        return false;
    }

    [[nodiscard]] size_t stack_offset(const Var& var) const
    {
        return (var.stack_loc + 1) * 8;
    }



    const NodeProg m_prog;
    stringstream m_output;
    size_t m_stack_size = 0;
    vector<Var> m_vars {};
    vector<pair<string, string>> m_strings{};
    vector<pair<string, string>> m_floats{};
    vector<size_t> m_scopes {};
    vector<string> m_break_labels{};
    vector<string> m_continue_labels{};
    vector<string> m_choose_exit_labels;
    unordered_map<string, ValueType> m_function_return_types {};
    unordered_map<string, string> m_function_return_structs {};
    unordered_map<string, long long> m_enum_values {};
    struct StructInfo
    {
        unordered_map<string, size_t> field_offsets;
        unordered_map<string, ValueType> field_types;
        size_t size = 0;
    };

    unordered_map<string, StructInfo> m_structs {};
    string m_current_function {};
    int m_label_count = 0;
};