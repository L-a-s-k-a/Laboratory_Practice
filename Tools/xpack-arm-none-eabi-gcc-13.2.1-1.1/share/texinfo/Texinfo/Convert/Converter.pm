# Converter.pm: Common code for Converters.
#
# Copyright 2011-2022 Free Software Foundation, Inc.
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

package Texinfo::Convert::Converter;

use 5.00405;
use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

# for fileparse
use File::Basename;
# for file names portability
use File::Spec;
use Encode qw(decode);

use Carp qw(cluck confess);

use Texinfo::Common;

use Texinfo::Report;

use Texinfo::Convert::Utils;
use Texinfo::Convert::Unicode;
use Texinfo::Convert::Text;
use Texinfo::Convert::NodeNameNormalization;
use Texinfo::Structuring;

use Texinfo::Translations;

require Exporter;
use vars qw($VERSION @ISA @EXPORT_OK %EXPORT_TAGS);
@ISA = qw(Exporter Texinfo::Report Texinfo::Translations);

%EXPORT_TAGS = ( 'all' => [ qw(
xml_protect_text
xml_comment
xml_accent
xml_accents
) ] );

@EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

$VERSION = '7.0.3';

my %defaults = (
  'documentlanguage'     => undef,
);

# defaults for all converters that are not defined elsewhere.
# undef values in general marks information passed by the caller that
# is valid in the parser initialization hash, but is not considered
# as "configuration/customization". It is not available through get_conf()
# but is available directly in the converter as a hash key.
# FIXME separate the two types of information and check that those
# items are not valid customization options?
# NOTE converters for now do not add more, set converted_format, and,
# rarely, output_format.  It would be good to keep it that way and add
# customization options instead.
my %common_converters_defaults = (
  'language_config_dirs' => undef,
  'converted_format'     => undef,
  # can be different from the converted_format, for example, epub3
  # output format converted format is html.
  'output_format'        => undef,
  'structuring'          => undef,
  'translated_commands'  => {'error' => 'error@arrow{}',},

# This is the default, mainly for tests; the caller should set them.  These
# values are what is used in tests of the Converters.  These variables are
# customization options, set in the main program when a converter is
# called from the main program.
  'PACKAGE_AND_VERSION'  => 'texinfo',
  'PACKAGE_VERSION'      => '',
  'PACKAGE_URL'          => 'http://www.gnu.org/software/texinfo/',
  'PROGRAM'              => '',
);

my %all_converters_defaults
 = (%Texinfo::Common::default_converter_command_line_options,
    %Texinfo::Common::default_converter_customization,
    %Texinfo::Common::document_settable_unique_at_commands,
    %Texinfo::Common::document_settable_multiple_at_commands,
    %common_converters_defaults
);

# For translation of in document string.
if (0) {
  my $self;
  $self->gdt('error@arrow{}');
}

# This is not used as code, but used to mark months as strings to be
# translated
if (0) {
  my $self;
  my @mark_month_for_translation = (
   $self->gdt('January'),
   $self->gdt('February'),
   $self->gdt('March'),
   $self->gdt('April'),
   $self->gdt('May'),
   $self->gdt('June'),
   $self->gdt('July'),
   $self->gdt('August'),
   $self->gdt('September'),
   $self->gdt('October'),
   $self->gdt('November'),
   $self->gdt('December')
  );
}

our %default_args_code_style = (
  'email' => [1],
  'anchor' => [1],
  'uref' => [1],
  'url' => [1],
  'math' => [1],
  'inforef' => [1,undef,1],
  'image' => [1, 1, 1, undef, 1],
# and type?
  'float' => [1],
);

foreach my $code_style_command (keys(%Texinfo::Common::brace_code_commands)) {
  $default_args_code_style{$code_style_command} = [1];
}
foreach my $ref_cmd ('pxref', 'xref', 'ref') {
  $default_args_code_style{$ref_cmd} = [1, undef, undef, 1];
}

################################################################
# converter API

# convert_tree() and convert() should be implemented in converters.
# There is an implementation of output() below but in general
# output() should also be implemented by Converters.

# Functions that should be defined in specific converters
sub converter_defaults($$)
{
  return %defaults;
}

sub converter_initialize($)
{
}

sub output_internal_links($)
{
  my $self = shift;
  return undef;
}

# this function is designed so as to be used in specific Converters
sub converter(;$)
{
  my $class = shift;
  my $converter = { 'set' => {} };

  my $conf;
  my $name = 'converter';

  if (ref($class) eq 'HASH') {
    $conf = $class;
    bless $converter;
  } elsif (defined($class)) {
    bless $converter, $class;
    $name = ref($converter);
    $conf = shift;
  } else {
    bless $converter;
    $conf = shift;
    $name = ref($converter);
  }
  my %defaults = $converter->converter_defaults($conf);
  foreach my $key (keys(%all_converters_defaults)) {
    $defaults{$key} = $all_converters_defaults{$key}
                          if (!exists($defaults{$key}));
  }
  $converter->{'conf'} = {};
  foreach my $key (keys(%defaults)) {
    if (Texinfo::Common::valid_customization_option($key)) {
      $converter->{'conf'}->{$key} = $defaults{$key};
    } else {
      $converter->{$key} = $defaults{$key};
    }
  }
  #$converter->{'converter_pre_conf'} = \%defaults;
  if (defined($conf)) {
    if ($conf->{'parser'}) {
      $converter->{'global_commands'}
         = $conf->{'parser'}->global_commands_information();
      $converter->{'parser_info'} = $conf->{'parser'}->global_information();
      my $floats = $conf->{'parser'}->floats_information();
      my ($labels, $targets_list, $nodes_list)
        = $conf->{'parser'}->labels_information();

      $converter->{'floats'} = $floats if ($floats);
      $converter->{'labels'} = $labels if ($labels);
      $converter->{'indices_information'}
             = $conf->{'parser'}->indices_information();
      $converter->{'values'} = $conf->{'parser'}->{'values'};
      delete $conf->{'parser'};
    }
    foreach my $key (keys(%$conf)) {
      if (Texinfo::Common::valid_customization_option($key)) {
        $converter->{'conf'}->{$key} = $conf->{$key};
      } elsif (!exists($defaults{$key})) {
        warn "$key not a possible configuration in $name\n";
      } else {
        $converter->{$key} = $conf->{$key};
      }
      # configuration set here, in general coming from command-line
      # will not be reset by set_conf.
      $converter->{'set'}->{$key} = 1;
    }
  }
  # set $converter->{'converter_init_conf'} to the customization
  # options obtained after setting the defaults and applying
  # the customization passed as argument.
  $converter->{'converter_init_conf'} = { %{$converter->{'conf'}} };
  foreach my $key (keys (%defaults)) {
    if (defined($converter->{$key})) {
      $converter->{'converter_init_conf'}->{$key} = $converter->{$key};
    }
  }

  Texinfo::Common::set_output_encodings($converter, $converter->{'parser_info'});

  # turn the array to a hash for speed.  Not sure it really matters for such
  # a small array.
  my $expanded_formats = $converter->get_conf('EXPANDED_FORMATS');
  $converter->{'expanded_formats_hash'} = {};
  if (defined($expanded_formats)) {
    foreach my $expanded_format (@{$converter->get_conf('EXPANDED_FORMATS')}) {
      $converter->{'expanded_formats_hash'}->{$expanded_format} = 1;
    }
  }

  # used for output files information, to register opened
  # and not closed files.  Accessed through output_files_information()
  $converter->{'output_files'} = Texinfo::Common::output_files_initialize();

  $converter->Texinfo::Report::new();

  $converter->converter_initialize();

  return $converter;
}

sub _convert_document_tree_units($$;$$)
{
  my $self = shift;
  my $root = shift;
  my $tree_units = shift;
  my $fh = shift;

  if ($tree_units) {
    my $result = '';
    foreach my $tree_unit (@$tree_units) {
      $result .= $self->write_or_return($self->convert_tree($tree_unit), $fh);
    }
    return $result;
  } else {
    return $self->write_or_return($self->convert_tree($root), $fh);
  }
}

# the two following methods can be used to implement convert() in
# Converters.
sub convert_document_sections($$;$)
{
  my $self = shift;
  my $root = shift;
  my $fh = shift;

  my $tree_units = Texinfo::Structuring::split_by_section($root);
  return $self->_convert_document_tree_units($root, $tree_units, $fh);
}

sub convert_document_nodes($$;$)
{
  my $self = shift;
  my $root = shift;
  my $fh = shift;

  my $tree_units = Texinfo::Structuring::split_by_node($root);
  return $self->_convert_document_tree_units($root, $tree_units, $fh);
}

