#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/JSONCompilationDatabase.h>
#include <llvm-c/Target.h>
#include <llvm/Support/CommandLine.h>

//#include "checkers/TemplateChecker.h"
#include "framework/ASTManager.h"
#include "framework/BasicChecker.h"
#include "framework/CallGraph.h"
#include "framework/Config.h"
#include "framework/Logger.h"

#include "CFG/InterProcedureCFG.h"

#include "checkers/ReturnStackAddrChecker/ReturnStackAddrChecker.h"
#include "checkers/checker5/Checker5.h"

using namespace clang;
using namespace llvm;
using namespace clang::tooling;

int main(int argc, const char *argv[])
{
  ofstream process_file("time.txt");
  if (!process_file.is_open())
  {
    cerr << "can't open time.txt\n";
    return -1;
  }
  clock_t startCTime, endCTime;
  startCTime = clock();

  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmParser();

  std::vector<std::string> ASTs = initialize(argv[1]);

  Config configure(argv[2]);

  ASTResource resource;
  ASTManager manager(ASTs, resource, configure);
  CallGraph *call_graph = nullptr;

  auto enable = configure.getOptionBlock("CheckerEnable");

  Logger::configure(configure);
/*
  if (enable.find("CallGraphChecker")->second == "true")
  {
    process_file << "Starting CallGraphChecker check" << endl;
    clock_t start, end;
    // record the time of generating call graph
    start = clock();

    call_graph =
        new CallGraph(manager, resource, configure.getOptionBlock("CallGraph"));
    end = clock();
    if (configure.getOptionBlock("CallGraph").find("printToConsole")->second ==
        "true")
      call_graph->printCallGraph(cout);

    unsigned sec = unsigned((end - start) / CLOCKS_PER_SEC);
    unsigned min = sec / 60;
    process_file << "Time: " << min << "min" << sec % 60 << "sec" << endl;
    process_file
        << "End of CallGraphChecker "
           "check\n-----------------------------------------------------------"
        << endl;
  }

  // ICFG
  if (enable.find("ICFG") != enable.end() &&
      (enable.find("ICFG")->second == "true"))
  {
    process_file << "ICFG" << endl;
    clock_t start_icfg, end_icfg;
    start_icfg = clock();

    InterProcedureCFG *icfgPtr =
        new InterProcedureCFG(&manager, &resource, call_graph, &configure);
    end_icfg = clock();
    unsigned sec_icfg = unsigned((end_icfg - start_icfg) / CLOCKS_PER_SEC);
    unsigned min_icfg = sec_icfg / 60;
    process_file << "Time: " << min_icfg << "min" << sec_icfg % 60 << "sec"
                 << endl;
    process_file << "End of ICFG "
                    "\n------------------------------------------------"
                    "-----------"
                 << endl;
  }
  */

  if (enable.find("ReturnStackAddrChecker")->second == "true")
  {
    process_file << "Starting ReturnStackAddrChecker" << endl;
    clock_t start, end;
    // record the time of generating call graph
    start = clock();
    ReturnStackAddrChecker checker1(&resource, &manager, call_graph, &configure);
    checker1.check();
    end = clock();

    unsigned sec = unsigned((end - start) / CLOCKS_PER_SEC);
    unsigned min = sec / 60;
    process_file << "Time: " << min << "min" << sec % 60 << "sec" << endl;
    process_file
        << "End of ReturnStackAddrChecker "
           "check\n-----------------------------------------------------------"
        << endl;
  }

  if (enable.find("Checker5")->second == "true")
  {
    process_file << "Starting Checker5" << endl;
    clock_t start, end;
    // record the time of generating call graph
    start = clock();
    Checker5 checker5(&resource, &manager, call_graph, &configure);
    checker5.check();
    end = clock();

    unsigned sec = unsigned((end - start) / CLOCKS_PER_SEC);
    unsigned min = sec / 60;
    process_file << "Time: " << min << "min" << sec % 60 << "sec" << endl;
    process_file
        << "End of Checker5 "
           "check\n-----------------------------------------------------------"
        << endl;
  }

  endCTime = clock();
  unsigned sec = unsigned((endCTime - startCTime) / CLOCKS_PER_SEC);
  unsigned min = sec / 60;
  process_file << "-----------------------------------------------------------"
                  "\nTotal time: "
               << min << "min" << sec % 60 << "sec" << endl;
  return 0;
}
