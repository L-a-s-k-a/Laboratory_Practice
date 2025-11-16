# Config.pm: namespace used for user configuration (init files) evaluation
#
# Copyright 2010-2022 Free Software Foundation, Inc.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 
# Original author: Patrice Dumas <pertusus@free.fr>
#
# functions that should not be called by user init files codes, but
# are called by the main program are prefixed by GNUT_ while functions that can
# be called by user init files codes are prefixed by texinfo_.
# Functions prefixed by _GNUT_ are private.
#
# This package is documented in the customization_api Texinfo manual.  This
# package is integrated with the Texinfo main program and the Texinfo
# HTML converter, such that it does not make sense to document the
# public interface separately.

package Texinfo::Config;

use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Carp qw(carp confess);

# for Encode::encode
use Encode;

# for __( and __p( and some functions
use Texinfo::Common;


# for error messages, passed from main program through initialization
# function.
my $real_command_name = '';


#####################################################################
# customization API, used from main program and from init files

my $cmdline_options;
my $main_program_default_options;
my $init_files_options = {};

# List options that can be set from main program are not
# handled in the same way than string options.  Indeed, the
# lists need to be defined in the main program, therefore the main
# program list options would always take precedence
# if there is a precedence, and the list options set from
# init file would never have any effect.
# Therefore, for list options, items are added and removed by
# calls to texinfo_add_to_option_list
# and texinfo_remove_from_option_list, be it from command line
# or init files, there is no precedence, but the order of calls
# matter.
my %options_as_lists;

# called from texi2any.pl main program and t/test_utils.pl.
sub GNUT_initialize_config($$$) {
  $real_command_name = shift;
  $main_program_default_options = shift;
  $cmdline_options = shift;
  # consider options passed from main program for initialization
  # as list options
  foreach my $cmdline_option (keys(%$cmdline_options)) {
    if (ref($cmdline_options->{$cmdline_option}) eq ''
        or ref($cmdline_options->{$cmdline_option}) ne 'ARRAY') {
      warn "BUG: $cmdline_option not an ARRAY $cmdline_options->{$cmdline_option}\n";
    }
    $options_as_lists{$cmdline_option} = 1;
  }
  #print STDERR "cmdline_options: ".join('|',keys(%$cmdline_options))."\n";
  return $init_files_options;
}

# duplicated from texi2any.pl
sub _GNUT_encode_message($)
{
  my $text = shift;
  my $encoding = texinfo_get_conf('MESSAGE_ENCODING');
  if (defined($encoding)) {
    return Encode::encode($encoding, $text);
  } else {
    return $text;
  }
}

# duplicated from texi2any.pl
sub _GNUT_decode_input($)
{
  my $text = shift;

  my $encoding = texinfo_get_conf('COMMAND_LINE_ENCODING');
  if (defined($encoding)) {
    return decode($encoding, $text);
  } else {
    return $text;
  }
}

# duplicated from texi2any.pl
sub _GNUT_document_warn($) {
  return if (texinfo_get_conf('NO_WARN'));
  my $text = shift;
  chomp ($text);
  warn(_GNUT_encode_message(
        sprintf(__p("program name: warning: warning_message",
                   "%s: warning: %s"), $real_command_name, $text)."\n"));
}

my @init_file_loading_messages;
# called from texi2any.pl main program and t/test_utils.pl.
# eval $FILE in the Texinfo::Config namespace. $FILE should be a binary string.
sub GNUT_load_init_file($) {
  my $file = shift;
  push @init_file_loading_messages, [];
  eval { require($file) ;};
  my $e = $@;
  if ($e ne '') {
    # $e may not be correctly encoded, but it is not clear what is to
    # be expected.  In the eval documentation there is only information
    # on the string eval case, not the block eval used here, and the
    # information is not particularly clear.
    _GNUT_document_warn(sprintf(__("error loading %s: %s"),
                _GNUT_decode_input($file), $e));
  }
  my $file_loading_messages = pop @init_file_loading_messages;
  my $error_nr = 0;
  for my $error (@{$file_loading_messages}) {
    my $type = $error->{'type'};
    my $message = $error->{'text'};
    chomp($message);
    if ($type eq 'error') {
      $error_nr += 1;
      warn(_GNUT_encode_message(
              sprintf(__p("init file: error_message",
                     "%s: %s"),
                 _GNUT_decode_input($file), $message)."\n"));
    } else {
      if (not texinfo_get_conf('NO_WARN')) {
        warn(_GNUT_encode_message(
              sprintf(__p("init file: warning: warning_message",
                          "%s: warning: %s"),
                         _GNUT_decode_input($file), $message)."\n"));
      }
    }
  }
  if ($error_nr > 0 and !texinfo_get_conf('FORCE')) {
    exit 1;
  }
}