# In general, converters override this method, but simple
# converters can use it.  It is used for the plaintext
# output format.
# use file_counters and out_filepaths converter states.
sub output($$)
{
  my $self = shift;
  my $root = shift;

  my $tree_units;

  if (defined($self->get_conf('OUTFILE'))
      and ($Texinfo::Common::null_device_file{$self->get_conf('OUTFILE')}
           or $self->get_conf('OUTFILE') eq '-'
           or $self->get_conf('OUTFILE') eq '')) {
    if ($self->get_conf('SPLIT')) {
      $self->document_warn($self,
               sprintf(__("%s: output incompatible with split"),
                                   $self->get_conf('OUTFILE')));
      $self->force_conf('SPLIT', 0);
    }
  }
  if ($self->get_conf('SPLIT')) {
    $self->set_conf('NODE_FILES', 1);
  }

  my ($output_file, $destination_directory, $output_filename,
                  $document_name) = $self->determine_files_and_directory();
  my ($encoded_destination_directory, $dir_encoding)
    = $self->encoded_output_file_name($destination_directory);
  my $succeeded
    = $self->create_destination_directory($encoded_destination_directory,
                                          $destination_directory);
  return undef unless $succeeded;

  if ($self->get_conf('USE_NODES')) {
    $tree_units = Texinfo::Structuring::split_by_node($root);
  } else {
    $tree_units = Texinfo::Structuring::split_by_section($root);
  }

  Texinfo::Structuring::split_pages($tree_units, $self->get_conf('SPLIT'));

  # determine file names associated with the different pages
  if ($output_file ne '') {
    $self->_set_tree_units_files($tree_units, $output_file, $destination_directory,
                                 $output_filename, $document_name);
  }

  #print STDERR "$tree_units $tree_units->[0]->{'structure'}->{'unit_filename'}\n";

  # Now do the output
  my $fh;
  my $output = '';
  if (!$tree_units or !$tree_units->[0]->{'structure'}
      or !defined($tree_units->[0]->{'structure'}->{'unit_filename'})) {
    # no page
    my $outfile_name;
    my $encoded_outfile_name;
    if ($output_file ne '') {
      if ($self->get_conf('SPLIT')) {
        my $top_node_file_name = $self->top_node_filename($document_name);
        if (defined($destination_directory) and $destination_directory ne '') {
          $outfile_name = File::Spec->catfile($destination_directory,
                                              $top_node_file_name);
        } else {
          $outfile_name = $top_node_file_name;
        }
      } else {
        $outfile_name = $output_file;
      }
      print STDERR "DO No pages, output in $outfile_name\n"
        if ($self->get_conf('DEBUG'));
      my $path_encoding;
      ($encoded_outfile_name, $path_encoding)
        = $self->encoded_output_file_name($outfile_name);
      my $error_message;
      ($fh, $error_message) = Texinfo::Common::output_files_open_out(
                    $self->output_files_information(), $self,
                    $encoded_outfile_name);
      if (!$fh) {
        $self->document_error($self,
                 sprintf(__("could not open %s for writing: %s"),
                                      $outfile_name, $error_message));
        return undef;
      }
    } else {
      print STDERR "DO No pages, string output\n"
        if ($self->get_conf('DEBUG'));
    }

    if ($tree_units and @$tree_units) {
      foreach my $tree_unit (@$tree_units) {
        my $tree_unit_text = $self->convert_tree($tree_unit);
        $output .= $self->write_or_return($tree_unit_text, $fh);
      }
    } else {
      # FIXME this is in general better to use convert() for instance
      # to have the converter output footnotes as it cannot do is as
      # part of tree units formatting.  However, this breaks the promise
      # that only convert_tree is used in generic Converter code.
      # REMARK that right now, this code is never called, as
      # Texinfo::Structuring::split_by_node or split_by_page always return
      # an array containing at least one unit.  But this was not the case
      # in the past and could change again in the future.
      $output .= $self->write_or_return($self->convert($root), $fh);
    }
    # NOTE do not close STDOUT now to avoid a perl warning.
    # FIXME is it still true that there is such a warning?
    if ($fh and $outfile_name ne '-') {
      Texinfo::Common::output_files_register_closed(
                  $self->output_files_information(), $encoded_outfile_name);
      if (!close($fh)) {
        $self->document_error($self,
                 sprintf(__("error on closing %s: %s"),
                                      $outfile_name, $!));
      }
    }
    return $output if ($output_file eq '');
  } else {
    # output with pages
    print STDERR "DO Elements with filenames\n"
      if ($self->get_conf('DEBUG'));
    my %files_filehandle;
    
    foreach my $tree_unit (@$tree_units) {
      my $tree_unit_filename = $tree_unit->{'structure'}->{'unit_filename'};
      my $out_filepath = $self->{'out_filepaths'}->{$tree_unit_filename};
      my $file_fh;
      # open the file and output the elements
      if (!exists($files_filehandle{$tree_unit_filename})) {
        my $error_message;
        ($file_fh, $error_message) = Texinfo::Common::output_files_open_out(
                             $self->output_files_information(), $self,
                             $out_filepath);
        if (!$file_fh) {
          $self->document_error($self,
                sprintf(__("could not open %s for writing: %s"),
                       $out_filepath, $error_message));
          return undef;
        }
        $files_filehandle{$tree_unit_filename} = $file_fh;
      } else {
        $file_fh = $files_filehandle{$tree_unit_filename};
      }
      my $tree_unit_text = $self->convert_tree($tree_unit);
      print $file_fh $tree_unit_text;
      $self->{'file_counters'}->{$tree_unit_filename}--;
      if ($self->{'file_counters'}->{$tree_unit_filename} == 0) {
        # NOTE do not close STDOUT here to avoid a perl warning
        if ($out_filepath ne '-') {
          Texinfo::Common::output_files_register_closed(
            $self->output_files_information(), $out_filepath);
          if (!close($file_fh)) {
            $self->document_error($self,
                     sprintf(__("error on closing %s: %s"),
                                  $out_filepath, $!));
            return undef;
          }
        }
      }
    }
  }
}


###############################################################
# Implementation of the customization API that is used in many
# Texinfo modules

sub get_conf($$)
{
  my $self = shift;
  my $conf = shift;
  if (!Texinfo::Common::valid_customization_option($conf)) {
    confess("CBUG: unknown option $conf\n");
    #return undef;
  }
  return $self->{'conf'}->{$conf};
}

sub set_conf($$$)
{
  my $self = shift;
  my $conf = shift;
  my $value = shift;
  if (!Texinfo::Common::valid_customization_option($conf)) {
    die "BBUG: unknown option $conf\n";
    return undef;
  }
  if ($self->{'set'}->{$conf}) {
    return 0;
  } else {
    $self->{'conf'}->{$conf} = $value;
    return 1;
  }
}

sub force_conf($$$)
{
  my $self = shift;
  my $conf = shift;
  my $value = shift;
  if (!Texinfo::Common::valid_customization_option($conf)) {
    die "ABUG: unknown option $conf\n";
    return undef;
  }
  $self->{'conf'}->{$conf} = $value;
  return 1;
}

sub output_files_information($)
{
  my $self = shift;
  return $self->{'output_files'};
}


####################################################################
# Useful functions.  Those in this section are used in the module and can
# also be used in other Converter modules.

# output fo $fh if defined, otherwise return the text.
sub write_or_return($$$)
{
  my $self = shift;
  my $text = shift;
  my $fh = shift;
  if ($fh) {
    print $fh $text;
    return '';
  } else {
    return $text;
  }
}

my $STDIN_DOCU_NAME = 'stdin';

