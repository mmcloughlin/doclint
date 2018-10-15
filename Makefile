LLVM_BIN=$(LLVM_DIR)/bin
LLVM_LIB=$(LLVM_DIR)/lib

CXX=./clang++
CFG=$(LLVM_BIN)/llvm-config

CLANGLIBS = clangFrontend clangSerialization clangDriver \
           	clangTooling clangParse clangSema clangAnalysis \
           	clangEdit clangAST clangLex clangBasic

CXXFLAGS = `${CFG} --cxxflags`
LDFLAGS = `${CFG} --ldflags`
LDLIBS = `${CFG} --libs --system-libs` $(addprefix -l, $(CLANGLIBS))

progs=hello example

all: $(progs)

config.hh: config.py
	python $< --prefix DOCLINT_ > $@

example: example.cc
	$(CXX) -o $@ $^

%.ast: %.cc
	$(LLVM_BIN)/clang++ -Xclang -ast-dump -fsyntax-only $< > $@

clean:
	$(RM) $(progs)