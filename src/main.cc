#include <string.h>

#include <fstream>
#include <iostream>

#include "code_gen_visitor.h"
#include "cxxopts.h"
#include "lexer.h"
#include "logger.h"
#include "mem_size_visitor.h"
#include "parser.h"
#include "symbol_table_visitor.h"
#include "type_check_visitor.h"
#include "util.h"

using namespace toy;

cxxopts::ParseResult ParseArgs(int argc, char* argv[]) {
  try {
    cxxopts::Options options("toy");
    options.custom_help("[options]");
    options.positional_help("file...");
    options.add_options()("f, file", "Source code file [required]",
                          cxxopts::value<std::string>())(
        "e, exe", "Execute the generated code after compilation.")(
        "h, help", "Display this information.");
    options.parse_positional({"file"});
    cxxopts::ParseResult result = options.parse(argc, argv);
    if (result.count("help") || !result.count("file")) {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    return result;
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

int main(int argc, char* argv[]) {
  auto result = ParseArgs(argc, argv);

  // Lexical analysis
  std::string filename = result["file"].as<std::string>();
  std::ifstream file_stream(filename);
  if (!file_stream.good()) {
    std::cout << "No such file " << filename;
    return 0;
  }
  Lexer lexer(file_stream);

  // Syntax analysis
  Grammar grammar;
  ParserGenerator pgen(grammar);
  Parser parser(lexer, pgen);
  std::shared_ptr<ASTNode> ast = parser.Parse();
  ast->ToDotFile("../out/outast.gv");
  if (Logger::HasErrors()) {
    Logger::PrintErrors();
    return 0;
  }

  // Semantic analysis
  SymbolTableVisitor symtab_visitor;
  ast->Accept(symtab_visitor);
  TypeCheckVisitor typecheck_visitor;
  ast->Accept(typecheck_visitor);
  Logger::PrintWarnings();
  if (Logger::HasErrors()) {
    Logger::PrintErrors();
    return 0;
  }

  // Code generation
  MemSizeVisitor memsize_visitor;
  ast->Accept(memsize_visitor);
  CodeGenVisitor codegen_visitor;
  ast->Accept(codegen_visitor);

  if (result.count("exe")) {
    util::ExeGeneratedCode();
  }

  return 0;
}