sub determine_files_and_directory($;$)
{
  my $self = shift;
  my $output_format = shift;

  if (not defined($output_format)) {
    $output_format = $self->{'output_format'};
  }

  # determine input file base name
  my $input_basefile;
  if (defined($self->{'parser_info'}->{'input_file_name'})) {
    # 'input_file_name' is not decoded, as it is derived from input
    # file which is not decoded either.  We want to return only
    # decoded character strings such that they can easily be mixed
    # with other character strings, so we decode here.
    my $input_file_name = $self->{'parser_info'}->{'input_file_name'};
    my $encoding = $self->get_conf('COMMAND_LINE_ENCODING');
    if (defined($encoding)) {
      $input_file_name = decode($encoding, $input_file_name, sub { '?' });
      # use '?' as replacement character rather than U+FFFD in case it
      # is re-encoded to an encoding without this character
    }
    my ($directories, $suffix);
    ($input_basefile, $directories, $suffix) = fileparse($input_file_name);
  } else {
    # This could happen if called on a piece of texinfo
    $input_basefile = '';
  }

  my $input_basename;
  if ($input_basefile eq '-') {
    $input_basename = $STDIN_DOCU_NAME;
  } else {
    $input_basename = $input_basefile;
    $input_basename =~ s/\.te?x(i|info)?$//;
  }

  my $setfilename;
  if (defined($self->get_conf('setfilename'))) {
    $setfilename = $self->get_conf('setfilename');
  } elsif ($self->{'global_commands'}
           and $self->{'global_commands'}->{'setfilename'}
           and $self->{'global_commands'}->{'setfilename'}->{'extra'}
           and defined($self->{'global_commands'}->{'setfilename'}->{'extra'}->{'text_arg'})) {
    $setfilename
      = $self->{'global_commands'}->{'setfilename'}->{'extra'}->{'text_arg'};
  }

  my $input_basename_for_outfile = $input_basename;
  my $setfilename_for_outfile = $setfilename;
  # PREFIX overrides both setfilename and the input file base name
  if (defined($self->get_conf('PREFIX'))) {
    $setfilename_for_outfile = undef;
    $input_basename_for_outfile = $self->get_conf('PREFIX');
  }

  # the document path, in general the outfile without
  # extension and can be set from setfilename if outfile is not set
  my $document_path;
  # determine output file and output file name
  my $output_file;
  if (!defined($self->get_conf('OUTFILE'))) {
    if (defined($setfilename_for_outfile)
        and !$self->get_conf('NO_USE_SETFILENAME')) {
      $output_file = $setfilename_for_outfile;
      $document_path = $setfilename_for_outfile;
      $document_path =~ s/\.[^\.]*$//;
      if (!$self->get_conf('USE_SETFILENAME_EXTENSION')) {
        $output_file =~ s/\.[^\.]*$//;
        $output_file .= '.'.$self->get_conf('EXTENSION')
          if (defined($self->get_conf('EXTENSION'))
              and $self->get_conf('EXTENSION') ne '');
      }
    } elsif ($input_basename_for_outfile ne '') {
      $output_file = $input_basename_for_outfile;
      $document_path = $input_basename_for_outfile;
      $output_file .= '.'.$self->get_conf('EXTENSION')
        if (defined($self->get_conf('EXTENSION'))
            and $self->get_conf('EXTENSION') ne '');
    } else {
      $output_file = '';
      $document_path = $output_file;
    }
    if (defined($self->get_conf('SUBDIR')) and $output_file ne '') {
      my $dir = File::Spec->canonpath($self->get_conf('SUBDIR'));
      $output_file = File::Spec->catfile($dir, $output_file);
    }
  } else {
    $document_path = $self->get_conf('OUTFILE');
    $document_path =~ s/\.[^\.]*$//;
    $output_file = $self->get_conf('OUTFILE');
  }

  # the output file path, in general same as the outfile but can be
  # set from setfilename if outfile is not set.
  my $output_filepath = $output_file;
  # in this case one wants to get the result in a string and there
  # is a setfilename.  The setfilename is used to get something.
  # This happens in the test suite.
  if ($output_file eq '' and defined($setfilename_for_outfile)
      and !$self->get_conf('NO_USE_SETFILENAME')) {
    $output_filepath = $setfilename_for_outfile;
    $document_path = $setfilename_for_outfile;
    $document_path =~ s/\.[^\.]*$//;
  }

  # $document_name is the name of the document, which is the output
  # file basename, $output_filename, without extension.
  my ($document_name, $output_filename, $directories, $suffix);
  # We may be handling setfilename there, so it is not obvious that we
  # want to use fileparse and not consider unixish separators.  However,
  # if this is setfilename, it should be a simple file name, so it
  # should hopefully be harmless to use fileparse
  ($document_name, $directories, $suffix) = fileparse($document_path);
  ($output_filename, $directories, $suffix) = fileparse($output_filepath);
  my $destination_directory;
  if ($self->get_conf('SPLIT')) {
    if (defined($self->get_conf('OUTFILE'))) {
      $destination_directory = $self->get_conf('OUTFILE');
    } elsif (defined($self->get_conf('SUBDIR'))) {
      $destination_directory = $self->get_conf('SUBDIR');
    } else {
      $destination_directory = $document_name;
      if (defined($output_format) and $output_format ne '') {
        $destination_directory .= '_'.$output_format;
      }
    }
  } else {
    # $output_file_filename is not used, but $output_filename should be
    # the same as long as $output_file is the same as $output_filepath
    # which is the case except if $output_file is ''.
    my ($output_file_filename, $output_dir, $suffix) = fileparse($output_file);
    if ($output_dir ne '') {
      $destination_directory = $output_dir;
    }
  }
  if (defined($destination_directory)
      and $destination_directory ne '') {
    $destination_directory = File::Spec->canonpath($destination_directory);
  }
  return ($output_file, $destination_directory, $output_filename,
          $document_name, $input_basefile);
}

sub _id_to_filename($$)
{
  my $self = shift;
  my $id = shift;
  return substr($id, 0, $self->get_conf('BASEFILENAME_LENGTH'));
}

sub normalized_sectioning_command_filename($$)
{
  my $self = shift;
  my $command = shift;
  my $no_unidecode;

  $no_unidecode = 1 if (defined($self->get_conf('USE_UNIDECODE'))
                        and !$self->get_conf('USE_UNIDECODE'));

  my $normalized_name
    = Texinfo::Convert::NodeNameNormalization::transliterate_texinfo(
         {'contents' => $command->{'args'}->[0]->{'contents'}},
                  $no_unidecode);

  my $filename = $self->_id_to_filename($normalized_name);
  $filename .= '.'.$self->get_conf('EXTENSION')
    if (defined($self->get_conf('EXTENSION'))
      and $self->get_conf('EXTENSION') ne '');
  return ($normalized_name, $filename);
}

sub node_information_filename($$)
{
  my $self = shift;
  my $node_info = shift;

  my $no_unidecode;
  $no_unidecode = 1 if (defined($self->get_conf('USE_UNIDECODE'))
                        and !$self->get_conf('USE_UNIDECODE'));

  my $filename;
  if (defined($node_info->{'normalized'})) {
    if ($self->get_conf('TRANSLITERATE_FILE_NAMES')) {
      $filename = Texinfo::Convert::NodeNameNormalization::transliterate_texinfo(
       {'contents' => $node_info->{'node_content'}},
            $no_unidecode);
    } else {
      $filename = $node_info->{'normalized'};
    }
  } elsif (defined($node_info->{'node_content'})) {
    $filename = Texinfo::Convert::NodeNameNormalization::normalize_node(
             { 'contents' => $node_info->{'node_content'} });
  } else {
    $filename = '';
  }
  $filename = $self->_id_to_filename($filename);
  return $filename;
}

sub initialize_tree_units_files($)
{
  my $self = shift;

  $self->{'out_filepaths'} = {};
  $self->{'file_counters'} = {};

  if ($self->get_conf('CASE_INSENSITIVE_FILENAMES')) {
    $self->{'filenames'} = {};
  }
}

# If CASE_INSENSITIVE_FILENAMES is set, reuse the first
# filename with the same name insensitive to the case.
sub _normalize_filename_case($$)
{
  my $self = shift;
  my $filename = shift;

  if ($self->get_conf('CASE_INSENSITIVE_FILENAMES')) {
    if (exists($self->{'filenames'}->{lc($filename)})) {
      if ($self->get_conf('DEBUG')) {
        print STDERR "Reusing ".$self->{'filenames'}->{lc($filename)}
                     ." for $filename\n";
      }
      $filename = $self->{'filenames'}->{lc($filename)};
    } else {
      $self->{'filenames'}->{lc($filename)} = $filename;
    }
  }
  return $filename;
}

# Sets $tree_unit->{'structure'}->{'unit_filename'}.
sub set_tree_unit_file($$$)
{
  my $self = shift;
  my $tree_unit = shift;
  my $filename = shift;

  if (!defined($filename)) {
    cluck("set_tree_unit_file: filename not defined\n");
  }

  $filename = $self->_normalize_filename_case($filename);

  $tree_unit->{'structure'} = {} if (!($tree_unit->{'structure'}));

  # This should never happen, set_tree_unit_file is called once per
  # tree unit.
  if (exists($tree_unit->{'structure'}->{'unit_filename'})) {
    if ($tree_unit->{'structure'}->{'unit_filename'} eq $filename) {
      print STDERR "set_tree_unit_file: already set: $filename\n"
         if ($self->get_conf('DEBUG'));
    } else {
      print STDERR  "set_tree_unit_file: unit_filename reset: "
        .$tree_unit->{'structure'}->{'unit_filename'}.", $filename\n"
           if ($self->get_conf('DEBUG'));
    }
  }
  $tree_unit->{'structure'}->{'unit_filename'} = $filename;
}

# sets out_filepaths converter state, associating a file name
# to a file path.
# $FILEPATH can be given explicitly, otherwise it is based on $FILENAME
# and $DESTINATION_DIRECTORY
sub set_file_path($$$;$)
{
  my $self = shift;
  my $filename = shift;
  my $destination_directory = shift;
  my $filepath = shift;

  if (!defined($filename)) {
    cluck("set_file_path: filename not defined\n");
  }

  $filename = $self->_normalize_filename_case($filename);

  if (not defined($filepath)) {
    if (defined($destination_directory) and $destination_directory ne '') {
      $filepath = File::Spec->catfile($destination_directory, $filename);
    } else {
      $filepath = $filename;
    }
  }
  # should not happen, the file path should be set only once
  # per file name.
  if (defined($self->{'out_filepaths'}->{$filename})) {
    if ($self->{'out_filepaths'}->{$filename} eq $filepath) {
      print STDERR "set_file_path: filepath set: $filepath\n"
         if ($self->get_conf('DEBUG'));
    } else {
      print STDERR  "set_file_path: filepath reset: "
        .$self->{'out_filepaths'}->{$filename}.", $filepath\n"
         if ($self->get_conf('DEBUG'));
    }
  }
  $self->{'out_filepaths'}->{$filename} = $filepath;
}

