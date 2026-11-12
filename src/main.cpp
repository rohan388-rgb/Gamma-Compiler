
#include <fstream>
#include <iostream>
#include <optional>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

#include "generation.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3) {
        cerr << "Incorrect usage. Correct usage is:" << endl;
        cerr << "Gamma <input.hy> [--compile-only]" << endl;
        return EXIT_FAILURE;
    }

    string contents;
    {
        stringstream contents_stream;
        fstream input(argv[1], ios::in);
        if (!input)
        {
            cerr << "Cannot open file: "<< argv[1]<< endl;

            return EXIT_FAILURE;
        }
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(move(contents));
    vector<Token> tokens = tokenizer.tokenize();

    // Write compiler metadata for the Web IDE
    {
        fstream metadata("compile_metadata.json", ios::out);

        metadata << "{\n";

        // Actual token information
        metadata << "  \"tokens\": [\n";

        for (size_t i = 0; i < tokens.size(); ++i)
        {
            const auto& token = tokens[i];

            metadata << "    {";
            metadata << "\"type\":\"" << to_string(token.type) << "\"";

            if (token.value.has_value())
            {
                string value = token.value.value();

                // Basic JSON escaping
                string escaped;
                for (char c : value)
                {
                    if (c == '\\')
                        escaped += "\\\\";
                    else if (c == '"')
                        escaped += "\\\"";
                    else if (c == '\n')
                        escaped += "\\n";
                    else
                        escaped += c;
                }

                metadata << ",\"value\":\"" << escaped << "\"";
            }
            else
            {
                metadata << ",\"value\":\"\"";
            }

            metadata << ",\"line\":" << token.line;
            metadata << "}";

            if (i + 1 < tokens.size())
                metadata << ",";

            metadata << "\n";
        }

        metadata << "  ],\n";

        // Basic AST information
        metadata << "  \"ast\": {\n";
        metadata << "    \"type\": \"Program\",\n";
        metadata << "    \"children\": [\n";

        for (size_t i = 0; i < 1; ++i)
        {
            metadata << "{\"type\":\"Program\"}";
        }

        metadata << "\n    ]\n";
        metadata << "  }\n";
        metadata << "}\n";
    }

    Parser parser(move(tokens));
    optional<NodeProg> prog = parser.parse_prog();

    if (!prog.has_value()) {
        cerr << "Invalid program" << endl;
        exit(EXIT_FAILURE);
    }

    {
        Generator generator(prog.value());
        fstream file("out.asm", ios::out);
        file << generator.gen_prog();
    }

    // Assemble generated assembly
    if (system("nasm -felf64 out.asm") != 0)
    {
        cerr << "Failed to assemble out.asm\n";
        return EXIT_FAILURE;
    }

    // Assemble runtime library
    if (system("nasm -felf64 ../src/runtime.asm -o runtime.o") != 0)
    {
        cerr << "Failed to assemble runtime.asm\n";
        return EXIT_FAILURE;
    }

    cout << "Using runtime: src/runtime.asm\n";

    // Link both object files
    if (system("ld -o out out.o runtime.o") != 0)
    {
        cerr << "Linking failed.\n";
        return EXIT_FAILURE;
    }

    cout << "Compilation successful.\n";

    bool compile_only =
        argc >= 3 &&
        std::string(argv[2]) == "--compile-only";

    if (compile_only)
    {
        return EXIT_SUCCESS;
    }

    cout << "Running program...\n";
    system("./out");

    return EXIT_SUCCESS;
}