# called from init files in case of errors at loading.
sub texinfo_register_init_loading_error($) {
  my $message = shift;
  push @{$init_file_loading_messages[-1]}, {'type' => 'error',
                                            'text' => $message};
}

# called from init files in case for warnings during loading.
sub texinfo_register_init_loading_warning($) {
  my $message = shift;
  push @{$init_file_loading_messages[-1]}, {'type' => 'warning',
                                            'text' => $message};
}

# L2H removed in 2021
# return undef var when there is nothing to set.
sub _GNUT_map_obsolete_options($$)
{
  my $input_var = shift;
  my $input_value = shift;

  my $var = $input_var;
  my $value = $input_value;

  if ($input_var eq 'L2H') {
    _GNUT_document_warn(sprintf(__("obsolete option: %s"), $input_var));
    if (! $input_value) {
      # nothing to do in that case
      $var = undef;
      $value = undef;
    } else {
      $var = 'HTML_MATH';
      $value = 'l2h';
    }
  }
  return $var, $value;
}

# Called from init files to set configuration options.
sub texinfo_set_from_init_file($$) {
  my $var = shift;
  my $value = shift;

  ($var, $value) = _GNUT_map_obsolete_options($var, $value);
  if (!defined($var)) {
    return 1;
  }
  if (!Texinfo::Common::valid_customization_option($var)) {
    # carp may be better, but infortunately, it points to the routine
    # that loads the file, and not to the init file.
    _GNUT_document_warn(sprintf(__("%s: unknown variable %s"),
                                'texinfo_set_from_init_file', $var));
    return 0;
  }
  return 0 if (defined($cmdline_options->{$var}));
  delete $main_program_default_options->{$var};
  $init_files_options->{$var} = $value;
  return 1;
}

# set option from the command line, called from main program.
# Highest precedence.
sub GNUT_set_from_cmdline($$)
{
  my $var = shift;
  my $value = shift;

  ($var, $value) = _GNUT_map_obsolete_options($var, $value);
  if (!defined($var)) {
    return 1;
  }

  delete $init_files_options->{$var};
  delete $main_program_default_options->{$var};
  if (!Texinfo::Common::valid_customization_option($var)) {
    _GNUT_document_warn(sprintf(__("unknown variable from command line: %s\n"),
                               $var));
    return 0;
  }
  $cmdline_options->{$var} = $value;
  return 1;
}

# add default based, for instance, on the format.
sub GNUT_set_main_program_default($$)
{
  my $var = shift;
  my $value = shift;

  ($var, $value) = _GNUT_map_obsolete_options($var, $value);
  if (!defined($var)) {
    return 1;
  }

  return 0 if (defined($cmdline_options->{$var})
    or defined($init_files_options->{$var}));
  $main_program_default_options->{$var} = $value;
  return 1;
}

# called both from main program and init files, for %options_as_lists
# options with lists set in main program.
sub texinfo_add_to_option_list($$)
{
  my $var = shift;
  my $values_array_ref = shift;
  if (not $options_as_lists{$var}) {
    return 0;
  }
  foreach my $value (@$values_array_ref) {
    push @{$cmdline_options->{$var}}, $value
      unless (grep {$_ eq $value} @{$cmdline_options->{$var}});
  }
  return 1;
}