sub top_node_filename($$)
{
  my $self = shift;
  my $document_name = shift;

  my $top_node_filename;
  if (defined($self->get_conf('TOP_FILE'))
      and $self->get_conf('TOP_FILE') ne '') {
    $top_node_filename = $self->get_conf('TOP_FILE');
  } else {
    my $extension = '';
    $extension = '.'.$self->get_conf('EXTENSION')
      if (defined($self->get_conf('EXTENSION'))
            and $self->get_conf('EXTENSION') ne '');

    $top_node_filename = $document_name;
    if (defined($top_node_filename)) {
      $top_node_filename .= $extension;
    }
  }
  return $top_node_filename;
}

sub _get_root_element($$)
{
  my $self = shift;
  my $command = shift;
  my $find_container = shift;

  my $current = $command;

  while (1) {
    if ($current->{'type'}) {
      if ($current->{'type'} eq 'unit') {
        return $current;
      }
    }
    if ($current->{'parent'}) {
      $current = $current->{'parent'};
    } else {
      return undef;
    }
  }
}

# set file_counters converter state
sub _set_tree_units_files($$$$$$)
{
  my $self = shift;
  my $tree_units = shift;
  my $output_file = shift;
  my $destination_directory = shift;
  my $output_filename = shift;
  my $document_name = shift;

  # Ensure that the document has pages
  return undef if (!defined($tree_units) or !@$tree_units);

  $self->initialize_tree_units_files();

  my $extension = '';
  $extension = '.'.$self->get_conf('EXTENSION')
            if (defined($self->get_conf('EXTENSION'))
                and $self->get_conf('EXTENSION') ne '');

  if (!$self->get_conf('SPLIT')) {
    $self->set_file_path($output_filename, undef, $output_file);
    foreach my $tree_unit (@$tree_units) {
      $self->set_tree_unit_file($tree_unit, $output_filename);
    }
  } else {
    my $node_top;
    $node_top = $self->{'labels'}->{'Top'} if ($self->{'labels'});
  
    my $top_node_filename = $self->top_node_filename($document_name);
    # first determine the top node file name.
    if ($node_top and defined($top_node_filename)) {
      my ($node_top_unit) = $self->_get_root_element($node_top);
      die "BUG: No element for top node" if (!defined($node_top));
      $self->set_file_path($top_node_filename, $destination_directory);
      $self->set_tree_unit_file($node_top_unit, $top_node_filename);
    }
    my $file_nr = 0;
    my $previous_page;
    foreach my $tree_unit (@$tree_units) {
      # For Top node.
      next if ($tree_unit->{'structure'}
               and defined($tree_unit->{'structure'}->{'unit_filename'}));
      my $file_tree_unit = $tree_unit->{'extra'}->{'first_in_page'};
      if (!$file_tree_unit) {
        cluck ("No first_in_page for $tree_unit\n");
      }
      if (!defined($file_tree_unit->{'structure'}->{'unit_filename'})) {
        foreach my $root_command (@{$file_tree_unit->{'contents'}}) {
          if ($root_command->{'cmdname'}
              and $root_command->{'cmdname'} eq 'node') {
            my $node_filename;
            # double node are not normalized, they are handled here
            if (!defined($root_command->{'extra'}->{'normalized'})
                or !defined($self->{'labels'}->{$root_command->{'extra'}->{'normalized'}})) {
              $node_filename = 'unknown_node';
            } else {
              $node_filename
               = $self->node_information_filename($root_command->{'extra'});
            }
            $node_filename .= $extension;
            $self->set_file_path($node_filename,$destination_directory);
            $self->set_tree_unit_file($file_tree_unit, $node_filename);
            last;
          }
        }
        if (!defined($file_tree_unit->{'structure'}->{'unit_filename'})) {
          # use section to do the file name if there is no node
          my $command = $file_tree_unit->{'extra'}->{'unit_command'};
          if ($command) {
            if ($command->{'cmdname'} eq 'top' and !$node_top
                and defined($top_node_filename)) {
              $self->set_file_path($top_node_filename, $destination_directory);
              $self->set_tree_unit_file($file_tree_unit, $top_node_filename);
            } else {
              my ($normalized_name, $filename)
                 = $self->normalized_sectioning_command_filename($command);
              $self->set_file_path($filename, $destination_directory);
              $self->set_tree_unit_file($file_tree_unit, $filename);
            }
          } else {
            # when everything else has failed
            if ($file_nr == 0 and !$node_top and defined($top_node_filename)) {
              $self->set_file_path($top_node_filename, $destination_directory);
              $self->set_tree_unit_file($file_tree_unit, $top_node_filename);
            } else {
              my $filename = $document_name . "_$file_nr";
              $filename .= $extension;
              $self->set_file_path($filename, $destination_directory);
              $self->set_tree_unit_file($tree_unit, $filename);
            }
            $file_nr++;
          }
        }
      }
      $self->set_tree_unit_file($tree_unit,
                    $file_tree_unit->{'structure'}->{'unit_filename'});
    }
  }

  foreach my $tree_unit (@$tree_units) {
    my $tree_unit_filename = $tree_unit->{'structure'}->{'unit_filename'};
    $self->{'file_counters'}->{$tree_unit_filename} = 0
       if (!exists($self->{'file_counters'}->{$tree_unit_filename}));
    $self->{'file_counters'}->{$tree_unit_filename}++;
    print STDERR "Page $tree_unit "
     .Texinfo::Structuring::root_or_external_element_cmd_texi($tree_unit)
     .": $tree_unit_filename($self->{'file_counters'}->{$tree_unit_filename})\n"
              if ($self->get_conf('DEBUG'));
  }
}

sub create_destination_directory($$$)
{
  my $self = shift;
  my $destination_directory_path = shift;
  my $destination_directory_name = shift;

  if (defined($destination_directory_path)
      and ! -d $destination_directory_path) {
    if (!mkdir($destination_directory_path, oct(755))) {
      $self->document_error($self, sprintf(__(
                                "could not create directory `%s': %s"),
                                   $destination_directory_name, $!));
      return 0;
    }
  }
  return 1;
}


#############################################################
# useful methods for Converters.

# determine the default, with $INIT_CONF if set, or the default common
# to all the converters
sub _command_init($$)
{
  my $global_command = shift;
  my $init_conf = shift;
  if (exists($Texinfo::Common::document_settable_at_commands{$global_command})) {
    if (defined($init_conf->{$global_command})) {
      return $init_conf->{$global_command};
    } elsif (defined($Texinfo::Common::document_settable_at_commands{$global_command})) {
      return $Texinfo::Common::document_settable_at_commands{$global_command};
    }
  }
}

# $COMMANDS_LOCATION is 'before', 'last', 'preamble' or 'preamble_or_first'
# 'before' means setting to the values before the document commands
# (defaults and command-line).
# 'preamble' means setting sequentially to the values in the preamble.
# 'preamble_or_first'  means setting to the first value for the command
# in the document if the first command is not in the preamble, else set
# sequentially to the values in the preamble.
# 'last' means setting to the last value for the command in the document.
#
# Notice that the only effect is to use set_conf (directly or through
# set_global_document_command), no @-commands setting side effects are done
# and associated customization variables are not set/reset either.
sub set_global_document_commands($$;$)
{
  my $self = shift;
  my $commands_location = shift;
  my $selected_commands = shift;

  my $init_conf;
  if (defined($self->{'output_init_conf'})) {
    # use in priority the initial customization per output
    $init_conf = $self->{'output_init_conf'};
  } else {
    $init_conf = $self->{'converter_init_conf'};
  }

  if (not defined($selected_commands)) {
    $selected_commands = [keys(%Texinfo::Common::document_settable_at_commands)];
  }
  if ($commands_location eq 'before') {
    foreach my $global_command (@{$selected_commands}) {
      # for commands not appearing in the document, this should set the
      # same value, the converter initialization value
      $self->set_conf($global_command, _command_init($global_command, $init_conf));
      # NOTE if the variable is set from an handler, or in the converter after
      # $init_conf was set, but before starting the conversion, it is ignored here
      # and the $init_conf value is set.  The previously set value could be
      # in $self->get_conf(), but what is available from $self->get_conf() could
      # also be a value set by a previous call of set_global_document_commands.
      # There is no easy way to deal with this issue, other than making sure that
      # a customization value that is expected to be set early is set in $init_conf.
    }
  } else {
    foreach my $global_command (@{$selected_commands}) {
      if ($self->get_conf('DEBUG')) {
        print STDERR "SET_global($commands_location) $global_command\n";
      }
      my $element = Texinfo::Common::set_global_document_command($self,
               $self->{'global_commands'}, $global_command, $commands_location);
      if (not defined($element)) {
        # commands not appearing in the document, this should set the
        # same value, the converter initialization value
        # the NOTE above in 'before' holds here too.
        $self->set_conf($global_command,
                        _command_init($global_command, $init_conf));
      }
    }
  }
}

