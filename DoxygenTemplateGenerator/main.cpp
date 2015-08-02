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

static llvm::cl::OptionCategory DTGMatcherCategory("DTG Matcher");

class TagHandler : public MatchFinder::MatchCallback {
public:
  TagHandler(Rewriter &Rewrite) : TheRewriter(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    // We have a match
    if (const TagDecl *TDecl =
            Result.Nodes.getNodeAs<clang::TagDecl>("tagDecl")) {
      // Only considering tag which are not forward declaration
      if (TDecl->isCompleteDefinition()) {
        // Formatting the doxygen comment
        std::stringstream SStream;
        SStream << "/**\n";

        if (TDecl->isClass())
          SStream << "* \\class ";
        else if (TDecl->isEnum())
          SStream << "* \\enum ";
        else if (TDecl->isStruct())
          SStream << "* \\struct ";
        else if (TDecl->isUnion())
          SStream << "* \\union ";
        else
          // No comments generated for interface
          return;

        SStream << TDecl->getNameAsString() << "\n";
        SStream << "* \\brief\tDESCRIPTION\n";
        SStream << "*/\n";

        SourceLocation SrcLoc = TDecl->getSourceRange().getBegin();
        TheRewriter.InsertText(SrcLoc, SStream.str(), /*InsertAfter=*/true,
                               /*IndentNewLines*/ true);
      }
    }
  }

private:
  Rewriter &TheRewriter;
};

class FuncHandler : public MatchFinder::MatchCallback {
public:
  FuncHandler(Rewriter &Rewrite) : TheRewriter(Rewrite) {}

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
  MyASTConsumer(Rewriter &R) : HandlerForTag(R), HandlerForFunc(R) {
    Matcher.addMatcher(recordDecl().bind("tagDecl"), &HandlerForTag);
    Matcher.addMatcher(enumDecl().bind("tagDecl"), &HandlerForTag);
    Matcher.addMatcher(functionDecl().bind("funcDecl"), &HandlerForFunc);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    Matcher.matchAST(Context);
  }

private:
  TagHandler HandlerForTag;
  FuncHandler HandlerForFunc;
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
  CommonOptionsParser op(argc, argv, DTGMatcherCategory);
  ClangTool tool(op.getCompilations(), op.getSourcePathList());

  return tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
