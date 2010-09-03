/** @file cmdline.h
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.22.4
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt by Lorenzo Bettini */

#ifndef CMDLINE_H
#define CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define CMDLINE_PARSER_PACKAGE "quvi"
#endif

#ifndef CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define CMDLINE_PARSER_PACKAGE_NAME "quvi"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION ""
#endif

/** @brief Where the command line options are stored */
struct gengetopt_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  const char *license_help; /**< @brief Print license and exit help description.  */
  const char *hosts_help; /**< @brief Show supported hosts help description.  */
  const char *xml_help; /**< @brief Print details in XML help description.  */
  const char *quiet_help; /**< @brief Turn off output to stderr help description.  */
  const char *debug_help; /**< @brief Turn on libcurl verbose mode help description.  */
  const char *no_verify_help; /**< @brief Do not verify video link help description.  */
  char * page_title_arg;	/**< @brief Expected video page title.  */
  char * page_title_orig;	/**< @brief Expected video page title original value given at command line.  */
  const char *page_title_help; /**< @brief Expected video page title help description.  */
  char * video_id_arg;	/**< @brief Expected video id.  */
  char * video_id_orig;	/**< @brief Expected video id original value given at command line.  */
  const char *video_id_help; /**< @brief Expected video id help description.  */
  double file_length_arg;	/**< @brief Expected video file length.  */
  char * file_length_orig;	/**< @brief Expected video file length original value given at command line.  */
  const char *file_length_help; /**< @brief Expected video file length help description.  */
  char * file_suffix_arg;	/**< @brief Expected video file suffix.  */
  char * file_suffix_orig;	/**< @brief Expected video file suffix original value given at command line.  */
  const char *file_suffix_help; /**< @brief Expected video file suffix help description.  */
  const char *test_all_help; /**< @brief Run built-in tests help description.  */
  char * test_arg;	/**< @brief Match regexp to a built-in test link.  */
  char * test_orig;	/**< @brief Match regexp to a built-in test link original value given at command line.  */
  const char *test_help; /**< @brief Match regexp to a built-in test link help description.  */
  const char *dump_help; /**< @brief Dump video details when running tests help description.  */
  char * agent_arg;	/**< @brief Identify cclive as string to servers (default='Mozilla/5.0').  */
  char * agent_orig;	/**< @brief Identify cclive as string to servers original value given at command line.  */
  const char *agent_help; /**< @brief Identify cclive as string to servers help description.  */
  char * proxy_arg;	/**< @brief Use specified proxy.  */
  char * proxy_orig;	/**< @brief Use specified proxy original value given at command line.  */
  const char *proxy_help; /**< @brief Use specified proxy help description.  */
  int connect_timeout_arg;	/**< @brief Max seconds allowed connection to server take (default='30').  */
  char * connect_timeout_orig;	/**< @brief Max seconds allowed connection to server take original value given at command line.  */
  const char *connect_timeout_help; /**< @brief Max seconds allowed connection to server take help description.  */
  char * format_arg;	/**< @brief Query video format (default='default').  */
  char * format_orig;	/**< @brief Query video format original value given at command line.  */
  const char *format_help; /**< @brief Query video format help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int license_given ;	/**< @brief Whether license was given.  */
  unsigned int hosts_given ;	/**< @brief Whether hosts was given.  */
  unsigned int xml_given ;	/**< @brief Whether xml was given.  */
  unsigned int quiet_given ;	/**< @brief Whether quiet was given.  */
  unsigned int debug_given ;	/**< @brief Whether debug was given.  */
  unsigned int no_verify_given ;	/**< @brief Whether no-verify was given.  */
  unsigned int page_title_given ;	/**< @brief Whether page-title was given.  */
  unsigned int video_id_given ;	/**< @brief Whether video-id was given.  */
  unsigned int file_length_given ;	/**< @brief Whether file-length was given.  */
  unsigned int file_suffix_given ;	/**< @brief Whether file-suffix was given.  */
  unsigned int test_all_given ;	/**< @brief Whether test-all was given.  */
  unsigned int test_given ;	/**< @brief Whether test was given.  */
  unsigned int dump_given ;	/**< @brief Whether dump was given.  */
  unsigned int agent_given ;	/**< @brief Whether agent was given.  */
  unsigned int proxy_given ;	/**< @brief Whether proxy was given.  */
  unsigned int connect_timeout_given ;	/**< @brief Whether connect-timeout was given.  */
  unsigned int format_given ;	/**< @brief Whether format was given.  */

  char **inputs ; /**< @brief unamed options (options without names) */
  unsigned inputs_num ; /**< @brief unamed options number */
} ;

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure gengetopt_args_info (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure gengetopt_args_info (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *gengetopt_args_info_purpose;
/** @brief the usage string of the program */
extern const char *gengetopt_args_info_usage;
/** @brief all the lines making the help output */
extern const char *gengetopt_args_info_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser (int argc, char **argv,
  struct gengetopt_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2 (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile,
  struct gengetopt_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename,
  struct gengetopt_args_info *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed gengetopt_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init (struct gengetopt_args_info *args_info);
/**
 * Deallocates the string fields of the gengetopt_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free (struct gengetopt_args_info *args_info);

/**
 * The config file parser (deprecated version)
 * @param filename the name of the config file
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_config_file() instead
 */
int cmdline_parser_configfile (const char *filename,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The config file parser
 * @param filename the name of the config file
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_config_file (const char *filename,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int cmdline_parser_required (struct gengetopt_args_info *args_info,
  const char *prog_name);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CMDLINE_H */
