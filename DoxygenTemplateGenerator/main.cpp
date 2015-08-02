#include <string>
#include <sstream>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory EnumDeclMatcherCategory("Enum Decl Matcher");
static llvm::cl::OptionCategory FuncDeclMatcherCategory("Func Decl Matcher");

class EnumDeclHandler : public MatchFinder::MatchCallback {
public:
  EnumDeclHandler(Rewriter &Rewrite) : TheRewriter(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    // We have a match
    if (const EnumDecl *EDecl =
            Result.Nodes.getNodeAs<clang::EnumDecl>("enumDecl")) {
      // Only considering enum which are not forward declaration
      if (EDecl->isComplete()) {
        // Formatting the doxygen comment
        std::stringstream SStream;
        SStream << "/**\n";
        SStream << "* \\enum " << EDecl->getNameAsString() << "\n";
        SStream << "* \\brief\tENUM DESCRIPTION\n";
        SStream << "*/\n";

        SourceLocation SrcLoc = EDecl->getSourceRange().getBegin();
        TheRewriter.InsertText(SrcLoc, SStream.str(), /*InsertAfter=*/true,
                               /*IndentNewLines*/ true);
      }
    }
  }

private:
  Rewriter &TheRewriter;
};

class FuncDeclHandler : public MatchFinder::MatchCallback {
public:
  FuncDeclHandler(Rewriter &Rewrite) : TheRewriter(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    // We have a match
    if (const FunctionDecl *FDecl =
            Result.Nodes.getNodeAs<clang::FunctionDecl>("funcDecl")) {
      // Only considering function definitions
      if (FDecl->hasBody()) {
        // Formatting the doxygen comment
        std::stringstream SStream;
        SStream << "/**\n";
        SStream << "* \\fn " << FDecl->getNameInfo().getAsString() << "\n";
        SStream << "* \\brief\tFUNCTION DESCRIPTION\n";
        for (auto &Param : FDecl->parameters())
          SStream << "* \\param " << Param->getName().str() << "\tPARAM DESC\n";
        SStream << "* \\return\tRETURN DESCRIPTION\n";
        SStream << "*/\n";

        SourceLocation SrcLoc = FDecl->getSourceRange().getBegin();
        TheRewriter.InsertText(SrcLoc, SStream.str(), /*InsertAfter=*/true,
                               /*IndentNewLines*/ true);
      }
    }
  }

private:
  Rewriter &TheRewriter;
};

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : HandlerForEnumDecl(R), HandlerForFuncDecl(R) {
    Matcher.addMatcher(enumDecl().bind("enumDecl"), &HandlerForEnumDecl);
    Matcher.addMatcher(functionDecl().bind("funcDecl"), &HandlerForFuncDecl);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    Matcher.matchAST(Context);
  }

private:
  EnumDeclHandler HandlerForEnumDecl;
  FuncDeclHandler HandlerForFuncDecl;
  MatchFinder Matcher;
};

class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
        .write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, FuncDeclMatcherCategory);
  ClangTool tool(op.getCompilations(), op.getSourcePathList());

  return tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