sub present_bug_message($$;$)
{
  my $self = shift;
  my $message = shift;
  my $current = shift;

  my $line_message = '';
  my $current_element_message = '';
  if ($current) {
    if ($current->{'source_info'}) {
      my $source_info = $current->{'source_info'};
      my $file = $source_info->{'file_name'};
      $line_message
        = "in: $source_info->{'file_name'}:$source_info->{'line_nr'}";
      if ($source_info->{'macro'} ne '') {
        $line_message .= " (possibly involving $source_info->{'macro'})";
      }
      $line_message .= "\n";
    }
    if ($current) {
      $current_element_message = "current: ".
        Texinfo::Common::debug_print_element($current);
    }
  }
  my $additional_information = '';
  if ($line_message.$current_element_message ne '') {
    $additional_information = "Additional information:\n".
       $line_message.$current_element_message;
  }
  warn "You found a bug: $message\n\n".$additional_information;
}

# Reverse the decoding of the file name from the input encoding.
sub encoded_input_file_name($$)
{
  my $self = shift;
  my $file_name = shift;

  my $encoding;
  my $input_file_name_encoding = $self->get_conf('INPUT_FILE_NAME_ENCODING');
  if ($input_file_name_encoding) {
    $encoding = $input_file_name_encoding;
  } elsif ($self->get_conf('DOC_ENCODING_FOR_INPUT_FILE_NAME')) {
    $encoding = $self->{'parser_info'}->{'input_perl_encoding'}
      if ($self->{'parser_info'}
        and defined($self->{'parser_info'}->{'input_perl_encoding'}));
  } else {
    $encoding = $self->get_conf('LOCALE_ENCODING');
  }

  return Texinfo::Common::encode_file_name($file_name, $encoding);
}

# A wrapper around Texinfo::Utils::encoded_output_file_name() that
# can be called in converters through an objet oriented syntax.
sub encoded_output_file_name($$)
{
  my $self = shift;
  my $file_name = shift;
  return Texinfo::Convert::Utils::encoded_output_file_name($self, $file_name);
}

sub txt_image_text($$$)
{
  my ($self, $element, $basefile) = @_;

  my ($text_file_name, $file_name_encoding)
    = $self->encoded_input_file_name($basefile.'.txt');

  my $txt_file = Texinfo::Common::locate_include_file($self, $text_file_name);
  if (!defined($txt_file)) {
    return undef;
  } else {
    my $filehandle = do { local *FH };
    if (open ($filehandle, $txt_file)) {
      my $enc = $element->{'extra'}->{'input_perl_encoding'};
      binmode($filehandle, ":encoding($enc)")
        if ($enc);
      my $result = '';
      my $max_width = 0;
      while (<$filehandle>) {
        my $width = Texinfo::Convert::Unicode::string_width($_);
        if ($width > $max_width) {
          $max_width = $width;
        }
        $result .= $_;
      }
      # remove last end of line
      chomp ($result);
      if (!close ($filehandle)) {
        $self->document_warn($self,
           sprintf(__("error on closing image text file %s: %s"),
                                     $txt_file, $!));
      }
      return ($result, $max_width);
    } else {
      $self->line_warn($self,
                  sprintf(__("\@image file `%s' unreadable: %s"),
                               $txt_file, $!), $element->{'source_info'});
    }
  }
  return undef;
}


sub float_type_number($$)
{
  my $self = shift;
  my $float = shift;

  my $type;
  if ($float->{'extra'}->{'type'}
      and $float->{'extra'}->{'type'}->{'normalized'} ne '') {
    $type = {'contents' => $float->{'extra'}->{'type'}->{'content'}};
  }

  my $tree;
  if ($type) {
    if (defined($float->{'structure'}->{'float_number'})) {
      $tree = $self->gdt("{float_type} {float_number}",
          {'float_type' => $type,
            'float_number' => $float->{'structure'}->{'float_number'}});
    } else {
      $tree = $self->gdt("{float_type}",
          {'float_type' => $type});
    }
  } elsif (defined($float->{'structure'}->{'float_number'})) {
    $tree = $self->gdt("{float_number}",
       {'float_number' => $float->{'structure'}->{'float_number'}});
  }
  return $tree;
}

sub float_name_caption($$)
{
  my $self = shift;
  my $element = shift;

  my $caption;
  if ($element->{'extra'}->{'caption'}) {
    $caption = $element->{'extra'}->{'caption'};
  } elsif ($element->{'extra'}->{'shortcaption'}) {
    $caption = $element->{'extra'}->{'shortcaption'};
  }
  #if ($self->get_conf('DEBUG')) {
  #  my $caption_texi =
  #    Texinfo::Convert::Texinfo::convert_to_texinfo({ 'contents' => $caption->{'contents'}});
  #  print STDERR "  CAPTION: $caption_texi\n";
  #}
  my $type;
  if ($element->{'extra'}->{'type'}->{'normalized'} ne '') {
    $type = {'contents' => $element->{'extra'}->{'type'}->{'content'}};
  }

  my $prepended;
  if ($type) {
    if ($caption) {
      if ($element->{'structure'}
          and defined($element->{'structure'}->{'float_number'})) {
        $prepended = $self->gdt('{float_type} {float_number}: ',
            {'float_type' => $type,
             'float_number' => $element->{'structure'}->{'float_number'}});
      } else {
        $prepended = $self->gdt('{float_type}: ',
          {'float_type' => $type});
      }
    } else {
      if ($element->{'structure'}
          and defined($element->{'structure'}->{'float_number'})) {
        $prepended = $self->gdt("{float_type} {float_number}\n",
            {'float_type' => $type,
              'float_number' => $element->{'structure'}->{'float_number'}});
      } else {
        $prepended = $self->gdt("{float_type}\n",
            {'float_type' => $type});
      }
    }
  } elsif ($element->{'structure'}
           and defined($element->{'structure'}->{'float_number'})) {
    if ($caption) {
      $prepended = $self->gdt('{float_number}: ',
          {'float_number' => $element->{'structure'}->{'float_number'}});
    } else {
      $prepended = $self->gdt("{float_number}\n",
           {'float_number' => $element->{'structure'}->{'float_number'}});
    }
  }
  return ($caption, $prepended);
}

# This is used when the formatted text has no comment nor new line, but
# one want to add the comment or new line from the original arg
sub format_comment_or_return_end_line($$)
{
  my $self = shift;
  my $element = shift;

  my $end_line;

  my $comment = $element->{'args'}->[-1]->{'extra'}->{'comment_at_end'}
    if $element->{'args'} and $element->{'args'}->[-1]->{'extra'};

  if ($comment) {
    $end_line = $self->convert_tree($comment);
  } elsif ($element->{'args'} and $element->{'args'}->[-1]->{'extra'}
      and $element->{'args'}->[-1]->{'extra'}->{'spaces_after_argument'}) {
    my $text = $element->{'args'}->[-1]->{'extra'}->{'spaces_after_argument'};
    if (chomp($text)) {
      $end_line = "\n";
    } else {
      $end_line = '';
    }
  } else {
    $end_line = '';
  }
  return $end_line;
}

sub table_item_content_tree($$$)
{
  my $self = shift;
  my $element = shift;
  my $contents = shift;

  my $table_item_tree = {'parent' => $element};

  return $table_item_tree
    if (!defined($contents));

  my $table_command = $element->{'parent'}->{'parent'}->{'parent'};
  if ($table_command->{'extra'}
     and $table_command->{'extra'}->{'command_as_argument'}) {
    my $command_as_argument
      = $table_command->{'extra'}->{'command_as_argument'};
    my $command = {'cmdname' => $command_as_argument->{'cmdname'},
               'source_info' => $element->{'source_info'},
               'parent' => $table_item_tree };
    if ($table_command->{'extra'}->{'command_as_argument_kbd_code'}) {
      $command->{'extra'} = {} if (!$command->{'extra'});
      $command->{'extra'}->{'code'} = 1;
    }
    if ($command_as_argument->{'type'} eq 'definfoenclose_command') {
      $command->{'type'} = $command_as_argument->{'type'};
      $command->{'extra'} = {} if (!$command->{'extra'});
      $command->{'extra'}->{'begin'} = $command_as_argument->{'extra'}->{'begin'};
      $command->{'extra'}->{'end'} = $command_as_argument->{'extra'}->{'end'};
    }
    my $arg = {'type' => 'brace_command_arg',
               'contents' => $contents,
               'parent' => $command,};
    $command->{'args'} = [$arg];
    $contents = [$command];
  }
  $table_item_tree->{'contents'} = $contents;
  return $table_item_tree;
}

