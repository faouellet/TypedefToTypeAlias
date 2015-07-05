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

static llvm::cl::OptionCategory TypedefMatcherCategory("Typedef Matcher");

class TypedefDeclHandler : public MatchFinder::MatchCallback {
public:
  TypedefDeclHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const TypedefDecl *TDecl =
            Result.Nodes.getNodeAs<clang::TypedefDecl>("typedefDecl")) {
      const TypeSourceInfo *TSI = TDecl->getTypeSourceInfo();
      if (TDecl->getLocation().isValid() && (TSI != nullptr)) {
        std::stringstream SS;
        SS << "using " << TDecl->getNameAsString() << " = "
           << TSI->getType().getAsString();
        Rewrite.ReplaceText(TDecl->getSourceRange(), SS.str());
      }
    }
  }

private:
  Rewriter &Rewrite;
};

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : HandlerForTypedef(R) {
    Matcher.addMatcher(typedefDecl().bind("typedefDecl"), &HandlerForTypedef);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    Matcher.matchAST(Context);
  }

private:
  TypedefDeclHandler HandlerForTypedef;
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
  CommonOptionsParser op(argc, argv, TypedefMatcherCategory);
  ClangTool tool(op.getCompilations(), op.getSourcePathList());

  return tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
