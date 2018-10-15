#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "config.hh"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static cl::OptionCategory MyToolCategory("doclint options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

class DocLintVisitor : public RecursiveASTVisitor<DocLintVisitor> {
public:
  explicit DocLintVisitor(ASTContext *Context) : Context(Context) {
    DiagID = Context->getDiagnostics().getCustomDiagID(
        clang::DiagnosticsEngine::Error, "missing comment");
  }

  bool VisitFunctionDecl(FunctionDecl *Declaration) {
    auto *comment = Context->getCommentForDecl(Declaration, nullptr);
    if (comment != nullptr || !shouldHaveComment(Declaration)) {
      return true;
    }

    // outs() << Declaration->getQualifiedNameAsString();
    auto &DE = Context->getDiagnostics();
    DE.Report(Declaration->getLocation(), DiagID);

    return true;
  }

private:
  bool shouldHaveComment(FunctionDecl *Declaration) {
    auto Access = Declaration->getAccess();
    FullSourceLoc FullLocation =
        Context->getFullLoc(Declaration->getBeginLoc());

    return !FullLocation.isInSystemHeader() &&
           Declaration->isExternallyVisible() &&
           (Access == AS_public || Access == AS_none) &&
           !Declaration->isTrivial();
  }

  ASTContext *Context;
  unsigned DiagID;
};

class DocLintConsumer : public clang::ASTConsumer {
public:
  explicit DocLintConsumer(ASTContext *Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  DocLintVisitor Visitor;
};

class DocLintAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new DocLintConsumer(&Compiler.getASTContext()));
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  Tool.appendArgumentsAdjuster(
      getInsertArgumentAdjuster("-resource-dir=" DOCLINT_RESOURCE_DIR));

  return Tool.run(newFrontendActionFactory<DocLintAction>().get());
}