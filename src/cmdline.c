/*
  File autogenerated by gengetopt version 2.22.2
  generated with the following command:
  gengetopt --unamed-opts=URL --no-version 

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif

#include "getopt.h"

#include "cmdline.h"

const char *gengetopt_args_info_purpose = "";

const char *gengetopt_args_info_usage = "Usage: quvi [OPTIONS]... [URL]...";

const char *gengetopt_args_info_description = "";

const char *gengetopt_args_info_help[] = {
  "  -h, --help                  Print help and exit",
  "  -v, --version               Print version and exit",
  "      --hosts                 Show supported hosts",
  "  -q, --quiet                 Turn off all output",
  "      --debug                 Turn on libcurl verbose mode",
  "  -n, --no-verify             Do not verify video link",
  "      --page-title=<string>   expected video page title",
  "      --video-id=<string>     expected video id",
  "      --file-length=<length>  expected video file length",
  "      --file-suffix=<string>  expected video file suffix",
  "  -a, --test-all              Run built-in tests",
  "  -t, --test=<regexp>         match regexp to a built-in test link",
  "  -d, --dump                  Dump video details when running tests",
  "      --agent=<string>        Identify cclive as string to servers  \n                                (default=`Mozilla/5.0')",
  "      --proxy=<host[:port]>   Use specified proxy",
  "      --connect-timeout=<s>   Max seconds allowed connection to server take  \n                                (default=`30')",
  "  -f, --format=<format_id>    Query video format  (possible values=\"flv\", \n                                \"best\", \"fmt17\", \"fmt18\", \"fmt22\", \n                                \"fmt34\", \"fmt35\", \"hq\", \"3gp\", \n                                \"spark-mini\", \"vp6-hq\", \"vp6-hd\", \n                                \"vp6\", \"h264\", \"hd\", \"mp4\", \"high\", \n                                \"ipod\", \"vp6_576\", \"vp6_928\", \n                                \"h264_1400\" default=`flv')",
  "\nExaples:\n\n  quvi -a         # run all built-in tests\n  quvi URL        # test URL\n  quvi -d URL     # test URL and dump video details\n  quvi -t youtube # match 'youtube' to built-in links and test it\n  quvi URL -f mp4 # query 'mp4' format of the video\n",
    0
};

typedef enum {ARG_NO
  , ARG_STRING
  , ARG_INT
  , ARG_DOUBLE
} cmdline_parser_arg_type;

static
void clear_given (struct gengetopt_args_info *args_info);
static
void clear_args (struct gengetopt_args_info *args_info);

static int
cmdline_parser_internal (int argc, char * const *argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);

static int
cmdline_parser_required2 (struct gengetopt_args_info *args_info, const char *prog_name, const char *additional_error);

const char *cmdline_parser_format_values[] = {"flv", "best", "fmt17", "fmt18", "fmt22", "fmt34", "fmt35", "hq", "3gp", "spark-mini", "vp6-hq", "vp6-hd", "vp6", "h264", "hd", "mp4", "high", "ipod", "vp6_576", "vp6_928", "h264_1400", 0}; /*< Possible values for format. */

static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct gengetopt_args_info *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->hosts_given = 0 ;
  args_info->quiet_given = 0 ;
  args_info->debug_given = 0 ;
  args_info->no_verify_given = 0 ;
  args_info->page_title_given = 0 ;
  args_info->video_id_given = 0 ;
  args_info->file_length_given = 0 ;
  args_info->file_suffix_given = 0 ;
  args_info->test_all_given = 0 ;
  args_info->test_given = 0 ;
  args_info->dump_given = 0 ;
  args_info->agent_given = 0 ;
  args_info->proxy_given = 0 ;
  args_info->connect_timeout_given = 0 ;
  args_info->format_given = 0 ;
}

static
void clear_args (struct gengetopt_args_info *args_info)
{
  FIX_UNUSED (args_info);
  args_info->page_title_arg = NULL;
  args_info->page_title_orig = NULL;
  args_info->video_id_arg = NULL;
  args_info->video_id_orig = NULL;
  args_info->file_length_orig = NULL;
  args_info->file_suffix_arg = NULL;
  args_info->file_suffix_orig = NULL;
  args_info->test_arg = NULL;
  args_info->test_orig = NULL;
  args_info->agent_arg = gengetopt_strdup ("Mozilla/5.0");
  args_info->agent_orig = NULL;
  args_info->proxy_arg = NULL;
  args_info->proxy_orig = NULL;
  args_info->connect_timeout_arg = 30;
  args_info->connect_timeout_orig = NULL;
  args_info->format_arg = gengetopt_strdup ("flv");
  args_info->format_orig = NULL;
  
}