sub convert_accents($$$;$)
{
  my $self = shift;
  my $accent = shift;
  my $format_accents = shift;
  my $in_upper_case = shift;

  my ($contents, $stack)
      = Texinfo::Convert::Utils::find_innermost_accent_contents($accent);
  my $result = $self->convert_tree({'contents' => $contents});

  my $encoded;
  if ($self->get_conf('ENABLE_ENCODING')) {
    $encoded = Texinfo::Convert::Unicode::encoded_accents($self, $result, $stack,
                                       $self->get_conf('OUTPUT_ENCODING_NAME'),
                                       $format_accents,
                                       $in_upper_case);
  }
  if (!defined($encoded)) {
    foreach my $accent_command (reverse(@$stack)) {
      $result = &$format_accents ($self, $result, $accent_command,
                                  $in_upper_case);
    }
    return $result;
  } else {
    return $encoded;
  }
}

# index sub-entries specified with @subentry, separated by commas.
sub comma_index_subentries_tree {
  my ($self, $entry) = @_;

  my @contents;
  my $tmp = $entry->{'entry_element'};
  while ($tmp->{'extra'} and $tmp->{'extra'}->{'subentry'}) {
    $tmp = $tmp->{'extra'}->{'subentry'};
    push @contents, {'text' => ', '}, $tmp->{'args'}->[0];
  }
  if (scalar(@contents)) {
    return {'contents' => \@contents};
  }
  return undef;
}

# This method allows to count words in elements and returns an array
# and a text already formatted.
sub sort_element_counts($$;$$)
{
  my $converter =  shift;
  my $tree = shift;
  my $use_sections = shift;
  my $count_words = shift;

  my $elements;
  if ($use_sections) {
    $elements = Texinfo::Structuring::split_by_section($tree);
  } else {
    $elements = Texinfo::Structuring::split_by_node($tree);
  }

  if (!$elements) {
    @$elements = ($tree);
  } elsif (scalar(@$elements) >= 1
           and not $elements->[0]->{'extra'}->{'unit_command'}) {
    shift @$elements;
  }

  my $max_count = 0;
  my @name_counts_array;

  require Texinfo::Convert::Texinfo;
  foreach my $element (@$elements) {
    my $name = 'UNNAMED tree element';
    if ($element->{'extra'} and $element->{'extra'}->{'unit_command'}) {
      my $command = $element->{'extra'}->{'unit_command'};
      if ($command->{'cmdname'} eq 'node') {
        $name = Texinfo::Convert::Texinfo::convert_to_texinfo(
    {'contents' => $command->{'extra'}->{'nodes_manuals'}->[0]->{'node_content'}});
      } else {
        $name = "\@$command->{'cmdname'}"
          .Texinfo::Convert::Texinfo::convert_to_texinfo($command->{'args'}->[0]);
      }
    }
    chomp($name);
    my $count;
    my $converted_element = $converter->convert_tree($element);
    if ($count_words) {
      my @res = split /\W+/, $converted_element;
      $count = scalar(@res);
    } else {
      my @res = split /^/, $converted_element;
      $count = scalar(@res);
    }
    push @name_counts_array, [$count, $name];
    if ($count > $max_count) {
      $max_count = $count;
    }
  }

  my @sorted_name_counts_array = sort {$a->[0] <=> $b->[0]} @name_counts_array;
  @sorted_name_counts_array = reverse(@sorted_name_counts_array);

  my $max_length = length($max_count);
  my $result = '';
  foreach my $sorted_count (@sorted_name_counts_array) {
    $result .=  sprintf("%${max_length}d  $sorted_count->[1]\n",
                        $sorted_count->[0]);
  }
  return (\@sorted_name_counts_array, $result);
}


########################################################################
# XML related methods and variables that may be used in different
# XML Converters.

my $xml_numeric_entity_mdash = '&#'.hex('2014').';';
my $xml_numeric_entity_ndash = '&#'.hex('2013').';';
my $xml_numeric_entity_ldquo = '&#'.hex('201C').';';
my $xml_numeric_entity_rdquo = '&#'.hex('201D').';';
my $xml_numeric_entity_lsquo = '&#'.hex('2018').';';
my $xml_numeric_entity_rsquo = '&#'.hex('2019').';';

sub xml_format_text_with_numeric_entities($$)
{
  my $self = shift;
  my $text = shift;
 
  $text =~ s/``/$xml_numeric_entity_ldquo/g;
  $text =~ s/\'\'/$xml_numeric_entity_rdquo/g;
  $text =~ s/`/$xml_numeric_entity_lsquo/g;
  $text =~ s/\'/$xml_numeric_entity_rsquo/g;
  $text =~ s/---/$xml_numeric_entity_mdash/g;
  $text =~ s/--/$xml_numeric_entity_ndash/g;

  return $text;
}

sub xml_protect_text($$)
{
  my $self = shift;
  my $text = shift;
  if (!defined($text)) {
    cluck;
  }
  $text =~ s/&/&amp;/g;
  $text =~ s/</&lt;/g;
  $text =~ s/>/&gt;/g;
  $text =~ s/\"/&quot;/g;
  return $text;
}

# 'today' is not set here.
our %xml_text_entity_no_arg_commands_formatting = (
               'TeX'          => 'TeX',
               'LaTeX'          => 'LaTeX',
               'bullet'       => '&bull;',
               'copyright'    => '&copy;',
               'registeredsymbol'   => '&reg;',
               'dots'         => '&hellip;',
               'enddots'      => '...',
               'equiv'        => '&equiv;',
               # in general the following is not used since error
               # appears in 'translated_commands'
               'error'        => 'error--&gt;',
               'expansion'    => '&rarr;',
               'arrow'        => '&rarr;',
               'click'        => '&rarr;',
               'minus'        => '&minus;',
               'point'        => '&lowast;',
               'print'        => '-|',
               'result'       => '&rArr;',
               'aa'           => '&aring;',
               'AA'           => '&Aring;',
               'ae'           => '&aelig;',
               'oe'           => '&oelig;', # &oelig; not in html 3.2
               'AE'           => '&AElig;',
               'OE'           => '&OElig;', # &OElig; not in html 3.2
               'o'            => '&oslash;',
               'O'            => '&Oslash;',
               'ss'           => '&szlig;',
               'DH'           => '&ETH;',
               'dh'           => '&eth;',
               'TH'           => '&THORN;',
               'th'           => '&thorn;',
               'l'            => '&#322;',
               'L'            => '&#321;',
               'exclamdown'   => '&iexcl;',
               'questiondown' => '&iquest;',
               'pounds'       => '&pound;',
               'ordf'         => '&ordf;',
               'ordm'         => '&ordm;',
               'comma'        => ',',
               'atchar'       => '@',
               'ampchar'      => '&amp;',
               'lbracechar'   => '{',
               'rbracechar'   => '}',
               'backslashchar' => '\\',
               'hashchar' => '#',
               'euro'         => '&euro;',
               'geq'          => '&ge;',
               'leq'          => '&le;',
               'tie'          => '&nbsp;',
               'textdegree'          => '&deg;',
               'quotedblleft'          => '&ldquo;',
               'quotedblright'          => '&rdquo;',
               'quoteleft'          => '&lsquo;',
               'quoteright'          => '&rsquo;',
               'quotedblbase'          => '&bdquo;',
               'quotesinglbase'          => '&sbquo;',
               'guillemetleft'          => '&laquo;',
               'guillemetright'          => '&raquo;',
               'guillemotleft'          => '&laquo;',
               'guillemotright'          => '&raquo;',
               'guilsinglleft'          => '&lsaquo;',
               'guilsinglright'          => '&rsaquo;',
);

foreach my $no_brace_command (keys(%Texinfo::Common::nobrace_symbol_text)) {
  $xml_text_entity_no_arg_commands_formatting{$no_brace_command}
    = $Texinfo::Common::nobrace_symbol_text{$no_brace_command};
}

$xml_text_entity_no_arg_commands_formatting{'&'} = '&amp;';

sub xml_comment($$)
{
  my $self = shift;
  my $text = shift;
  chomp $text;
  $text =~ s/--+/-/go;
  return '<!--' . $text . ' -->' . "\n";
}

our %xml_accent_entities = (
          '"',  'uml',
          '~',  'tilde',
          '^',  'circ',
          '`',  'grave',
          "'", 'acute',
          ",", 'cedil',
          'ringaccent', 'ring',
          'ogonek', 'ogon',
          'dotless', 'nodot',
         );

our %xml_accent_text_with_entities = (
      'ringaccent' => 'aA',
      "'"          => 'aeiouyAEIOUY',
      ','          => 'cC',
      '^'          => 'aeiouAEIOU',
      '`'          => 'aeiouAEIOU',
      '~'          => 'nNaoAO',
      '"'          => 'aeiouyAEIOU',
      'dotless'    => 'i',
# according to http://www2.lib.virginia.edu/small/vhp/download/ISO.txt
# however this doesn't seems to work in firefox
#      'ogonek'     => 'aeiuAEIU',
);

