#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#undef DEBUG_ARGS

static const char* OPTION_FROM_UNITY = "--from-unity";
static const char* OPTION_GOTO = "-g";
static const char* PATH_EMACSCLIENT = "/usr/local/bin/emacsclient";
static const char* PATH_SH = "/bin/sh";
static const char* PATH_CODE = "/usr/share/code/bin/code";

static void print_error(const char *format, ...) {
  va_list args;
  va_start(args, format);

  const int len = strlen(format);
  char *nl_fmt = malloc((len + 1) * sizeof(char));
  strcpy(nl_fmt, format);
  nl_fmt[len] = '\n';
  nl_fmt[len + 1] = '\0';
  vfprintf(stderr, nl_fmt, args);
  free(nl_fmt);
  va_end(args);
}

static void duplicate_string(char **dest, const char *src) {
  size_t length = strlen(src) + 1;
  *dest = malloc(length);
  memcpy(*dest, src, length);
}

static void copy_without_quotes(char *dest, const char *src) {
  const int l = strlen(src);
  if (l < 1) return;

  int n = src[l - 1] == '\'' ? l - 1 : l;
  if (*src == '\'') {
    --n;
    ++src;
  }
  strncpy(dest, src, n);
}

static const char** handle_emacs(int argc, char **argv) {
  const int opt_goto_len = strlen(OPTION_GOTO);
  int arg_file_idx = -1;

  // Unity invokes Visual Code in the following manner:
  //   code 'PROJECT_ROOT_PATH' -g 'FILE_PATH':LINE_NUM
  //
  // Single quotes are added by Unity and are extracted here.
  //
  // Looping through all arguments looking for the `-g` option.  When found the file path is
  // extracted from the next argument.
  for (int i = 1; i < argc; ++i) {
    if (strncmp(OPTION_GOTO, argv[i], opt_goto_len) != 0) {
      continue;
    }

    ++i;
    if (i >= argc) {
      print_error("file path not specified after -g option");
      return NULL;
    }

    char *end = strchr(argv[i], ':');
    if (end == NULL) {
      print_error("failed to extract file path from arguments");
      return NULL;
    }

    // Simply fake-shortening the argument string since it won't be used again.
    *end = '\0';
    arg_file_idx = i;
  }

  if (arg_file_idx< 1) {
    print_error("invalid arguments or location option not found");
    return NULL;
  }

  char **new_argv = malloc(3 * sizeof *new_argv);

  // First argument contains the path to the emacsclient binary
  duplicate_string(&new_argv[0], PATH_EMACSCLIENT);

  // Second argument contains the path to the file to edit, which is likely wrapped by single
  // quotes and need to be removed.
  new_argv[1] = malloc((strlen(argv[arg_file_idx]) + 1) * sizeof(char));
  copy_without_quotes(new_argv[1], argv[arg_file_idx]);

  new_argv[2] = NULL;
  return (const char **) new_argv;
}

static const char** handle_visual_code(int argc, char **argv) {
  char **new_argv = malloc((argc + 3) * sizeof *new_argv);
  int j = 0;

  duplicate_string(&new_argv[j++], PATH_SH);
  duplicate_string(&new_argv[j++], PATH_CODE);

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], OPTION_FROM_UNITY) == 0) {
      continue;
    }

    duplicate_string(&new_argv[j++], argv[i]);
  }

  new_argv[j] = NULL;
  return (const char **) new_argv;
}

int main(int argc, char **argv) {
  const char **new_argv = NULL;

  #ifdef DEBUG_ARGS
  FILE *f = fopen("/tmp/code.log", "w+");
  for (int i = 0; argv[i] != NULL; ++i) {
    fprintf(f, "%d: %s\n", i, argv[i]);
  }
  fprintf(f, "\n");
  #endif

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], OPTION_FROM_UNITY) == 0) {
      new_argv = handle_emacs(argc, argv);
      break;
    }
  }

  if (!new_argv) new_argv = handle_visual_code(argc, argv);

  #ifdef DEBUG_ARGS
  for (int i = 0; new_argv[i] != NULL; ++i) {
    fprintf(f, "%d: %s\n", i, new_argv[i]);
  }
  fclose(f);
  exit(0);
  #endif

  if (execv(new_argv[0], (char **)new_argv) == -1) {
    perror("child process `execv` failed");
    return -1;
  }

  return 0;
}