static
void init_args_info(struct gengetopt_args_info *args_info)
{


  args_info->help_help = gengetopt_args_info_help[0] ;
  args_info->version_help = gengetopt_args_info_help[1] ;
  args_info->hosts_help = gengetopt_args_info_help[2] ;
  args_info->quiet_help = gengetopt_args_info_help[3] ;
  args_info->debug_help = gengetopt_args_info_help[4] ;
  args_info->no_verify_help = gengetopt_args_info_help[5] ;
  args_info->page_title_help = gengetopt_args_info_help[6] ;
  args_info->video_id_help = gengetopt_args_info_help[7] ;
  args_info->file_length_help = gengetopt_args_info_help[8] ;
  args_info->file_suffix_help = gengetopt_args_info_help[9] ;
  args_info->test_all_help = gengetopt_args_info_help[10] ;
  args_info->test_help = gengetopt_args_info_help[11] ;
  args_info->dump_help = gengetopt_args_info_help[12] ;
  args_info->agent_help = gengetopt_args_info_help[13] ;
  args_info->proxy_help = gengetopt_args_info_help[14] ;
  args_info->connect_timeout_help = gengetopt_args_info_help[15] ;
  args_info->format_help = gengetopt_args_info_help[16] ;
  
}

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n",
     (strlen(CMDLINE_PARSER_PACKAGE_NAME) ? CMDLINE_PARSER_PACKAGE_NAME : CMDLINE_PARSER_PACKAGE),
     CMDLINE_PARSER_VERSION);
}

static void print_help_common(void) {
  cmdline_parser_print_version ();

  if (strlen(gengetopt_args_info_purpose) > 0)
    printf("\n%s\n", gengetopt_args_info_purpose);

  if (strlen(gengetopt_args_info_usage) > 0)
    printf("\n%s\n", gengetopt_args_info_usage);

  printf("\n");

  if (strlen(gengetopt_args_info_description) > 0)
    printf("%s\n\n", gengetopt_args_info_description);
}

void
cmdline_parser_print_help (void)
{
  int i = 0;
  print_help_common();
  while (gengetopt_args_info_help[i])
    printf("%s\n", gengetopt_args_info_help[i++]);
}

void
cmdline_parser_init (struct gengetopt_args_info *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);

  args_info->inputs = 0;
  args_info->inputs_num = 0;
}

