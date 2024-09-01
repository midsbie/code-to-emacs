#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static const char* OPTION_GOTO = "-g";
static const char* PATH_EMACSCLIENT = "/usr/local/bin/emacsclient";
static const char* EMACSCLIENT_ARGS[] = {"-n"};
static const char* PATH_SH = "/bin/sh";
static const char* PATH_CODE = "/usr/share/code/bin/code";

static void print_error(const char* format, ...) {
  va_list args;
  va_start(args, format);

  const int len = strlen(format);
  char* nl_fmt = malloc((len + 1) * sizeof(char));
  strcpy(nl_fmt, format);
  nl_fmt[len] = '\n';
  nl_fmt[len + 1] = '\0';
  vfprintf(stderr, nl_fmt, args);
  free(nl_fmt);
  va_end(args);
}

static void duplicate_string(char** dest, const char* src) {
  size_t length = strlen(src) + 1;
  *dest = malloc(length);
  memcpy(*dest, src, length);
}

static void copy_without_quotes(char* dest, const char* src) {
  if (!src || !dest) {
    return;
  }

  // Skip leading whitespace and quotes.
  while (*src && (isspace((unsigned char)*src) || *src == '\'' || *src == '"')) {
    src++;
  }

  // Find the end of the string, excluding trailing whitespace and quotes.
  const char* end = src + strlen(src) - 1;
  while (end > src && (isspace((unsigned char)*end) || *end == '\'' || *end == '"')) {
    end--;
  }

  size_t length = end - src + 1;
  strncpy(dest, src, length);
  dest[length] = '\0';
}

static char* make_emacs_line_arg(const char* line) {
  const int len = strlen(line);
  char* arg = malloc(len + 2);
  if (len < 1) {
    return NULL;
  }

  sprintf(arg, "+%s", line);
  return arg;
}

static const char** handle_emacs(int argc, char** argv) {
  const int opt_goto_len = strlen(OPTION_GOTO);

  // Unity invokes Visual Code in the following form:
  //   code PROJECT_ROOT_PATH -g FILE_PATH:LINE_NUM
  //
  // Single quotes are added by Unity when paths contain special characters like whitespace, and
  // must be extracted.
  //
  // Starting by looping through all arguments looking for the `-g` option.  When found the file
  // path is extracted from the next argument.
  int i = 1;
  while (i < argc) {
    if (strncmp(OPTION_GOTO, argv[i++], opt_goto_len) != 0) {
      continue;
    }

    break;
  }

  if (i >= argc) {
    print_error("file path not specified after -g option");
    return NULL;
  }

  char* sep = strchr(argv[i], ':');
  if (!sep) {
    print_error("failed to extract file path from arguments");
    return NULL;
  }

  // Simply fake-shortening the argument string since it won't be used again.
  *sep = '\0';

  const int static_args_len = (int)sizeof(EMACSCLIENT_ARGS) / (int)sizeof(EMACSCLIENT_ARGS[0]);
  char** exec_argv = malloc((static_args_len + 4) * sizeof *exec_argv);
  int j = 0;

  // First argument contains the path to the emacsclient binary
  duplicate_string(&exec_argv[j++], PATH_EMACSCLIENT);

  // Second argument contains the line number in the form +LINE
  char* line_arg = make_emacs_line_arg(sep + 1);
  if (line_arg) {
    exec_argv[j++] = line_arg;
  }

  // Now adding all static arguments to feed to emacsclient
  for (int i = 0; i < static_args_len; ++i) {
    duplicate_string(&exec_argv[j++], EMACSCLIENT_ARGS[i]);
  }

  // Last argument contains the path to the file to edit.
  exec_argv[j] = malloc((strlen(argv[i]) + 1) * sizeof(char));
  copy_without_quotes(exec_argv[j++], argv[i]);

  exec_argv[j] = NULL;
  return (const char**)exec_argv;
}

static void free_exec_argv(const char** exec_argv) {
  if (!exec_argv) {
    return;
  }

  for (int i = 0; exec_argv[i]; ++i) {
    free((void*)exec_argv[i]);
  }
  free(exec_argv);
}

#ifdef DEBUG
static void log_argv(int argc, char** argv) {
  FILE* log_file = fopen("/tmp/code.log", "a");

  if (log_file == NULL) {
    perror("Failed to open log file");
    return;
  }

  fprintf(log_file, "Command-line arguments:\n");
  for (int i = 0; i < argc; ++i) {
    fprintf(log_file, "argv[%d]: %s\n", i, argv[i]);
  }
  fprintf(log_file, "\n");

  fclose(log_file);
}
#endif

int main(int argc, char** argv) {
  const char** exec_argv = NULL;
  exec_argv = handle_emacs(argc, argv);
  if (!exec_argv) {
    return EXIT_FAILURE;
  }

  int ret_code = 0;
  if (execv(exec_argv[0], (char**)exec_argv) == -1) {
    perror("unable to spawn child process");
    ret_code = EXIT_FAILURE;
  }

  free_exec_argv(exec_argv);
  return ret_code;
}