# called both from main program and init files.
sub texinfo_remove_from_option_list($$)
{
  my $var = shift;
  my $values_array_ref = shift;
  if (not $options_as_lists{$var}) {
    return 0;
  }
  foreach my $value (@$values_array_ref) {
    @{$cmdline_options->{$var}}
      = grep {$_ ne $value} @{$cmdline_options->{$var}};
  }
  return 1;
}

# This function can be used to get main program variables
# customization values.
# For conversion customization variables, converter methods
# should be used instead, the implementation usually used being
# from Texinfo::Convert::Converter.
# It is possible to set up an interface similar to those used in
# converters for the main program configuration with the
# Texinfo::MainConfig below, but it should not be accessed/used
# in user defined code (and is therefore undocumented).
sub texinfo_get_conf($)
{
  my $var = shift;
  confess("BUG: texinfo_get_conf: undef \$cmdline_options."
         ." Call GNUT_initialize_config")
    if (!$cmdline_options);
  if (exists($cmdline_options->{$var})) {
    return $cmdline_options->{$var};
  } elsif (exists($init_files_options->{$var})) {
    return $init_files_options->{$var};
  } elsif (exists($main_program_default_options->{$var})) {
    return $main_program_default_options->{$var};
  } else {
    return undef;
  }
}

# to dynamically add customization options from init files
sub texinfo_add_valid_customization_option($)
{
  my $option = shift;
  return Texinfo::Common::add_valid_customization_option($option);
}


########################################################################
# Output format API.  Handled differently from customization option
# because a function from main program need to be called on formats, so
# there is a function called from the main program to get the format set
# in init files.

my $init_file_format;
sub texinfo_set_format_from_init_file($)
{
  $init_file_format = shift;
}

sub GNUT_get_format_from_init_file()
{
  return $init_file_format;
}


#####################################################################
# stages handlers API.  Used in HTML only.

my @possible_stages = ('setup', 'structure', 'init', 'finish');

my $default_priority = 'default';

# FIXME add another level with format?  Not needed now as HTML is
# the only customizable format for now.
my $GNUT_stage_handlers = {};

foreach my $stage (@possible_stages) {
  $GNUT_stage_handlers->{$stage} = {};
}

sub texinfo_register_handler($$;$)
{
  my $stage = shift;
  my $handler = shift;
  my $priority = shift;

  if (!$GNUT_stage_handlers->{$stage}) {
    carp ("Unknown stage $stage\n");
    return 0;
  }
  $priority = $default_priority if (!defined($priority));
  push @{$GNUT_stage_handlers->{$stage}->{$priority}}, $handler;
  return 1;
}

# called from the Converter
sub GNUT_get_stage_handlers()
{
  return $GNUT_stage_handlers;
}

#####################################################################
# API used to override formatting.  Used in HTML only.

my $GNUT_file_id_setting_references = {};
my $GNUT_formatting_references = {};
my $GNUT_formatting_special_element_body = {};
my $GNUT_commands_conversion = {};
my $GNUT_commands_open = {};
my $GNUT_types_conversion = {};
my $GNUT_types_open = {};
my $GNUT_no_arg_commands_formatting_strings = {};
my $GNUT_style_commands_formatting_info = {};
my $GNUT_accent_command_formatting_info = {};
my $GNUT_types_formatting_info = {};

# called from init files
sub texinfo_register_file_id_setting_function($$)
{
  my $thing = shift;
  my $handler = shift;
  $GNUT_file_id_setting_references->{$thing} = $handler;
}

# called from the Converter
sub GNUT_get_file_id_setting_references()
{
  return $GNUT_file_id_setting_references;
}

# called from init files
sub texinfo_register_formatting_function($$)
{
  my $thing = shift;
  my $handler = shift;
  $GNUT_formatting_references->{$thing} = $handler;
}

# called from the Converter
sub GNUT_get_formatting_references()
{
  return $GNUT_formatting_references;
}

# called from init files
sub texinfo_register_command_formatting($$)
{
  my $command = shift;
  my $reference = shift;
  $GNUT_commands_conversion->{$command} = $reference;
}