sub xml_numeric_entity_accent($$)
{
  my $accent = shift;
  my $text = shift;

  if (exists($Texinfo::Convert::Unicode::unicode_accented_letters{$accent})
      and exists($Texinfo::Convert::Unicode::unicode_accented_letters{$accent}->{$text})) {
    return '&#' .
      hex($Texinfo::Convert::Unicode::unicode_accented_letters{$accent}->{$text}). ';';
  }
  if (exists($Texinfo::Convert::Unicode::unicode_diacritics{$accent})) {
    my $diacritic = '&#'
       .hex($Texinfo::Convert::Unicode::unicode_diacritics{$accent}). ';';
    if ($accent ne 'tieaccent') {
      return $text . $diacritic;
    } else {
      # tieaccent diacritic is naturally and correctly composed
      # between two characters
      my $remaining_text = $text;
      # we consider that letters are either characters or entities
      if ($remaining_text =~ s/^([\p{L}\d]|&[a-zA-Z0-9]+;)([\p{L}\d]|&[a-zA-Z0-9]+;)(.*)$/$3/) {
        return $1.$diacritic.$2 . $remaining_text;
      } else {
        return $text . $diacritic;
      }
    }
  }
  return undef;
}

sub xml_accent($$$;$$$)
{
  my $self = shift;
  my $text = shift;
  my $command = shift;
  my $in_upper_case = shift;
  my $use_numeric_entities = shift;
  my $accent = $command->{'cmdname'};
  
  if ($in_upper_case and $text =~ /^\w$/) {
    $text = uc ($text);
  }

  # do not return a dotless i or j as such if it is further composed
  # with an accented letter, return the letter as is
  if ($accent eq 'dotless') {
    if ($Texinfo::Convert::Unicode::unicode_accented_letters{$accent}
        and exists($Texinfo::Convert::Unicode::unicode_accented_letters{$accent}->{$text})
        and ($command->{'parent'}
             and $command->{'parent'}->{'parent'}
             and $command->{'parent'}->{'parent'}->{'cmdname'}
             and $Texinfo::Convert::Unicode::unicode_accented_letters{$command->{'parent'}
                                        ->{'parent'}->{'cmdname'}})) {
      return $text;
    }
  }
 
  if ($use_numeric_entities) {
    my $formatted_accent = xml_numeric_entity_accent($accent, $text);
    if (defined($formatted_accent)) {
      return $formatted_accent;
    }
  } else {
    return "&${text}$xml_accent_entities{$accent};"
      if (defined($xml_accent_entities{$accent})
          and defined($xml_accent_text_with_entities{$accent})
          and ($text =~ /^[$xml_accent_text_with_entities{$accent}]$/));
    my $formatted_accent = xml_numeric_entity_accent($accent, $text);
    if (defined($formatted_accent)) {
      return $formatted_accent;
    }
  }

  # should never happen, there are diacritics for every accent command
  return $text . '&lt;' if ($accent eq 'v');
  # FIXME it is not possible to call xml_protect_text since what is in $text
  # may already be xml.  But this means that each time ascii_accent changes
  # it should be changed here too.
  return Texinfo::Convert::Text::ascii_accent($text, $command);
}

sub _xml_numeric_entities_accent($$$;$)
{
  my $self = shift;
  my $text = shift;
  my $command = shift;
  my $in_upper_case = shift;

  return xml_accent($self, $text, $command, $in_upper_case, 1);
}

sub xml_accents($$;$)
{
  my $self = shift;
  my $accent = shift;
  my $in_upper_case = shift;

  my $format_accents;
  if ($self->get_conf('USE_NUMERIC_ENTITY')) {
    $format_accents = \&_xml_numeric_entities_accent;
  } else {
    $format_accents = \&xml_accent;
  }
  
  return $self->convert_accents($accent, $format_accents, $in_upper_case);
}

1;

__END__

=head1 NAME

Texinfo::Convert::Converter - Parent class for Texinfo tree converters

=head1 SYNOPSIS

  package Texinfo::Convert::MyConverter;

  use Texinfo::Convert::Converter;
  @ISA = qw(Texinfo::Convert::Converter);

  sub converter_defaults ($$) {
    return %myconverter_defaults;
  }
  sub converter_initialize($) {
    my $self = shift;
    $self->{'document_context'} = [{}];
  }

  sub convert($$) {
    ...
  }
  sub convert_tree($$) {
    ...
  }
  sub output($$) {
    ...
  }

  # end of Texinfo::Convert::MyConverter

  my $converter = Texinfo::Convert::MyConverter->converter(
                                               {'parser' => $parser});
  $converter->output($texinfo_tree);

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

C<Texinfo::Convert::Converter> is a super class that can be used to
simplify converters initialization.  The class also provide some
useful methods.

In turn, the converter should define some methods.  Two are
optional, C<converter_defaults>, C<converter_initialize> and
used for initialization, to give information to C<Texinfo::Convert::Converter>.

X<C<convert_tree>> X<C<output>> X<C<convert>>
The C<convert_tree> method is mandatory and should convert portions of Texinfo
tree.  The C<output> method is used by converters as entry point for conversion
to a file with headers and so on.  Although it is is not called from other
modules, it should in general be implemented by converters. C<output> is called
from C<texi2any>.  C<convert> is not required, but customarily used by
converters as entry point for a conversion of a whole Texinfo tree without
the headers done when outputting to a file.

Existing backends may be used as examples that implement those
methods.  C<Texinfo::Convert::Texinfo> together with
C<Texinfo::Convert::PlainTexinfo>, as well as
C<Texinfo::Convert::TextContent> are trivial examples.
C<Texinfo::Convert::Text> is less trivial, although still simple,
while C<Texinfo::Convert::DocBook> is a real converter
that is also not too complex.

The documentation of L<Texinfo::Common>, L<Texinfo::Convert::Unicode>
and L<Texinfo::Report> describes modules or additional function
that may be useful for backends, while the parsed Texinfo tree is
described in L<Texinfo::Parser>.


=head1 METHODS

=head2 Initialization

X<C<converter>>
X<C<Texinfo::Convert::Converter> initialization>

A module subclassing C<Texinfo::Convert::Converter> is created by calling
the C<converter> method that should be inherited from
C<Texinfo::Convert::Converter>.

=over

=item $converter = MyConverter->converter($options)

