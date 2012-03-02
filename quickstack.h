#ifndef quickstack_h
#define quickstack_h

#include <bfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <dirent.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <list>
#include <algorithm>
#include <libelf.h>

using std::string;
using std::vector;
using std::map;

// From binutils/include/demangle.h
#define DMGL_PARAMS      (1 << 0)  /* Include function args */
#define DMGL_ANSI        (1 << 1)  /* Include const, volatile, etc */
#define DMGL_VERBOSE     (1 << 3)  /* Include implementation details.  */
#define DMGL_TYPES       (1 << 4)  /* Also try to demangle type encodings. */
extern "C" char *cplus_demangle(const char *mangled, int options);

#define DBG(v, format, ...) if(debug_level >= v) { print_log(format, __VA_ARGS__); }


void print_log(const char *format, ...);

struct bfd_handle;

struct symbol_ent {
  ulong addr;
  string name;
  operator ulong() const { return addr; }
  symbol_ent() : addr(0) { }
};

struct symbol_table {
  typedef vector<symbol_ent> symbols_type;
  symbols_type symbols;
  ulong text_vma;
  ulong text_size;
  bfd_handle *bh;
  symbol_table() : text_vma(0), text_size(0) { }
  ~symbol_table() {
  }
};

struct auto_fp {
  explicit auto_fp(FILE *fp) : fp(fp) { }
  ~auto_fp() { if (fp) { fclose(fp); } }
  operator FILE *() { return fp; }
private:
  FILE *fp;
  auto_fp(const auto_fp&);
  auto_fp& operator =(const auto_fp&);
};

struct bfd_handle {
  bfd_handle(const char*filename) : filename(filename){
    size= 0;
    has_debug_symbols= false;
    syms= NULL;
    st= NULL;
    debug_file= NULL;
  }
  void close_all();
  void init(const char *file);
  void debug_init();
  void close();
  void load_symbols();
  void load_debug_section_if();
  bfd *get();
  bool has_debug();
  void free_syms_if();
  void free_st_if();
  int get_file_line(ulong addr, const char** file, const char **function, uint *lineno);

  symbol_table *st;
  const char *filename;
private:
  uint size;
  bool dynamic;
  char *debug_file;
  asection *dbgsec;
  bool has_debug_symbols;
  bfd *abfd;
  asymbol **syms;
  ulong symcnt;
};

struct symbol_table_map {
  symbol_table_map() {
  }
  ~symbol_table_map() {
    for (m_type::iterator i = m.begin(); i != m.end(); ++i) {
      symbol_table *st= i->second;
      bfd_handle *bh= st->bh;
      bh->close_all();
      delete bh;
    }
  }
  symbol_table *get(const std::string& path) {
    m_type::iterator i = m.find(path);
    if (i != m.end()) {
      return i->second;
    }
    return NULL;
  }
  void set(const std::string& path, symbol_table *st) {
    m[path]= st;
  }
 private:
  typedef map<std::string, symbol_table *> m_type;
  m_type m;
 private:
  symbol_table_map(const symbol_table_map&);
  symbol_table_map& operator =(const symbol_table_map&);
};


struct proc_map_ent {
  ulong addr_begin;
  ulong addr_size;
  ulong offset;
  string path;
  symbol_table *stbl;
  bool relative : 1;
  bool is_vdso : 1;
  operator ulong() const { return addr_begin; } /* for comparison */
  proc_map_ent() : addr_begin(0), addr_size(0), offset(0), stbl(0),
    relative(false), is_vdso(false) { }
};

struct proc_info {
  typedef vector<proc_map_ent> maps_type;
  maps_type maps;
};

typedef struct stopper_symbol {
  string name;
  ulong addr_begin;
  ulong addr_end;
} stopper_symbol;

extern int target_pid;
extern int debug_level;
extern const char *debug_dir;
extern stopper_symbol stopper[];
extern int num_stopper_symbol;
extern int print_arg;
extern int single_line;
extern int basename_only;
extern int max_ptrace_calls;

#endif