# called from the Converter
sub GNUT_get_commands_conversion()
{
  return $GNUT_commands_conversion;
}

# called from init files
sub texinfo_register_command_opening($$)
{
  my $command = shift;
  my $reference = shift;
  $GNUT_commands_open->{$command} = $reference;
}

# called from the Converter
sub GNUT_get_commands_open()
{
  return $GNUT_commands_open;
}

# called from init files
sub texinfo_register_type_formatting($$)
{
  my $command = shift;
  my $reference = shift;
  $GNUT_types_conversion->{$command} = $reference;
}

# called from the Converter
sub GNUT_get_types_conversion()
{
  return $GNUT_types_conversion;
}

# called from init files
sub texinfo_register_type_opening($$)
{
  my $type = shift;
  my $reference = shift;
  $GNUT_types_open->{$type} = $reference;
}

# called from the Converter
sub GNUT_get_types_open()
{
  return $GNUT_types_open;
}

# called from init files
sub texinfo_register_formatting_special_element_body($$)
{
  my $special_element_variety = shift;
  my $handler = shift;

  $GNUT_formatting_special_element_body->{$special_element_variety} = $handler;
}

# called from the Converter
sub GNUT_get_formatting_special_element_body_references()
{
  return $GNUT_formatting_special_element_body;
}

my $default_formatting_context = 'normal';
foreach my $possible_formatting_context (($default_formatting_context,
                       'preformatted', 'string', 'css_string')) {
  $GNUT_no_arg_commands_formatting_strings->{$possible_formatting_context} = {};
  $GNUT_style_commands_formatting_info->{$possible_formatting_context} = {};
}

# $translated_string is supposed to be already formatted.
# It may also be relevant to be able to pass a 'tree'
# directly (it is actually handled by the converter code).
# Passing a texinfo string that can be translated (like
# the 'translated_commands' customization variable) may also
# be interesting.
sub texinfo_register_no_arg_command_formatting($$;$$$)
{
  my $command = shift;
  my $context = shift;
  my $text = shift;
  # html element
  my $element = shift;
  my $translated_string = shift;

  if (!defined($context)) {
    $context = $default_formatting_context;
  } elsif (not defined($GNUT_no_arg_commands_formatting_strings->{$context})) {
    _GNUT_document_warn(sprintf(__("%s: unknown formatting context %s\n"),
                  'texinfo_register_no_arg_command_formatting', $context));
    return 0;
  }
  my $specification = {};
  if (defined($text)) {
    $specification->{'text'} = $text;
  }
  if (defined($element)) {
    $specification->{'element'} = $element;
  }
  if (defined($translated_string)) {
    $specification->{'translated'} = $translated_string;
    # NOTE unset 'text'?  A priori not needed, it will be overwritten
  }
  $GNUT_no_arg_commands_formatting_strings->{$context}->{$command} = $specification;
  return 1;
}

sub GNUT_get_no_arg_command_formatting($;$)
{
  my $command = shift;
  my $context = shift;

  if (!defined($context)) {
    $context = $default_formatting_context;
  } elsif (not defined($GNUT_style_commands_formatting_info->{$context})) {
    _GNUT_document_warn(sprintf(__("%s: unknown formatting context %s\n"),
                        'GNUT_get_no_arg_command_formatting', $context));
    return undef;
  }
  if (exists($GNUT_no_arg_commands_formatting_strings->{$context})
      and exists($GNUT_no_arg_commands_formatting_strings->{$context}->{$command})) {
    return $GNUT_no_arg_commands_formatting_strings->{$context}->{$command};
  }
  return undef;
}