The I<$options> hash reference holds options for the converter.  In
this option hash reference a L<parser object|Texinfo::Parser>
may be associated with the I<parser> key.  The other options
are Texinfo customization options and a few other options that can
be passed to the converter. Most of the customization options
are described in the Texinfo manual.
Those customization options, when appropriate, override the document content.
B<TODO what about the other options (all are used in converters;
'structuring' is available in HTML $converter-E<gt>get_info()?>
The parser should not be available directly anymore after getting the
associated information. B<TODO document this associated information
('parser_info', 'indices_information', 'floats'..., most available
in HTML converter, either through $converter-E<gt>get_info() or label_command())>

The C<converter> function returns a converter object (a blessed hash
reference) after checking the options and performing some initializations,
especially when a parser is given among the options.  The converter is
also initialized as a L<Texinfo::Report>.

=back

To help with these initializations, the modules subclassing C<Texinfo::Convert::Converter>
can define two methods:

=over

=item %defaults = $converter->converter_defaults($options)
X<C<converter_defaults>>

The module can provide a defaults hash for converter customization options.
The I<$options> hash reference holds options for the converter.

=item converter_initialize
X<C<converter_initialize>>

This method is called at the end of the C<Texinfo::Convert::Converter>
converter initialization.

=back

=head2 Getting and setting customization variables

C<Texinfo::Convert::Converter> implements a simple interface to
set and retrieve Texinfo customization variables.  Helper
functions from diverse Texinfo modules needing customization
information expect an object implementing C<get_conf> and/or
C<set_conf>.  The converter itself can therefore be used in
such cases.

=over

=item $converter->force_conf($variable_name, $variable_value)
X<C<force_conf>>

Set the Texinfo customization option I<$variable_name> to I<$variable_value>.
This should rarely be used, but the purpose of this method is to be able
to revert a customization that is always wrong for a given output
format, like the splitting for example.

=item $converter->get_conf($variable_name)
X<C<get_conf>>

Returns the value of the Texinfo customization variable I<$variable_name>.

=item $converter->set_conf($variable_name, $variable_value)
X<C<set_conf>>

Set the Texinfo customization option I<$variable_name> to I<$variable_value> if
not set as a converter option.

=back

=head2 Conversion to XML

Some C<Texinfo::Convert::Converter> methods target conversion to XML.
Most methods take a I<$converter> as argument to get some
information and use methods for error reporting.

=over

=item $formatted_text = $converter->xml_format_text_with_numeric_entities($text)
X<C<xml_format_text_with_numeric_entities>>

Replace quotation marks and hyphens used to represent dash in
Texinfo text with numeric XML entities.

=item $protected_text = $converter->xml_protect_text($text)
X<C<xml_protect_text>>

Protect special XML characters (&, E<lt>, E<gt>, ") of I<$text>.

=item $comment = $converter->xml_comment($text)
X<C<xml_comment>>

Returns an XML comment for I<$text>.

=item $result = xml_accent($text, $accent_command, $in_upper_case, $use_numeric_entities)
X<C<xml_accent>>

I<$text> is the text appearing within an accent command.  I<$accent_command>
should be a Texinfo tree element corresponding to an accent command taking
an argument.  I<$in_upper_case> is optional, and, if set, the text is put
in upper case.  The function returns the accented letter as XML named entity
if possible, falling back to numeric entities if there is no named entity
and to an ASCII transliteration as last resort.  I<$use_numeric_entities>
is optional.  If set, numerical entities are used instead of named entities
if possible.

=item $result = $converter->xml_accents($accent_command, $in_upper_case)
X<C<xml_accents>>

I<$accent_command> is an accent command, which may have other accent
commands nested.  If I<$in_upper_case> is set, the result should be
upper cased.  The function returns the accents formatted as XML.

=item $result = xml_numeric_entity_accent($accent_command_name, $text)
X<C<xml_numeric_entity_accent>>

I<$accent_command_name> is the name of an accent command.  I<$text> is the text
appearing within the accent command.  Returns the accented letter as XML numeric
entity, or C<undef> is there is no such entity.

=back

=head2 Helper methods

The module provides methods that may be useful for converter.
Most methods take a I<$converter> as argument to get some
information and use methods for error reporting, see L<Texinfo::Report>.  Also
to translate strings, see L<Texinfo::Translations>.  For
useful methods that need a converter optionally and can be used
in converters that do not inherit from C<Texinfo::Convert::Converter>,
see L<Texinfo::Convert::Utils>.

=over

=item $contents_array = $converter->comma_index_subentries_tree($entry)
X<C<comma_index_subentries_tree>>

I<$entry> is a Texinfo tree index entry element. The function sets up
an array with the C<@subentry> contents, separated by commas.  The
array reference is returned as I<$contents_array>, or C<undef> if there
is no such content.

=item $result = $converter->convert_accents($accent_command, \&format_accents, $in_upper_case)
X<C<convert_accents>>

I<$accent_command> is an accent command, which may have other accent
commands nested.  The function returns the accents formatted either
as encoded letters, or formatted using I<\&format_accents>.
If I<$in_upper_case> is set, the result should be uppercased.

=item $result = $converter->convert_document_sections($root, $file_handler)
X<C<convert_document_sections>>

This method splits the I<$root> Texinfo tree at sections and
calls C<convert_tree> on the elements.  If the optional I<$file_handler>
is given in argument, the result are output in I<$file_handler>, otherwise
the resulting string is returned.

=item $succeeded = $converter->create_destination_directory($destination_directory_path, $destination_directory_name)
X<C<create_destination_directory>>

Create destination directory I<$destination_directory_path>.
I<$destination_directory_path> should be a binary string, while
I<$destination_directory_name> should be a character string, that can be used in
error messages.  I<$succeeded> is true if the creation was successful or
uneeded, false otherwise.

=item ($output_file, $destination_directory, $output_filename, $document_name, $input_basefile) = $converter->determine_files_and_directory($output_format)
X<C<determine_files_and_directory>>

Determine output file and directory, as well as names related to files.  The
result depends on the presence of C<@setfilename>, on the Texinfo input file
name, and on customization options such as C<OUTPUT>, C<SUBDIR> or C<SPLIT>,
as described in the Texinfo manual.  I<$output_format> is optional.  If it is
not set the current output format, if defined, is used instead.  If not an
empty string, C<_$output_format> is prepended to the default directory name.

I<$output_file> is mainly relevant when not split and should be used as the
output file name.  In general, if not split and I<$output_file> is an empty
string, it means that text should be returned by the converter instead of being
written to an output file.  This is used in the test suite.
I<$destination_directory> is either the directory I<$output_file> is in, or if
split, the directory where the files should be created.  I<$output_filename>
is, in general, the file name portion of I<$output_file> (without directory)
but can also be set based on C<@setfilename>, in particular when
I<$output_file> is an empty string. I<$document_name> is I<$output_filename>
without extension.  I<$input_basefile> is based on the input texinfo file name,
with the file name portion only (without directory).

The strings returned are text strings.

=item ($encoded_name, $encoding) = $converter->encoded_input_file_name($character_string_name)

=item ($encoded_name, $encoding) = $converter->encoded_output_file_name($character_string_name)
X<C<encoded_input_file_name>> X<C<encoded_output_file_name>>

Encode I<$character_string_name> in the same way as other file name are
encoded in the converter, based on customization variables, and possibly
on the input file encoding.  Return the encoded name and the encoding
used to encode the name.  The C<encoded_input_file_name> and
C<encoded_output_file_name> functions use different customization variables to
determine the encoding.

Note that C<encoded_output_file_name> is a wrapper around the
function with the same name in L<<< Texinfo::Convert::Utils::encoded_output_file_name|Texinfo::Convert::Utils/($encoded_name, $encoding) = $converter->encoded_output_file_name($converter, $character_string_name) >>>.

=item ($caption, $prepended) = $converter->float_name_caption($float)
X<C<float_name_caption>>

I<$float> is a texinfo tree C<@float> element.  This function
returns the caption element that should be used for the float formatting
and the I<$prepended> texinfo tree combining the type and label
of the float.

=item $tree = $converter->float_type_number($float)
X<C<float_type_number>>

I<$float> is a texinfo tree C<@float> element.  This function
returns the type and number of the float as a texinfo tree with
translations.

=item $end_line = $converter->format_comment_or_return_end_line($element)
X<C<format_comment_or_return_end_line>>

Format comment at end of line or return the end of line associated with
the element.  In many cases, converters ignore comments and output is
better formatted with new lines added independently of the presence
of newline or comment in the initial Texinfo line, so most converters
are better off not using this method.

=item $filename = sub $converter->node_information_filename($node_info)
X<C<node_information_filename>>

Returns the normalized file name corresponding to the I<$node_info>
node element tree C<extra> field.

=item ($normalized_name, $filename) = $converter->normalized_sectioning_command_filename($element)
X<C<normalized_sectioning_command_filename>>

Returns a normalized name I<$normalized_name> corresponding to a sectioning
command tree element I<$element>, expanding the command argument using
transliteration and characters protection.  Also returns I<$filename>
the corresponding filename based on I<$normalized_name> taking into
account additional constraint on file names and adding a file extension.

=item $converter->present_bug_message($message, $element)
X<C<present_bug_message>>

Show a bug message using I<$message> text.  Use information on
I<$element> tree element if given in argument.

=item $converter->set_global_document_commands($commands_location, $selected_commands)
X<C<set_global_document_commands>>

Set the Texinfo customization options for @-commands.  I<$selected_commands>
is an optional array reference containing the @-commands set, if not given
all the global informative @-commands are set.  I<$commands_location> specifies
where in the document the value should be taken from. The possibilities are:

=over

=item before

Set to the values before document conversion, from defaults and command-line.

=item last

Set to the last value for the command.

=item preamble

Set sequentially to the values in the Texinfo preamble.

=item preamble_or_first

Set to the first value of the command if the first command is not
in the Texinfo preamble, else set as with I<preamble>,
sequentially to the values in the Texinfo preamble.

=back

Notice that the only effect of this function is to set a customization
variable value, no @-command side effects are run, no associated customization
variables are set.

For more information on the function used to set the value for each of the command, see
L<Texinfo::Common set_global_document_command|Texinfo::Common/$element = set_global_document_command($customization_information, $global_commands_information, $cmdname, $command_location)>.

=item $table_item_tree = $converter->table_item_content_tree($element, $contents)
X<C<table_item_content_tree>>

I<$element> should be an C<@item> or C<@itemx> tree element,
I<$contents> should be corresponding texinfo tree contents.
Returns a tree in which the @-command in argument of C<@*table>
of the I<$element> has been applied to I<$contents>.

=item $result = $converter->top_node_filename($document_name)
X<C<top_node_filename>>

Returns a file name for the Top node file using either C<TOP_FILE>
customization value, or C<EXTENSION> customization value and I<$document_name>.

=back

Finally, there is:

=over

=item $result = $converter->output_internal_links()
X<C<output_internal_links>>

At this level, the method just returns undef.  It is used in the HTML
output, following the C<--internal-links> option of C<texi2any>
specification.

=back

=head1 SEE ALSO

L<Texinfo::Common>, L<Texinfo::Convert::Unicode>, L<Texinfo::Report>,
L<Texinfo::Translations>, L<Texinfo::Convert::Utils> and L<Texinfo::Parser>.

=head1 AUTHOR

Patrice Dumas, E<lt>pertusus@free.frE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright 2011- Free Software Foundation, Inc.  See the source file for
all copyright years.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

=cut