void
cmdline_parser_params_init(struct cmdline_parser_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct cmdline_parser_params *
cmdline_parser_params_create(void)
{
  struct cmdline_parser_params *params = 
    (struct cmdline_parser_params *)malloc(sizeof(struct cmdline_parser_params));
  cmdline_parser_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
cmdline_parser_release (struct gengetopt_args_info *args_info)
{
  unsigned int i;
  free_string_field (&(args_info->page_title_arg));
  free_string_field (&(args_info->page_title_orig));
  free_string_field (&(args_info->video_id_arg));
  free_string_field (&(args_info->video_id_orig));
  free_string_field (&(args_info->file_length_orig));
  free_string_field (&(args_info->file_suffix_arg));
  free_string_field (&(args_info->file_suffix_orig));
  free_string_field (&(args_info->test_arg));
  free_string_field (&(args_info->test_orig));
  free_string_field (&(args_info->agent_arg));
  free_string_field (&(args_info->agent_orig));
  free_string_field (&(args_info->proxy_arg));
  free_string_field (&(args_info->proxy_orig));
  free_string_field (&(args_info->connect_timeout_orig));
  free_string_field (&(args_info->format_arg));
  free_string_field (&(args_info->format_orig));
  
  
  for (i = 0; i < args_info->inputs_num; ++i)
    free (args_info->inputs [i]);

  if (args_info->inputs_num)
    free (args_info->inputs);

  clear_given (args_info);
}

/**
 * @param val the value to check
 * @param values the possible values
 * @return the index of the matched value:
 * -1 if no value matched,
 * -2 if more than one value has matched
 */
static int
check_possible_values(const char *val, const char *values[])
{
  int i, found, last;
  size_t len;

  if (!val)   /* otherwise strlen() crashes below */
    return -1; /* -1 means no argument for the option */

  found = last = 0;

  for (i = 0, len = strlen(val); values[i]; ++i)
    {
      if (strncmp(val, values[i], len) == 0)
        {
          ++found;
          last = i;
          if (strlen(values[i]) == len)
            return i; /* exact macth no need to check more */
        }
    }

  if (found == 1) /* one match: OK */
    return last;

  return (found ? -2 : -1); /* return many values or none matched */
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  int found = -1;
  if (arg) {
    if (values) {
      found = check_possible_values(arg, values);      
    }
    if (found >= 0)
      fprintf(outfile, "%s=\"%s\" # %s\n", opt, arg, values[found]);
    else
      fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}


int
cmdline_parser_dump(FILE *outfile, struct gengetopt_args_info *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", CMDLINE_PARSER_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->hosts_given)
    write_into_file(outfile, "hosts", 0, 0 );
  if (args_info->quiet_given)
    write_into_file(outfile, "quiet", 0, 0 );
  if (args_info->debug_given)
    write_into_file(outfile, "debug", 0, 0 );
  if (args_info->no_verify_given)
    write_into_file(outfile, "no-verify", 0, 0 );
  if (args_info->page_title_given)
    write_into_file(outfile, "page-title", args_info->page_title_orig, 0);
  if (args_info->video_id_given)
    write_into_file(outfile, "video-id", args_info->video_id_orig, 0);
  if (args_info->file_length_given)
    write_into_file(outfile, "file-length", args_info->file_length_orig, 0);
  if (args_info->file_suffix_given)
    write_into_file(outfile, "file-suffix", args_info->file_suffix_orig, 0);
  if (args_info->test_all_given)
    write_into_file(outfile, "test-all", 0, 0 );
  if (args_info->test_given)
    write_into_file(outfile, "test", args_info->test_orig, 0);
  if (args_info->dump_given)
    write_into_file(outfile, "dump", 0, 0 );
  if (args_info->agent_given)
    write_into_file(outfile, "agent", args_info->agent_orig, 0);
  if (args_info->proxy_given)
    write_into_file(outfile, "proxy", args_info->proxy_orig, 0);
  if (args_info->connect_timeout_given)
    write_into_file(outfile, "connect-timeout", args_info->connect_timeout_orig, 0);
  if (args_info->format_given)
    write_into_file(outfile, "format", args_info->format_orig, cmdline_parser_format_values);
  

  i = EXIT_SUCCESS;
  return i;
}

int
cmdline_parser_file_save(const char *filename, struct gengetopt_args_info *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = cmdline_parser_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
cmdline_parser_free (struct gengetopt_args_info *args_info)
{
  cmdline_parser_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

int
cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info)
{
  return cmdline_parser2 (argc, argv, args_info, 0, 1, 1);
}

int
cmdline_parser_ext (int argc, char * const *argv, struct gengetopt_args_info *args_info,
                   struct cmdline_parser_params *params)
{
  int result;
  result = cmdline_parser_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser2 (int argc, char * const *argv, struct gengetopt_args_info *args_info, int override, int initialize, int check_required)
{
  int result;
  struct cmdline_parser_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = cmdline_parser_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required (struct gengetopt_args_info *args_info, const char *prog_name)
{
  int result = EXIT_SUCCESS;

  if (cmdline_parser_required2(args_info, prog_name, 0) > 0)
    result = EXIT_FAILURE;

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required2 (struct gengetopt_args_info *args_info, const char *prog_name, const char *additional_error)
{
  FIX_UNUSED (additional_error);
  int error = 0;

  /* checks for required options */
  
  /* checks for dependences among options */
  if (args_info->dump_given && ! args_info->test_all_given)
    {
      fprintf (stderr, "%s: '--dump' ('-d') option depends on option 'test-all'%s\n", prog_name, (additional_error ? additional_error : ""));
      error = 1;
    }

  return error;
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see cmdline_parser_params.check_ambiguity
 * @param override @see cmdline_parser_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               cmdline_parser_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  FIX_UNUSED (field);
  char *stop_char = 0;
  const char *val = value;
  int found;
  char **string_field;

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  if (possible_values && (found = check_possible_values((value ? value : default_value), possible_values)) < 0)
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: %s argument, \"%s\", for option `--%s' (`-%c')%s\n", 
          package_name, (found == -2) ? "ambiguous" : "invalid", value, long_opt, short_opt,
          (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: %s argument, \"%s\", for option `--%s'%s\n", 
          package_name, (found == -2) ? "ambiguous" : "invalid", value, long_opt,
          (additional_error ? additional_error : ""));
      return 1; /* failure */
    }
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_INT:
    if (val) *((int *)field) = strtol (val, &stop_char, 0);
    break;
  case ARG_DOUBLE:
    if (val) *((double *)field) = strtod (val, &stop_char);
    break;
  case ARG_STRING:
    if (val) {
      string_field = (char **)field;
      if (!no_free && *string_field)
        free (*string_field); /* free previous string */
      *string_field = gengetopt_strdup (val);
    }
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_INT:
  case ARG_DOUBLE:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
cmdline_parser_internal (
  int argc, char * const *argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;	/* Character of the parsed option.  */

  int error = 0;
  struct gengetopt_args_info local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    cmdline_parser_init (args_info);

  cmdline_parser_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'v' },
        { "hosts",	0, NULL, 0 },
        { "quiet",	0, NULL, 'q' },
        { "debug",	0, NULL, 0 },
        { "no-verify",	0, NULL, 'n' },
        { "page-title",	1, NULL, 0 },
        { "video-id",	1, NULL, 0 },
        { "file-length",	1, NULL, 0 },
        { "file-suffix",	1, NULL, 0 },
        { "test-all",	0, NULL, 'a' },
        { "test",	1, NULL, 't' },
        { "dump",	0, NULL, 'd' },
        { "agent",	1, NULL, 0 },
        { "proxy",	1, NULL, 0 },
        { "connect-timeout",	1, NULL, 0 },
        { "format",	1, NULL, 'f' },
        { 0,  0, 0, 0 }
      };

      c = getopt_long (argc, argv, "hvqnat:df:", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          cmdline_parser_print_help ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'v':	/* Print version and exit.  */
        
        
          if (update_arg( 0 , 
               0 , &(args_info->version_given),
              &(local_args_info.version_given), optarg, 0, 0, ARG_NO,
              check_ambiguity, override, 0, 0,
              "version", 'v',
              additional_error))
            goto failure;
        
          break;
        case 'q':	/* Turn off all output.  */
        
        
          if (update_arg( 0 , 
               0 , &(args_info->quiet_given),
              &(local_args_info.quiet_given), optarg, 0, 0, ARG_NO,
              check_ambiguity, override, 0, 0,
              "quiet", 'q',
              additional_error))
            goto failure;
        
          break;
        case 'n':	/* Do not verify video link.  */
        
        
          if (update_arg( 0 , 
               0 , &(args_info->no_verify_given),
              &(local_args_info.no_verify_given), optarg, 0, 0, ARG_NO,
              check_ambiguity, override, 0, 0,
              "no-verify", 'n',
              additional_error))
            goto failure;
        
          break;
        case 'a':	/* Run built-in tests.  */
        
        
          if (update_arg( 0 , 
               0 , &(args_info->test_all_given),
              &(local_args_info.test_all_given), optarg, 0, 0, ARG_NO,
              check_ambiguity, override, 0, 0,
              "test-all", 'a',
              additional_error))
            goto failure;
        
          break;
        case 't':	/* match regexp to a built-in test link.  */
        
        
          if (update_arg( (void *)&(args_info->test_arg), 
               &(args_info->test_orig), &(args_info->test_given),
              &(local_args_info.test_given), optarg, 0, 0, ARG_STRING,
              check_ambiguity, override, 0, 0,
              "test", 't',
              additional_error))
            goto failure;
        
          break;
        case 'd':	/* Dump video details when running tests.  */
        
        
          if (update_arg( 0 , 
               0 , &(args_info->dump_given),
              &(local_args_info.dump_given), optarg, 0, 0, ARG_NO,
              check_ambiguity, override, 0, 0,
              "dump", 'd',
              additional_error))
            goto failure;
        
          break;
        case 'f':	/* Query video format.  */
        
        
          if (update_arg( (void *)&(args_info->format_arg), 
               &(args_info->format_orig), &(args_info->format_given),
              &(local_args_info.format_given), optarg, cmdline_parser_format_values, "flv", ARG_STRING,
              check_ambiguity, override, 0, 0,
              "format", 'f',
              additional_error))
            goto failure;
        
          break;

        case 0:	/* Long option with no short option */
          /* Show supported hosts.  */
          if (strcmp (long_options[option_index].name, "hosts") == 0)
          {
          
          
            if (update_arg( 0 , 
                 0 , &(args_info->hosts_given),
                &(local_args_info.hosts_given), optarg, 0, 0, ARG_NO,
                check_ambiguity, override, 0, 0,
                "hosts", '-',
                additional_error))
              goto failure;
          
          }
          /* Turn on libcurl verbose mode.  */
          else if (strcmp (long_options[option_index].name, "debug") == 0)
          {
          
          
            if (update_arg( 0 , 
                 0 , &(args_info->debug_given),
                &(local_args_info.debug_given), optarg, 0, 0, ARG_NO,
                check_ambiguity, override, 0, 0,
                "debug", '-',
                additional_error))
              goto failure;
          
          }
          /* expected video page title.  */
          else if (strcmp (long_options[option_index].name, "page-title") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->page_title_arg), 
                 &(args_info->page_title_orig), &(args_info->page_title_given),
                &(local_args_info.page_title_given), optarg, 0, 0, ARG_STRING,
                check_ambiguity, override, 0, 0,
                "page-title", '-',
                additional_error))
              goto failure;
          
          }
          /* expected video id.  */
          else if (strcmp (long_options[option_index].name, "video-id") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->video_id_arg), 
                 &(args_info->video_id_orig), &(args_info->video_id_given),
                &(local_args_info.video_id_given), optarg, 0, 0, ARG_STRING,
                check_ambiguity, override, 0, 0,
                "video-id", '-',
                additional_error))
              goto failure;
          
          }
          /* expected video file length.  */
          else if (strcmp (long_options[option_index].name, "file-length") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->file_length_arg), 
                 &(args_info->file_length_orig), &(args_info->file_length_given),
                &(local_args_info.file_length_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "file-length", '-',
                additional_error))
              goto failure;
          
          }
          /* expected video file suffix.  */
          else if (strcmp (long_options[option_index].name, "file-suffix") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->file_suffix_arg), 
                 &(args_info->file_suffix_orig), &(args_info->file_suffix_given),
                &(local_args_info.file_suffix_given), optarg, 0, 0, ARG_STRING,
                check_ambiguity, override, 0, 0,
                "file-suffix", '-',
                additional_error))
              goto failure;
          
          }
          /* Identify cclive as string to servers.  */
          else if (strcmp (long_options[option_index].name, "agent") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->agent_arg), 
                 &(args_info->agent_orig), &(args_info->agent_given),
                &(local_args_info.agent_given), optarg, 0, "Mozilla/5.0", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "agent", '-',
                additional_error))
              goto failure;
          
          }
          /* Use specified proxy.  */
          else if (strcmp (long_options[option_index].name, "proxy") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->proxy_arg), 
                 &(args_info->proxy_orig), &(args_info->proxy_given),
                &(local_args_info.proxy_given), optarg, 0, 0, ARG_STRING,
                check_ambiguity, override, 0, 0,
                "proxy", '-',
                additional_error))
              goto failure;
          
          }
          /* Max seconds allowed connection to server take.  */
          else if (strcmp (long_options[option_index].name, "connect-timeout") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->connect_timeout_arg), 
                 &(args_info->connect_timeout_orig), &(args_info->connect_timeout_given),
                &(local_args_info.connect_timeout_given), optarg, 0, "30", ARG_INT,
                check_ambiguity, override, 0, 0,
                "connect-timeout", '-',
                additional_error))
              goto failure;
          
          }
          
          break;
        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", CMDLINE_PARSER_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */



  if (check_required)
    {
      error += cmdline_parser_required2 (args_info, argv[0], additional_error);
    }

  cmdline_parser_release (&local_args_info);

  if ( error )
    return (EXIT_FAILURE);

  if (optind < argc)
    {
      int i = 0 ;
      int found_prog_name = 0;
      /* whether program name, i.e., argv[0], is in the remaining args
         (this may happen with some implementations of getopt,
          but surely not with the one included by gengetopt) */

      i = optind;
      while (i < argc)
        if (argv[i++] == argv[0]) {
          found_prog_name = 1;
          break;
        }
      i = 0;

      args_info->inputs_num = argc - optind - found_prog_name;
      args_info->inputs =
        (char **)(malloc ((args_info->inputs_num)*sizeof(char *))) ;
      while (optind < argc)
        if (argv[optind++] != argv[0])
          args_info->inputs[ i++ ] = gengetopt_strdup (argv[optind-1]) ;
    }

  return 0;

failure:
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}