# called from init files
sub texinfo_register_style_command_formatting($$;$$)
{
  my $command = shift;
  my $html_element = shift;
  my $in_quotes = shift;
  my $context = shift;

  if (!defined($context)) {
    $context = $default_formatting_context;
  } elsif (not defined($GNUT_style_commands_formatting_info->{$context})) {
    _GNUT_document_warn(sprintf(__("%s: unknown formatting context %s\n"),
                  'texinfo_register_style_command_formatting', $context));
    return 0;
  }
  my $specification = {};
  if ($in_quotes) {
    $specification->{'quotes'} = $in_quotes;
  }
  if (defined($html_element)) {
    $specification->{'element'} = $html_element;
  }
  $GNUT_style_commands_formatting_info->{$context}->{$command} = $specification;
  return 1;
}

sub GNUT_get_style_command_formatting($;$)
{
  my $command = shift;
  my $context = shift;

  if (!defined($context)) {
    $context = $default_formatting_context;
  } elsif (not defined($GNUT_style_commands_formatting_info->{$context})) {
    _GNUT_document_warn(sprintf(__("%s: unknown formatting context %s\n"),
                        'GNUT_get_style_command_formatting', $context));
    return undef;
  }
  if (exists($GNUT_style_commands_formatting_info->{$context})
      and exists($GNUT_style_commands_formatting_info->{$context}->{$command})) {
    return $GNUT_style_commands_formatting_info->{$context}->{$command};
  }
  return undef;
}

# called from init files
sub texinfo_register_accent_command_formatting($$$)
{
  my $command = shift;
  my $accent_command_entity = shift;
  my $accent_command_text_with_entities = shift;

  $GNUT_accent_command_formatting_info->{$command}
    = [$accent_command_entity, $accent_command_text_with_entities];
  return 1;
}

# called from the Converter
sub GNUT_get_accent_command_formatting($)
{
  my $command = shift;
  if (exists($GNUT_accent_command_formatting_info->{$command})) {
    return @{$GNUT_accent_command_formatting_info->{$command}};
  }
  return (undef, undef);
}

# need to give both arguments
# FIXME is it ok, or should there be two functions?
sub texinfo_register_type_format_info($$$)
{
  my $type = shift;
  my $code_type = shift;
  my $pre_class_type = shift;

  $GNUT_types_formatting_info->{$type}
    = {'code' => $code_type, 'pre_class' => $pre_class_type};
}

sub GNUT_get_types_formatting_info()
{
  # NOTE a deep copy could also be done if needed
  return { %$GNUT_types_formatting_info };
}


#####################################################################
# the objective of this small package is to be in another
# scope than init files and setup blessed objects that can call
# get_conf() and set_conf() methods like a parser or a converter.
#
# For the main program, there is also the need to have
# access to configuration options in order to have get_conf()
# return the same as Texinfo::Config::texinfo_get_conf().
# This is obtained by calling new() without argument.
#
# In tests the situation is different as nothing from the
# Texinfo::Config space is used, it is assumed that the
# configuration is available as a hash reference key
# value.  This is obtained by calling new() with an hash
# reference argument.
package Texinfo::MainConfig;

sub new(;$)
{
  my $options = shift;
  my $config;
  if (defined($options)) {
    # creates a new object based on input hash reference
    $config = {'standalone' => 1, 'config' => {%$options}};
  } else {
    # use Texinfo::Config
    $config = {'standalone' => 0, 'config' => {}};
  }
  bless $config;
  return $config;
}

sub get_conf($$)
{
  my $self = shift;
  my $var = shift;

  if ($self->{'standalone'}) {
    if (defined($self->{'config'}->{$var})) {
      return $self->{'config'}->{$var};
    }
  } else {
    # as get_conf, but with self having precedence on
    # main program defaults
    if (exists($cmdline_options->{$var})) {
      return $cmdline_options->{$var};
    } elsif (exists($init_files_options->{$var})) {
      return $init_files_options->{$var};
    } elsif (exists($self->{'config'}->{$var})) {
      return $self->{'config'}->{$var};
    } elsif (exists($main_program_default_options->{$var})) {
      return $main_program_default_options->{$var};
    } else {
      return undef;
    }
  }
}

sub set_conf($$$)
{
  my $self = shift;
  my $var = shift;
  my $val = shift;
  $self->{'config'}->{$var} = $val;
}


1;
