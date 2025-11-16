# TexinfoMarkup.pm: output tree keeping Texinfo code information
#
# Copyright 2011, 2012, 2013, 2016, 2017, 2018 Free Software Foundation, Inc.
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
#
# This is an abstract class, that calls markup language formatting
# functions, but does not implement them.  The type of markup corresponds
# to nested elements with attributes, similar to XML.  Initially setup
# for XML, but more generic.
#
# Markup language formatting functions should be implemented in modules that
# inherit from this module.  The functions to implement are documented in
# the POD section.

package Texinfo::Convert::TexinfoMarkup;

use 5.00405;
use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Texinfo::Commands;
use Texinfo::Common;
use Texinfo::Convert::Converter;
use Texinfo::Convert::Unicode;
# for debugging and adding the original line for some commands
use Texinfo::Convert::Texinfo;
use Data::Dumper;
use Carp qw(cluck);

use vars qw($VERSION @ISA);
@ISA = qw(Texinfo::Convert::Converter);

$VERSION = '7.0.3';


# our because it is used in the xml to texi translator and subclasses.
our %no_arg_commands_formatting = (
           '*' => 'linebreak',
           ' ' => ['spacecmd', ['type', 'spc']],
           "\t" => ['spacecmd', ['type', 'tab']],
           "\n" => ['spacecmd', ['type', 'nl']],
           '-' => 'hyphenbreak',  # hyphenation hint
           '|' => ['divideheading'],  # used in formatting commands @evenfooting and friends
           '/' => 'slashbreak',
           ':' => 'noeos',
           '!' => 'eosexcl',
           '?' => 'eosquest',
           '.' => 'eosperiod',
           '@' => 'arobase',
           '{' => 'lbrace',
           '}' => 'rbrace',
           '&' => 'amp',
           '\\' => 'backslash',  # should only appear in math

           'TeX' => 'tex',
           'LaTeX' => 'latex',
           'bullet' => 'bullet',
           'copyright'    => 'copyright',
           'registeredsymbol'   => 'registered',
           'dots'    => 'dots',
           'enddots'    => 'enddots',
           'error'        => 'errorglyph',
           'expansion'     => 'expansion',
           'arrow'        => 'rarr',
           'click'        => ['click', ['command', 'arrow']],
           'minus'        => 'minus',
           'point'        => 'point',
           'print'        => 'printglyph',
           'result'       => 'result',
           'l'            => 'lslash',
           'L'            => 'Lslash',
           'today'        => ['today'],
           'comma'        => 'comma',
           'atchar'       => 'atchar',
           'ampchar'      => 'ampchar',
           'lbracechar'   => 'lbracechar',
           'rbracechar'   => 'rbracechar',
           'backslashchar' => 'backslashchar',
           'hashchar'      => 'hashchar',
           # in Texinfo::Convert::Converter::default_xml_no_arg_commands_formatting,
           # guillemotleft and guillemotright are mapped to laquo and raquo,
           # but guillemetleft and guillemetright are also mapped to those
           # entities.  To make sure that it is possible to go back to the
           # @-command from the entity, we use specific entities here
           'guillemotleft'   => 'guillemotleft',
           'guillemotright'  => 'guillemotright',
);

my %brace_commands = %Texinfo::Commands::brace_commands;

# use default XML formatting to complete the hash, removing XML
# specific formatting.  This avoids some code duplication.
my %default_xml_no_arg_commands_formatting =
    %Texinfo::Convert::Converter::xml_text_entity_no_arg_commands_formatting;

foreach my $command (keys(%default_xml_no_arg_commands_formatting)) {
  if (!exists($no_arg_commands_formatting{$command})) {
    if ($default_xml_no_arg_commands_formatting{$command} ne '') {
      if ($default_xml_no_arg_commands_formatting{$command} =~ /^&(.*);$/) {
        $no_arg_commands_formatting{$command} = $1;
      } else {
        die "BUG: Strange xml_no_arg_commands_formatting: $default_xml_no_arg_commands_formatting{$command}\n";
      }
    } else {
      $no_arg_commands_formatting{$command} = '';
    }
  }
}

# following is not format specific.  Some infos are taken from generic
# XML, but XML specific formatting is stripped.

my %accents = (
 '=' => 'macr',
# following are not entities
 'H' => 'doubleacute',
 'u' => 'breve',
 'v' => 'caron',
);

# our because it is used in the xml to texi translator
our %accent_types = (%Texinfo::Convert::Converter::xml_accent_entities, %accents);

# do not treat @dotless as an accent command but
# together with brace commands.
delete $accent_types{'dotless'};

# no entity
my @other_accents = ('dotaccent', 'tieaccent', 'ubaraccent', 'udotaccent');
foreach my $accent (@other_accents) {
  $accent_types{$accent} = $accent;
}

my %line_command_line_attributes = (
  'setfilename' => 'file',
  'documentencoding' => 'encoding',
  'verbatiminclude' => 'file',
  'documentlanguage' => 'lang',
);

my %line_command_numbered_arguments_attributes = (
  'definfoenclose' => [ 'command', 'open', 'close' ],
  'alias' => [ 'new', 'existing' ],
  'syncodeindex' => [ 'from', 'to' ],
  'synindex' => [ 'from', 'to' ],
);

my %nobrace_commands = %Texinfo::Commands::nobrace_commands;
my %line_commands = %Texinfo::Commands::line_commands;

foreach my $command ('item', 'headitem', 'tab') {
  delete $nobrace_commands{$command};
}

foreach my $command ('item', 'itemx',
                      keys(%Texinfo::Commands::def_commands)) {
  delete $line_commands{$command};
}

my %default_args_code_style
  = %Texinfo::Convert::Converter::default_args_code_style;

# our because it is used in the xml to texi translator
our %commands_args_elements = (
  'email' => ['emailaddress', 'emailname'],
  'uref' => ['urefurl', 'urefdesc', 'urefreplacement'],
  'url' => ['urefurl', 'urefdesc', 'urefreplacement'],
  'inforef' => ['inforefnodename', 'inforefrefname', 'inforefinfoname'],
  'image' => ['imagefile', 'imagewidth', 'imageheight',
              'alttext', 'imageextension'],
  # * means that the previous element is variadic, ie can appear indefinitely
  'example' => ['examplelanguage', 'examplearg', '*'],
  'quotation' => ['quotationtype'],
  'float' => ['floattype', 'floatname'],
  'itemize' => ['itemprepend'],
  'enumerate' => ['enumeratefirst'],
);

foreach my $ref_cmd ('pxref', 'xref', 'ref') {
  $commands_args_elements{$ref_cmd}
    = ['xrefnodename', 'xrefinfoname', 'xrefprinteddesc', 'xrefinfofile',
       'xrefprintedname'];
}

foreach my $explained_command (keys(%Texinfo::Common::explained_commands)) {
  $commands_args_elements{$explained_command} = ["${explained_command}word",
                                                 "${explained_command}desc"];
}

foreach my $brace_command (keys(%Texinfo::Commands::brace_commands)) {
  if ($Texinfo::Commands::brace_commands{$brace_command} eq 'inline') {
    if ($brace_command eq 'inlinefmtifelse') {
      $commands_args_elements{$brace_command} = ["${brace_command}format",
             "${brace_command}contentif", "${brace_command}contentelse"];

    } else {
      $commands_args_elements{$brace_command} = ["${brace_command}format",
                                                 "${brace_command}content"];
    }
  }
}

my %defcommand_name_type = (
 'deffn'     => 'function',
 'defvr'     => 'variable',
 'deftypefn' => 'function',
 'deftypeop' => 'operation',
 'deftypevr' => 'variable',
 'defcv'     => 'classvar',
 'deftypecv' => 'classvar',
 'defop'     => 'operation',
 'deftp'     => 'datatype',
);

my %ignored_types;
foreach my $type (
            # @-commands replaced in the tree
            'replaced',
  ) {
  $ignored_types{$type} = 1;
}

my %type_elements = (
  'paragraph' => 'para',
  'preformatted' => 'pre',
  'menu_entry' => 'menuentry',
  'menu_entry_node' => 'menunode',
  'menu_comment' => 'menucomment',
  'menu_entry_description' => 'menudescription',
  'menu_entry_name' => 'menutitle',
  'preamble_before_beginning' => 'preamblebeforebeginning',
  'table_item' => 'tableitem',
  'table_entry' => 'tableentry',
  'table_term' => 'tableterm',
  'row' => 'row',
  'multitable_head' => 'thead',
  'multitable_body' => 'tbody',
  'def_item' => 'definitionitem',
  'before_item' => 'beforefirstitem',
);

my %default_context_block_commands = (
  'float' => 1,
);

# converter_defaults() should be implemented by subclasses.
sub converter_defaults($$)
{
}

sub converter_initialize($)
{
  my $self = shift;

  $self->{'document_context'} = [{'monospace' => [0]}];
  $self->{'context_block_commands'} = {%default_context_block_commands};
  foreach my $raw (grep {$Texinfo::Commands::block_commands{$_} eq 'format_raw'}
                        keys(%Texinfo::Commands::block_commands)) {
    $self->{'context_block_commands'}->{$raw} = 1
         if $self->{'expanded_formats_hash'}->{$raw};
  }
}

# Main output function for the Texinfo language markup output files.
sub output($$)
{
  my $self = shift;
  my $root = shift;

  my ($output_file, $destination_directory, $output_filename)
       = $self->determine_files_and_directory();

  my ($encoded_destination_directory, $dir_encoding)
    = $self->encoded_output_file_name($destination_directory);
  my $succeeded
    = $self->create_destination_directory($encoded_destination_directory,
                                          $destination_directory);
  return undef unless $succeeded;


  my $fh;
  my $encoded_output_file;
  if (! $output_file eq '') {
    my $path_encoding;
    ($encoded_output_file, $path_encoding)
      = $self->encoded_output_file_name($output_file);
    my $error_message;
    ($fh, $error_message) = Texinfo::Common::output_files_open_out(
                             $self->output_files_information(), $self,
                             $encoded_output_file);
    if (!$fh) {
      $self->document_error($self,
                 sprintf(__("could not open %s for writing: %s"),
                                    $output_file, $error_message));
      return undef;
    }
  }

  my $result = '';
  $result .= $self->write_or_return($self->txi_markup_header(), $fh);
  $result
    .= $self->write_or_return($self->txi_markup_open_element('texinfo')."\n", $fh);
  if ($output_file ne '') {
    my $filename_element = $self->txi_markup_open_element('filename',
                                                  [['file', $output_filename]])
             .$self->txi_markup_close_element('filename')."\n";
    $result .= $self->write_or_return($filename_element, $fh);
  }
  if ($self->get_conf('USE_NODES')) {
    $result .= $self->convert_document_nodes($root, $fh);
  } else {
    $result .= $self->convert_document_sections($root, $fh);
  }
  $result .= $self->write_or_return($self->txi_markup_close_element('texinfo')."\n", $fh);
  # FIXME add txi_markup_footer() to format a footer for the file?
  if ($fh and $output_file ne '-') {
    Texinfo::Common::output_files_register_closed(
                  $self->output_files_information(), $encoded_output_file);
    if (!close ($fh)) {
      $self->document_error($self,
                  sprintf(__("error on closing %s: %s"),
                                    $output_file, $!));
    }
  }

  return $result;
}

# API for markup formatting subclasses
sub in_monospace($)
{
  my $self = shift;
  return $self->{'document_context'}->[-1]->{'monospace'}->[-1];
}
# end of API

sub _format_command($$)
{
  my $self = shift;
  my $command = shift;

  if (ref($no_arg_commands_formatting{$command}) eq '') {
    return $self->txi_markup_atom($command);
  } else {
    my @spec = @{$no_arg_commands_formatting{$command}};
    my $format_element = shift @spec;
    return $self->txi_markup_element($format_element, \@spec);
  }
}

sub _index_entry($$)
{
  my $self = shift;
  my $element = shift;
  if ($element->{'extra'} and $element->{'extra'}->{'index_entry'}) {
    my $index_entry = $element->{'extra'}->{'index_entry'};
    my $attribute = [['index', $index_entry->{'index_name'}]];
    push @$attribute, ['number', $index_entry->{'entry_number'}]
        if (defined($index_entry->{'entry_number'}));
    # in case the index is not a default index, or the style of the
    # entry (in code or not) is not the default for this index
    if ($self->{'indices_information'}) {
      my $in_code
         = $self->{'indices_information'}->{$index_entry->{'index_name'}}->{'in_code'};
      if (!$Texinfo::Commands::index_names{$index_entry->{'index_name'}}
          or $in_code != $Texinfo::Commands::index_names{$index_entry->{'index_name'}}->{'in_code'}) {
        push @$attribute, ['incode', $in_code];
      }
      if ($self->{'indices_information'}->{$index_entry->{'index_name'}}->{'merged_in'}) {
        push @$attribute, ['mergedindex',
         $self->{'indices_information'}->{$index_entry->{'index_name'}}->{'merged_in'}];
      }
    }
    my $result = $self->txi_markup_open_element('indexterm', $attribute);
    push @{$self->{'document_context'}}, {'monospace' => [0]};
    $self->{'document_context'}->[-1]->{'monospace'}->[-1] = 1
      if ($index_entry->{'in_code'});
    $result .= $self->_convert({'contents' => $index_entry->{'entry_content'}});
    pop @{$self->{'document_context'}};
    $result .= $self->txi_markup_close_element('indexterm');
    return $result;
  }
  return '';
}

sub _infoenclose_attribute($$) {
  my $self = shift;
  my $element = shift;
  my @attribute = ();
  return @attribute if (!$element->{'extra'});
  push @attribute, ['begin', $element->{'extra'}->{'begin'}]
    if (defined($element->{'extra'}->{'begin'}));
  push @attribute, ['end', $element->{'extra'}->{'end'}]
    if (defined($element->{'extra'}->{'end'}));
  return @attribute;
}

sub _accent($$;$$$)
{
  my $self = shift;
  my $text = shift;
  my $element = shift;
  my $in_upper_case = shift;
  my $attributes = shift;
  $attributes = [] if (!defined($attributes));

  unshift @$attributes, ['type', $accent_types{$element->{'cmdname'}}];
  my $result = $self->txi_markup_open_element('accent', $attributes);
  $result .= $text;
  $result .= $self->txi_markup_close_element('accent');
  return $result;
}

sub convert($$)
{
  my $self = shift;
  my $root = shift;
  
  return $self->convert_document_sections($root);
}

sub convert_tree($$)
{
  my $self = shift;
  my $root = shift;

  return $self->_convert($root);
}

# FIXME is that function markup format specific or not?
sub _protect_in_spaces_attribute_text($)
{
  my $text = shift;
  $text =~ s/\n/\\n/g;
  $text =~ s/\f/\\f/g;
  return $text;
}

sub _leading_spaces_arg($)
{
  my $element = shift;
  if ($element->{'extra'} and $element->{'extra'}->{'spaces_before_argument'}
      and $element->{'extra'}->{'spaces_before_argument'} ne '') {
    return ['spaces', _protect_in_spaces_attribute_text(
                            $element->{'extra'}->{'spaces_before_argument'})];
  } else {
    return ();
  }
}

# return spaces only, end of line is gathered by calling
# format_comment_or_return_end_line
sub _end_line_spaces
{
  my $self = shift;
  my $element = shift;

  my $end_spaces = '';
  if ($element->{'args'}->[-1]
      and $element->{'args'}->[-1]->{'extra'}
      and $element->{'args'}->[-1]->{'extra'}->{'spaces_after_argument'}) {
    # spaces and form feeds only, protection is needed for form feeds
    my $spaces = $element->{'args'}->[-1]->{'extra'}->{'spaces_after_argument'};
    chomp $spaces;
    $end_spaces = $self->txi_markup_protect_text($spaces);
  }
  return $end_spaces;
}

sub _arg_line($)
{
  my $self = shift;
  my $element = shift;
  if ($element->{'extra'} and defined($element->{'extra'}->{'arg_line'})) {
    my $line = $element->{'extra'}->{'arg_line'};
    chomp($line);
    if ($line ne '') {
      return ['line', $line];
    }
  }
  return ();
}

# without the end of line.  The end of line is usually returned by
# format_comment_or_return_end_line
sub _trailing_spaces_arg($)
{
  my $element = shift;
  
  if ($element->{'extra'} and
      $element->{'extra'}->{'spaces_after_argument'}) {
    my $spaces = $element->{'extra'}->{'spaces_after_argument'};
    chomp($spaces);
    if ($spaces ne '') {
      return ['trailingspaces', _protect_in_spaces_attribute_text($spaces)];
    }
  }
  return ();
}

sub _leading_trailing_spaces_arg($)
{
  my $element = shift;

  my @result;
  push @result, _leading_spaces_arg($element);
  push @result, _trailing_spaces_arg($element);
  return @result;
}

sub _texinfo_line($$)
{
  my $self = shift;
  my $element = shift;

  if ($element->{'args'}) {
    my $line = Texinfo::Convert::Texinfo::convert_to_texinfo($element->{'args'}->[-1]);
    chomp($line);
    if ($line ne '') {
      return ['line', $line];
    }
  }
  return ();
}

sub _convert_argument_and_end_line($$)
{
  my $self = shift;
  my $element = shift;

  my $converted = $self->convert_tree($element->{'args'}->[-1]);
  my $end_space = _end_line_spaces($self, $element);
  my $end_line = $self->format_comment_or_return_end_line($element);
  return ($converted, $end_space, $end_line);
}

my @node_directions = ('Next', 'Prev', 'Up');

# not used here, but it is consistent with other %commands_args_elements
# entries and may be used by XML to Texinfo converters
$commands_args_elements{'node'} = ['nodename'];
foreach my $direction (@node_directions) {
  push @{$commands_args_elements{'node'}}, 'node'.lc($direction);
}

sub _convert($$;$);

sub _convert($$;$)
{
  my $self = shift;
  my $element = shift;

  if (0) {
  #if (1) { #}
    print STDERR "root\n";
    print STDERR "  Command: $element->{'cmdname'}\n" if ($element->{'cmdname'});
    print STDERR "  Type: $element->{'type'}\n" if ($element->{'type'});
    print STDERR "  Text: $element->{'text'}\n" if (defined($element->{'text'}));
    #print STDERR "  Special def_command: $element->{'extra'}->{'def_command'}\n"
    #  if (defined($element->{'extra'}) and $element->{'extra'}->{'def_command'});
  }

  return '' if ($element->{'type'} and $ignored_types{$element->{'type'}});
  if (defined($element->{'text'})) {
    if ($self->{'document_context'}->[-1]->{'raw'}) {
      return $element->{'text'};
    }
    return $self->txi_markup_convert_text($element);
  }

  my @close_format_elements;
  # for @-commands with contents, to accumulate conversion results
  my $result = '';
  if ($element->{'cmdname'}) {
    if (defined($no_arg_commands_formatting{$element->{'cmdname'}})) {
      if ($element->{'cmdname'} eq 'click'
          and $element->{'extra'}
          and defined($element->{'extra'}->{'clickstyle'})) {
        return $self->txi_markup_element('click',
                        [['command', $element->{'extra'}->{'clickstyle'}]]);
      }
      if ($self->{'itemize_line'} and $element->{'type'}
          and ($element->{'type'} eq 'command_as_argument'
               or $element->{'type'} eq 'command_as_argument_inserted')
          and !$element->{'args'}) {
        my $arguments = [['command', $element->{'cmdname'}]];
        push @$arguments, ['automatic', 'on']
          if ($element->{'type'} eq 'command_as_argument_inserted');
        return $self->txi_markup_element('formattingcommand', $arguments);
      }
      return $self->_format_command($element->{'cmdname'});
    } elsif ($accent_types{$element->{'cmdname'}}) {
      if ($self->get_conf('ENABLE_ENCODING')) {
        return $self->convert_accents($element, \&_accent);
      } else {
        my $attributes = [];
        my $arg;
        if (!$element->{'args'}) {
          $arg = '';
        } else {
          $arg = $self->_convert($element->{'args'}->[0]);
          if ($element->{'extra'} and $element->{'extra'}->{'spaces'}) {
            push @$attributes, ['spaces', $element->{'extra'}->{'spaces'}];
          }
          if ($element->{'args'}->[0]->{'type'} eq 'following_arg') {
             push @$attributes, ['bracketed', 'off'];
          }
        }
        return $self->_accent($arg, $element, undef, $attributes);
      }
    } elsif ($element->{'cmdname'} eq 'item' or $element->{'cmdname'} eq 'itemx'
             or $element->{'cmdname'} eq 'headitem'
             or $element->{'cmdname'} eq 'tab') {
      if ($element->{'cmdname'} eq 'item'
          and $element->{'parent'}->{'cmdname'}
          and ($element->{'parent'}->{'cmdname'} eq 'itemize'
               or $element->{'parent'}->{'cmdname'} eq 'enumerate')) {
        $result .= $self->txi_markup_open_element('listitem',
                                [_leading_spaces_arg($element)]);
        if ($element->{'parent'}->{'cmdname'} eq 'itemize'
            and $element->{'parent'}->{'args'}
            and @{$element->{'parent'}->{'args'}}) {
          $result .= $self->txi_markup_open_element('prepend')
            .$self->_convert($element->{'parent'}->{'args'}->[0])
            .$self->txi_markup_close_element('prepend');
        }
        unshift @close_format_elements, 'listitem';
      } elsif (($element->{'cmdname'} eq 'item' or $element->{'cmdname'} eq 'itemx')
               and $element->{'parent'}->{'type'}
               and $element->{'parent'}->{'type'} eq 'table_term') {
        my $table_command = $element->{'parent'}->{'parent'}->{'parent'};
        my $format_item_command;
        my $attribute = [];
        if ($table_command->{'extra'}
            and $table_command->{'extra'}->{'command_as_argument'}) {
          $format_item_command
            = $table_command->{'extra'}->{'command_as_argument'}->{'cmdname'};
          $attribute
           = [$self->_infoenclose_attribute($table_command->{'extra'}->{'command_as_argument'})];
        }
        my $line_item_result = $self->txi_markup_open_element($element->{'cmdname'},
                                                     [_leading_spaces_arg($element)]);
        if ($format_item_command) {
          $line_item_result .= $self->txi_markup_open_element('itemformat',
                                         [['command', $format_item_command], @$attribute]);
        }
        $line_item_result .= $self->_index_entry($element);
        my $in_code;
        $in_code = 1
          if ($format_item_command
              and defined($default_args_code_style{$format_item_command})
              and $default_args_code_style{$format_item_command}->[0]);
        my $in_monospace_not_normal;
        if ($format_item_command) {
          if (defined($default_args_code_style{$format_item_command})
              and $default_args_code_style{$format_item_command}->[0]) {
            $in_monospace_not_normal = 1;
          } elsif ($brace_commands{$format_item_command}
                   and $brace_commands{$format_item_command} eq 'style_no_code') {
            $in_monospace_not_normal = 0;
          }
        }
        push @{$self->{'document_context'}->[-1]->{'monospace'}},
          $in_monospace_not_normal
            if (defined($in_monospace_not_normal));

        my ($arg, $end_space, $end_line)
             = $self->_convert_argument_and_end_line($element);

        pop @{$self->{'document_context'}->[-1]->{'monospace'}}
          if (defined($in_monospace_not_normal));

        $line_item_result .= $arg . $end_space;
        if ($format_item_command) {
          $line_item_result .= $self->txi_markup_close_element('itemformat');
        }
        $line_item_result
           .= $self->txi_markup_close_element($element->{'cmdname'}).$end_line;
        return $line_item_result;
      } else {
        unless (($element->{'cmdname'} eq 'item'
                     or $element->{'cmdname'} eq 'headitem'
                     or $element->{'cmdname'} eq 'tab')
                    and $element->{'parent'}->{'type'}
                    and $element->{'parent'}->{'type'} eq 'row') {
          print STDERR "BUG: multitable cell command not in a row "
            .Texinfo::Common::debug_print_element($element);
        }
        
        $result .= $self->txi_markup_open_element('entry',
               [['command', $element->{'cmdname'}], _leading_spaces_arg($element)]);
        unshift @close_format_elements, 'entry';
      }
    } elsif ($element->{'type'} and $element->{'type'} eq 'index_entry_command') {
      my $format_element;
      my $attribute = [];
      if (exists $line_commands{$element->{'cmdname'}}) {
        $format_element = $element->{'cmdname'};
      } else {
        $format_element = 'indexcommand';
        $attribute = [['command', $element->{'cmdname'}]];
      }
      push @$attribute, ['index', $element->{'extra'}->{'index_entry'}->{'index_name'}];
      push @$attribute, _leading_spaces_arg($element);

      # this is important to get the spaces before a @subentry
      my $end_line_spaces = _end_line_spaces($self, $element);
      my $end_line = $self->format_comment_or_return_end_line($element);

      return $self->txi_markup_open_element($format_element, $attribute)
          .$self->_index_entry($element)
          .$self->txi_markup_close_element($format_element).$end_line_spaces
          .${end_line};
    } elsif (exists($line_commands{$element->{'cmdname'}})) {
      my $cmdname = $element->{'cmdname'};
      my $type = $line_commands{$cmdname};
      if ($type eq 'text') {
        return '' if ($cmdname eq 'end');
        my $attribute;
        if ($line_command_line_attributes{$cmdname}) {
          if ($element->{'extra'} and defined($element->{'extra'}->{'text_arg'})) {
            push @$attribute, [$line_command_line_attributes{$cmdname},
                  $element->{'extra'}->{'text_arg'}];
          }
        }
        my ($arg, $end_space, $end_line)
              = $self->_convert_argument_and_end_line($element);
        push @$attribute, _leading_spaces_arg($element);
        return $self->txi_markup_open_element($cmdname, $attribute).$arg.$end_space
                .$self->txi_markup_close_element($cmdname).${end_line};
      } elsif ($type eq 'line') {
        if ($cmdname eq 'node') {
          my $nodename;
          if (defined($element->{'extra'}->{'normalized'})) {
            $nodename = $element->{'extra'}->{'normalized'};
          } else {
            $nodename = '';
          }
          $result .= $self->txi_markup_open_element('node', [['name', $nodename],
                                         _leading_spaces_arg($element)]);
          push @{$self->{'document_context'}->[-1]->{'monospace'}}, 1;
          $result .= $self->txi_markup_open_element('nodename',
                                    [_trailing_spaces_arg($element->{'args'}->[0])])
             .$self->_convert({'contents' => $element->{'extra'}->{'node_content'}})
             .$self->txi_markup_close_element('nodename');
          # first arg is the node name, directions start at 1.
          my $direction_index = 1;
          my $pending_empty_directions = '';
          foreach my $direction(@node_directions) {
            my $format_element = 'node'.lc($direction);
            if ($element->{'structure'}->{'node_'.lc($direction)}) {
              my $node_direction = $element->{'structure'}->{'node_'.lc($direction)};
              my $node_name = '';
              my $attributes = [];
              if (! defined($element->{'extra'}->{'nodes_manuals'}->[$direction_index])) {
                push @$attributes, ['automatic', 'on'];
              }
              if ($element->{'args'}->[$direction_index]) {
                push @$attributes, _leading_trailing_spaces_arg(
                                 $element->{'args'}->[$direction_index]);
              }
              if ($node_direction->{'extra'}->{'manual_content'}) {
                $node_name .= $self->_convert({
                             'contents' => [{'text' => '('},
                             @{$node_direction->{'extra'}->{'manual_content'}},
                                          {'text' => ')'}]});
              }
              if ($node_direction->{'extra'}->{'node_content'}) {
                $node_name .= Texinfo::Common::normalize_top_node_name($self->_convert({
                  'contents' => $node_direction->{'extra'}->{'node_content'}}));
              }
              $result .= "$pending_empty_directions".
                $self->txi_markup_open_element($format_element, $attributes).$node_name.
                $self->txi_markup_close_element($format_element);
              $pending_empty_directions = '';
            } else {
              if ($element->{'args'}->[$direction_index]) {
                $pending_empty_directions .=
                  $self->txi_markup_open_element($format_element,
                    [_leading_trailing_spaces_arg(
                                 $element->{'args'}->[$direction_index])])
                            .$self->txi_markup_close_element($format_element);
              }
            }
            $direction_index++;
          }
          if (! $self->get_conf('USE_NODES')) {
            $result .= $self->txi_markup_close_element('node');
          }
          $result .= $self->format_comment_or_return_end_line($element);
          pop @{$self->{'document_context'}->[-1]->{'monospace'}};
        } elsif ($Texinfo::Commands::root_commands{$cmdname}) {
          my $attribute = [_leading_spaces_arg($element)];
          my $level_adjusted_cmdname
            = Texinfo::Structuring::section_level_adjusted_command_name($element);
          if ($level_adjusted_cmdname ne $cmdname) {
            unshift @$attribute, ['originalcommand', $cmdname];
          }
          $result .= $self->txi_markup_open_element($level_adjusted_cmdname,
                                                    $attribute);
          my $closed_section_element;
          if ($self->get_conf('USE_NODES')) {
            $closed_section_element
               = $self->txi_markup_close_element($level_adjusted_cmdname);
          } else {
            $closed_section_element = '';
          }

          if ($element->{'args'} and $element->{'args'}->[0]) {
            my ($arg, $end_space, $end_line)
               = $self->_convert_argument_and_end_line($element);
            $result .= $self->txi_markup_open_element('sectiontitle').$arg.$end_space
                      .$self->txi_markup_close_element('sectiontitle')
                      .$closed_section_element.$end_line;
          } else {
            $result .= $closed_section_element;
          }
        } else {
          my $attribute = [_leading_spaces_arg($element)];
          if ($cmdname eq 'listoffloats' and $element->{'extra'}
              and $element->{'extra'}->{'type'}
              and defined($element->{'extra'}->{'type'}->{'normalized'})) {
            unshift @$attribute, ['type', $element->{'extra'}->{'type'}->{'normalized'}];
          }
          my ($arg, $end_space, $end_line)
                = $self->_convert_argument_and_end_line($element);
          return $self->txi_markup_open_element($cmdname, $attribute).$arg.$end_space
               .$self->txi_markup_close_element($cmdname).$end_line;
        }
      } elsif ($type eq 'skipline') {
        # the command associated with an element is closed at the end of the
        # element. @bye is withing the element, but we want it to appear after
        # the command closing.  So we delay the output of @bye, and store it.
        if ($cmdname eq 'bye' and $element->{'structure'}
            and $element->{'structure'}->{'associated_unit'}
            and $element->{'structure'}->{'associated_unit'}->{'extra'}
            and defined($element->{'structure'}->{'associated_unit'}->{'extra'}->{'unit_command'})) {
          $self->{'pending_bye'} = $self->txi_markup_open_element($cmdname)
                    .$self->txi_markup_close_element($cmdname)."\n";
          return '';
        }
        my $attribute = [];
        if ($element->{'args'} and $element->{'args'}->[0]
            and defined($element->{'args'}->[0]->{'text'})) {
          my $line = $element->{'args'}->[0]->{'text'};
          chomp($line);
          $attribute = [['line', $line]]
             if ($line ne '');
        }
        return $self->txi_markup_open_element($cmdname, $attribute)
                 .$self->txi_markup_close_element($cmdname)."\n";
      } elsif ($type eq 'special') {
        if ($cmdname eq 'clear' or $cmdname eq 'set') {
          my $attribute = [];
          if ($element->{'args'} and $element->{'args'}->[0]
              and defined($element->{'args'}->[0]->{'text'})) {
            push @$attribute, ['name', $element->{'args'}->[0]->{'text'}];
          }
          my $value = '';
          if ($cmdname eq 'set' and $element->{'args'} and $element->{'args'}->[1]
              and defined($element->{'args'}->[1]->{'text'})) {
            $value
              = $self->txi_markup_protect_text($element->{'args'}->[1]->{'text'});
          }
          push @$attribute, $self->_arg_line($element);
          return $self->txi_markup_open_element($cmdname, $attribute)
                      .$value.$self->txi_markup_close_element($cmdname)."\n";
        } elsif ($cmdname eq 'clickstyle') {
          my $attribute = [$self->_arg_line($element)];
          my $value = '';
          if ($element->{'args'} and $element->{'args'}->[0]
              and defined($element->{'args'}->[0]->{'text'})) {
            my $click_command = $element->{'args'}->[0]->{'text'};
            $click_command =~ s/^\@//;
            unshift @$attribute, ['command', $click_command];
            $value
              = $self->txi_markup_protect_text($element->{'args'}->[0]->{'text'});
          };
          return $self->txi_markup_open_element($cmdname, $attribute)
                         .$value.$self->txi_markup_close_element($cmdname)."\n";
        } else {
          # should only be unmacro
          my $attribute = [$self->_arg_line($element)];
          if ($element->{'args'} and $element->{'args'}->[0]
              and defined($element->{'args'}->[0]->{'text'})) {
            unshift @$attribute, ['name', $element->{'args'}->[0]->{'text'}];
          }
          return $self->txi_markup_open_element($cmdname, $attribute)
                    .$self->txi_markup_close_element($cmdname)."\n";
        }
      } elsif ($type eq 'lineraw') {
        if ($cmdname eq 'c' or $cmdname eq 'comment') {
          return $self->txi_markup_comment(
                         " $cmdname".$element->{'args'}->[0]->{'text'})
        } else {
          my $value = '';
          if ($element->{'args'} and $element->{'args'}->[0]
              and defined($element->{'args'}->[0]->{'text'})) {
            $value
             = $self->txi_markup_protect_text($element->{'args'}->[0]->{'text'});
          }
          chomp ($value);
          return $self->txi_markup_open_element($cmdname).$value
                    .$self->txi_markup_close_element($cmdname)."\n";
        }
      } else {
        print STDERR "BUG: unknown line_command style $type\n"
           if ($type ne 'specific');
        my $args_attributes;
        if ($line_command_numbered_arguments_attributes{$cmdname}) {
          $args_attributes = $line_command_numbered_arguments_attributes{$cmdname};
        } else {
          $args_attributes = ['value'];
        }
        my $attribute = [_leading_spaces_arg($element)];
        my $arg_index = 0;
        if (defined($element->{'extra'})
            and defined($element->{'extra'}->{'misc_args'})) {
          foreach my $arg_attribute (@{$args_attributes}) {
            if (defined ($element->{'extra'}->{'misc_args'}->[$arg_index])) {
              push @$attribute, [$arg_attribute,
                        $element->{'extra'}->{'misc_args'}->[$arg_index]];
            }
            $arg_index++;
          }
        }
        my $end_line = $self->format_comment_or_return_end_line($element);
        # not sure if it may happen
        $end_line = "\n" if ($end_line eq '');
        push @$attribute, $self->_texinfo_line($element);
        return $self->txi_markup_open_element($cmdname, $attribute)
                    .$self->txi_markup_close_element($cmdname).$end_line;
      }
    } elsif (exists($nobrace_commands{$element->{'cmdname'}})) {
      # other or skipspace, symbol commands should be selected above
      # as being in no_arg_commands_formatting
      my $cmdname = $element->{'cmdname'};
      my $spaces = '';
      $spaces = $element->{'extra'}->{'spaces_after_command'}
        if ($element->{'extra'} and $element->{'extra'}->{'spaces_after_command'}
            and $element->{'extra'}->{'spaces_after_command'} ne '');
      return $self->txi_markup_open_element($cmdname)
              .$self->txi_markup_close_element($cmdname).$spaces;
    } elsif ($element->{'type'}
             and $element->{'type'} eq 'definfoenclose_command') {
      my $in_monospace_not_normal;
      if (defined($default_args_code_style{$element->{'cmdname'}})
          and $default_args_code_style{$element->{'cmdname'}}->[0]) {
        $in_monospace_not_normal = 1;
      } elsif ($brace_commands{$element->{'cmdname'}}
               and $brace_commands{$element->{'cmdname'}} eq 'style_no_code') {
        $in_monospace_not_normal = 0;
      }
      push @{$self->{'document_context'}->[-1]->{'monospace'}},
        $in_monospace_not_normal
          if (defined($in_monospace_not_normal));
      my $arg = $self->_convert($element->{'args'}->[0]);
      pop @{$self->{'document_context'}->[-1]->{'monospace'}}
        if (defined($in_monospace_not_normal));
      my $command_result = $self->txi_markup_open_element('infoenclose',
                                          [['command', $element->{'cmdname'}],
                                       $self->_infoenclose_attribute($element)])
                 .$arg.$self->txi_markup_close_element('infoenclose');
      return $command_result;
    } elsif ($element->{'args'}
             and exists($brace_commands{$element->{'cmdname'}})) {

      if ($Texinfo::Common::inline_format_commands{$element->{'cmdname'}}
          and $element->{'extra'} and $element->{'extra'}->{'format'}
          and $self->{'expanded_formats_hash'}->{$element->{'extra'}->{'format'}}) {
        if ($element->{'cmdname'} eq 'inlineraw') {
          push @{$self->{'document_context'}}, {'monospace' => [0]};
          $self->{'document_context'}->[-1]->{'raw'} = 1;
        }
        my $command_result = '';
        if (scalar (@{$element->{'args'}}) == 2
              and defined($element->{'args'}->[-1])
              and $element->{'args'}->[-1]->{'contents'}
              and @{$element->{'args'}->[-1]->{'contents'}}) {
          $command_result = $self->_convert({'contents'
                        => $element->{'args'}->[-1]->{'contents'}});
        }
        if ($element->{'cmdname'} eq 'inlineraw') {
          pop @{$self->{'document_context'}};
        }
        return $command_result;
      }


      # this is used for commands without args, or associated to the
      # first argument
      my $attribute = [];
      if ($element->{'cmdname'} eq 'verb') {
        push @$attribute, ['delimiter', $element->{'extra'}->{'delimiter'}];
      } elsif ($element->{'cmdname'} eq 'anchor') {
        my $anchor_name;
        if (defined($element->{'extra'}->{'normalized'})) {
          $anchor_name = $element->{'extra'}->{'normalized'};
        } else {
          $anchor_name = '';
        }
        push @$attribute, ['name', $anchor_name];
      }

      my @format_elements;
      my $main_cmdname;
      if ($commands_args_elements{$element->{'cmdname'}}) {
        # command with arguments, format the arguments using their specific
        # elements
        @format_elements = @{$commands_args_elements{$element->{'cmdname'}}};
        $main_cmdname = $element->{'cmdname'};
      } else {
        # command with only one argument, the command itself is used as
        # element for the first argument
        @format_elements = ($element->{'cmdname'});
        # leading spaces are directly associated to the @-command for @-command
        # in context brace_commands
        push @$attribute, _leading_spaces_arg($element);
      }

      if ($Texinfo::Commands::brace_commands{$element->{'cmdname'}} eq 'context') {
        push @{$self->{'document_context'}}, {'monospace' => [0]};
      }
      my $last_empty_element;
      my $args_or_one_arg_cmd = '';
      my $arg_index = 0;
      foreach my $format_element (@format_elements) {
        if (defined($element->{'args'}->[$arg_index])) {
          # Leading spaces are gathered here except for context brace_commands
          # (gathered just above).
          push @$attribute,
            _leading_spaces_arg($element->{'args'}->[$arg_index]);
          my $in_monospace_not_normal;
          if (defined($default_args_code_style{$element->{'cmdname'}})
              and $default_args_code_style{$element->{'cmdname'}}->[$arg_index]) {
            $in_monospace_not_normal = 1;
          } elsif ($brace_commands{$element->{'cmdname'}}
                   and $brace_commands{$element->{'cmdname'}} eq 'style_no_code') {
            $in_monospace_not_normal = 0;
          }
          push @{$self->{'document_context'}->[-1]->{'monospace'}},
            $in_monospace_not_normal
              if (defined($in_monospace_not_normal));
          my $arg = $self->_convert($element->{'args'}->[$arg_index]);
          pop @{$self->{'document_context'}->[-1]->{'monospace'}}
            if (defined($in_monospace_not_normal));

          if ($element->{'args'}->[$arg_index]->{'extra'}
              and $element->{'args'}->[$arg_index]->{'extra'}->{'spaces_after_argument'}) {
            $arg .= $element->{'args'}->[$arg_index]
                   ->{'extra'}->{'spaces_after_argument'};
          }

          if (!defined($main_cmdname) or $arg ne '' or scalar(@$attribute) > 0) {
            $args_or_one_arg_cmd .=
                 $self->txi_markup_open_element($format_element, $attribute).$arg
                      .$self->txi_markup_close_element($format_element);
            $last_empty_element = undef;
          # we keep the last empty argument to be able to prepend it to be able
          # to reconstitute trailing empty arguments in the original Texinfo code.
          # For example, for @bracecmd{a,b,,c,,} we keep the last (6th argument)
          # empty element.
          # Not if in inline conditionals as we are not interested in empty ignored
          # inline conditional arguments.
          } elsif (defined($main_cmdname)
                   and not $brace_commands{$element->{'cmdname'}} eq 'inline') {
            $last_empty_element = $self->txi_markup_open_element($format_element)
                                  .$self->txi_markup_close_element($format_element);
          }
          $attribute = [];
        } else {
          last;
        }
        $arg_index++;
      }
      if ($last_empty_element) {
        $args_or_one_arg_cmd .= $last_empty_element;
      }
      if ($Texinfo::Commands::brace_commands{$element->{'cmdname'}} eq 'context') {
        pop @{$self->{'document_context'}};
      }
      if (not defined($main_cmdname)) {
        # one argument @-command
        return $args_or_one_arg_cmd;
      }
      # This is for the main command
      $attribute = [];
      if ($element->{'cmdname'} eq 'image') {
        if (Texinfo::Common::element_is_inline($element)) {
          push @$attribute, ['where', 'inline'];
        }
      } elsif ($Texinfo::Commands::ref_commands{$element->{'cmdname'}}) {
        if ($element->{'args'}) {
          my $normalized;
          if ($element->{'extra'}
              and $element->{'extra'}->{'node_argument'}
              and $element->{'extra'}->{'node_argument'}->{'node_content'}) {
            my $normalized;
            if (defined($element->{'extra'}->{'node_argument'}->{'normalized'})) {
              $normalized = $element->{'extra'}->{'node_argument'}->{'normalized'};
            } else {
              $normalized
               = Texinfo::Convert::NodeNameNormalization::normalize_node(
              {'contents' => $element->{'extra'}->{'node_argument'}->{'node_content'}});
            }
            if ($normalized) {
              push @$attribute, ['label', $normalized];
            }
          }
          my $manual;
          my $manual_arg_index = 3;
          if ($element->{'cmdname'} eq 'inforef') {
            $manual_arg_index = 2;
          }
          if (defined($element->{'args'}->[$manual_arg_index])
              and $element->{'args'}->[$manual_arg_index]->{'contents'}
              and @{$element->{'args'}->[$manual_arg_index]->{'contents'}}) {
            $manual = Texinfo::Convert::Text::convert_to_text({'contents'
                     => $element->{'args'}->[$manual_arg_index]->{'contents'}},
                 {'code' => 1,
                  Texinfo::Convert::Text::copy_options_for_convert_text($self, 1)});
          }
          if (!defined($manual) and $element->{'extra'}
              and $element->{'extra'}->{'node_argument'}
              and $element->{'extra'}->{'node_argument'}->{'manual_content'}) {
            $manual = Texinfo::Convert::Text::convert_to_text({'contents'
                   => $element->{'extra'}->{'node_argument'}->{'manual_content'}},
                 {'code' => 1,
                  Texinfo::Convert::Text::copy_options_for_convert_text($self, 1)});
          }
          if (defined($manual)) {
            my $manual_base = $manual;
            $manual_base =~ s/\.[^\.]*$//;
            $manual_base =~ s/^.*\///;
            
            push @$attribute, ['manual', $manual_base]
                  if ($manual_base ne '');
          }
        }
      }
      # this never happens as there is no @-commands with more than one
      # argument that is also a context brace_commands.  Leading spaces
      # after the command brace opening are only associated with command
      # if a context brace_commands, therefore they are with the first argument.
      push @$attribute, _leading_spaces_arg($element);
      return $self->txi_markup_open_element($main_cmdname, $attribute)
                 .$args_or_one_arg_cmd.$self->txi_markup_close_element($main_cmdname);
    } elsif (exists($Texinfo::Commands::block_commands{$element->{'cmdname'}})) {
      if ($self->{'context_block_commands'}->{$element->{'cmdname'}}) {
        push @{$self->{'document_context'}}, {'monospace' => [0]};
      }
      my $prepended_elements = '';
      my $attribute = [];
      $self->{'itemize_line'} = 1 if ($element->{'cmdname'} eq 'itemize');
      if ($element->{'extra'} and $element->{'extra'}->{'command_as_argument'}) {
        my $command_as_arg = $element->{'extra'}->{'command_as_argument'};
        push @$attribute,
         (['commandarg', $command_as_arg->{'cmdname'}],
             $self->_infoenclose_attribute($command_as_arg));
      } elsif ($element->{'extra'}
               and $element->{'extra'}->{'enumerate_specification'}) {
        push @$attribute, ['first', $element->{'extra'}->{'enumerate_specification'}];
      } elsif ($element->{'cmdname'} eq 'float' and $element->{'extra'}) {
        if (defined($element->{'extra'}->{'node_content'})) {
          my $normalized =
            Texinfo::Convert::NodeNameNormalization::normalize_node (
                   { 'contents' => $element->{'extra'}->{'node_content'} });
          push @$attribute, ['name', $normalized];
        }
        if ($element->{'extra'}->{'type'} and
            defined($element->{'extra'}->{'type'}->{'normalized'})) {
          push @$attribute, ['type', $element->{'extra'}->{'type'}->{'normalized'}];
        }
        if ($element->{'structure'}
            and defined($element->{'structure'}->{'float_number'})) {
          push @$attribute, ['number', $element->{'structure'}->{'float_number'}];
        }
      } elsif ($element->{'cmdname'} eq 'verbatim') {
        push @$attribute, ['space', 'preserve'];
      } elsif ($element->{'cmdname'} eq 'macro'
               or $element->{'cmdname'} eq 'rmacro') {
        if (defined($element->{'args'})) {
          my @args = @{$element->{'args'}};
          my $name_arg = shift @args;
          if (defined($name_arg) and defined($name_arg->{'text'})) {
            push @$attribute, ['name', $name_arg->{'text'}];
          }
          
          while (@args) {
            my $formal_arg = shift @args;
            $prepended_elements .= $self->txi_markup_open_element('formalarg')
                .$self->txi_markup_protect_text($formal_arg->{'text'})
                .$self->txi_markup_close_element('formalarg');
          }
        }
        push @$attribute, $self->_arg_line($element);
      }
      if ($self->{'expanded_formats_hash'}->{$element->{'cmdname'}}) {
        $self->{'document_context'}->[-1]->{'raw'} = 1;
      } else {
        my $end_command;
        if ($element->{'contents'} and scalar(@{$element->{'contents'}}) > 0
            and $element->{'contents'}->[-1]->{'cmdname'}
            and $element->{'contents'}->[-1]->{'cmdname'} eq 'end') {
          $end_command = $element->{'contents'}->[-1];
        }
        my @end_command_spaces;
        push @end_command_spaces, _leading_spaces_arg($end_command);
        if (scalar(@end_command_spaces)) {
          $end_command_spaces[0]->[0] = 'endspaces';
        }
        $result .= $self->txi_markup_open_element($element->{'cmdname'},
                                   [@$attribute, _leading_spaces_arg($element),
                                    @end_command_spaces])
                   .${prepended_elements};
        if ($element->{'args'}) {
          my $variadic_element = undef;
          my $last_empty_element;
          my $end_line = '';
          if ($commands_args_elements{$element->{'cmdname'}}) {
            my $arg_index = 0;
            foreach my $arg_element (@{$element->{'args'}}) {
              my $format_element;
              if (defined($variadic_element)) {
                $format_element = $variadic_element;
              } else {
                if ($commands_args_elements{$element->{'cmdname'}}
                                                      ->[$arg_index] eq '*') {
                  $variadic_element
                        = $commands_args_elements{$element->{'cmdname'}}
                                                               ->[$arg_index-1];
                  $format_element = $variadic_element;
                  # the last argument was empty, it is the same argument
                  # as the variadic argument, it needs to be output to have
                  # it count as the last non variadic argument.
                  if ($last_empty_element) {
                    $result .= $last_empty_element;
                    $last_empty_element = undef;
                  }
                } else {
                  $format_element
                    = $commands_args_elements{$element->{'cmdname'}}
                                                        ->[$arg_index];
                }
              }
              my $spaces = [];
              my $arg = '';
              my $end_space = '';
              if (defined($arg_element)) {
                my $in_code;
                 $in_code = 1
                  if (defined($default_args_code_style{$element->{'cmdname'}})
                    and $default_args_code_style{$element->{'cmdname'}}
                                                               ->[$arg_index]);
                push @{$self->{'document_context'}->[-1]->{'monospace'}}, 1
                  if ($in_code);
                if ($arg_index+1 eq scalar(@{$element->{'args'}})) {
                  # last argument
                  ($arg, $end_space, $end_line)
                    = $self->_convert_argument_and_end_line($element);
                } else {
                  $arg = $self->_convert($arg_element);
                }
                pop @{$self->{'document_context'}->[-1]->{'monospace'}}
                  if ($in_code);
                if ($arg_index != 0) {
                  push @$spaces, _leading_spaces_arg($arg_element);
                }
              }
              # must add every variadic argument even if empty to get the correct count
              if ($arg ne '' or scalar(@$spaces) or $variadic_element) {
                $result .= $self->txi_markup_open_element($format_element, $spaces)
                                   .$arg.$end_space
                                   .$self->txi_markup_close_element($format_element);
                $last_empty_element = undef;
              } else {
                $result .= $end_space;
                if ($arg_index > 0) {
                  # we keep the last empty argument to be able to prepend it to be able
                  # to reconstitute trailing empty arguments in the original Texinfo code.
                  $last_empty_element = $self->txi_markup_open_element($format_element)
                                .$self->txi_markup_close_element($format_element);
                }
              }
              $arg_index++;
            }
            if ($last_empty_element) {
              $result .= $last_empty_element;
            }
            $result .= $end_line;
          } else {

            # in that case the end of line is in the columnfractions line
            # or in the columnprototypes.
            if ($element->{'cmdname'} eq 'multitable') {
              my @prototype_line;
              if (not $element->{'extra'}->{'columnfractions'}) {
                # Like 'prototypes' extra value, but keeping spaces information
                if (defined $element->{'args'}->[0]
                    and defined $element->{'args'}->[0]->{'type'}
                    and $element->{'args'}->[0]->{'type'} eq 'block_line_arg'
                    and $element->{'args'}->[0]->{'contents'}) {
                  foreach my $content (@{$element->{'args'}->[0]->{'contents'}}) {
                    if ($content->{'type'} and $content->{'type'} eq 'bracketed') {
                      push @prototype_line, $content;
                    } elsif ($content->{'text'}) {
                      # The regexp breaks between characters, with a non space
                      # followed by a space or a space followed by non space.
                      # It is like \b, but for \s \S, and not \w \W.
                      foreach my $prototype_or_space (
                          split /(?<=\S)(?=\s)|(?=\S)(?<=\s)/, $content->{'text'}) {
                        if ($prototype_or_space =~ /\S/) {
                          push @prototype_line, {'text' => $prototype_or_space,
                            'type' => 'row_prototype' };
                        } elsif ($prototype_or_space =~ /\s/) {
                          push @prototype_line, {'text' => $prototype_or_space,
                            'type' => 'prototype_space' };
                        }
                      }
                    # $content->{'cmdname'} should be defined at this point, if not,
                    # there should be a perl warning
                    } elsif ($content->{'cmdname'} eq 'c'
                             or $content->{'cmdname'} eq 'comment') {
                      # NOTE it does not happen right now, because a comment
                      # will be in extra comment_at_end.  If comments are back
                      # in the tree, they should be ignored here, as they would
                      # better be handled in format_comment_or_return_end_line
                    } else { # a command
                      push @prototype_line, $content;
                    }
                  }
                }
              }

              if (scalar(@prototype_line) > 0) {
                $result .= $self->txi_markup_open_element('columnprototypes');
                my $first_proto = 1;
                foreach my $prototype (@prototype_line) {
                  if ($prototype->{'text'} and $prototype->{'text'} !~ /\S/) {
                    if (!$first_proto) {
                      my $spaces = $prototype->{'text'};
                      chomp($spaces);
                      $result .= $spaces;
                    }
                  } else {
                    my $attributes = [];
                    if ($prototype->{'type'}
                        and $prototype->{'type'} eq 'bracketed') {
                      push @$attributes, ['bracketed', 'on'];
                      push @$attributes,
                                  _leading_spaces_arg($prototype);
                    }
                    $result .= $self->txi_markup_open_element('columnprototype',
                                                              $attributes)
                           .$self->_convert($prototype)
                           .$self->txi_markup_close_element('columnprototype');
                  }
                  $first_proto = 0;
                }
                $result .= $self->txi_markup_close_element('columnprototypes');
                $result .= $self->format_comment_or_return_end_line($element);
              } elsif ($element->{'extra'}
                       and $element->{'extra'}->{'columnfractions'}
                       and $element->{'args'}->[0]->{'contents'}) {
                my $cmd;
                foreach my $content (@{$element->{'args'}->[0]->{'contents'}}) {
                  if ($content->{'cmdname'}
                      and $content->{'cmdname'} eq 'columnfractions') {
                    $cmd = $content;
                    last;
                  }
                }
                my $attribute = [_leading_spaces_arg($cmd),
                                 $self->_texinfo_line($cmd)];
                $result .= $self->txi_markup_open_element('columnfractions', $attribute);
                foreach my $fraction (@{$element->{'extra'}->{'columnfractions'}
                                             ->{'extra'}->{'misc_args'}}) {
                  $result .= $self->txi_markup_open_element('columnfraction',
                                                [['value', $fraction]])
                             .$self->txi_markup_close_element('columnfraction');
                }
                $result .= $self->txi_markup_close_element('columnfractions');
                $result .= $self->format_comment_or_return_end_line($cmd);
              } else { # bogus multitable
                $result .= "\n";
              }
            } else {
              # get end of lines from @*table and block @-commands with
              # no argument that have a bogus argument.
              $result .= _end_line_spaces($self, $element);
              $result .= $self->format_comment_or_return_end_line($element);
            }
          }
        }
        unshift @close_format_elements, $element->{'cmdname'};
      }
      delete $self->{'itemize_line'} if ($self->{'itemize_line'});
    }
  }
  if ($element->{'type'}) {
    if (defined($type_elements{$element->{'type'}})) {
      my $attribute = [];
      if ($element->{'type'} eq 'preformatted') {
        push @$attribute, ['space', 'preserve'];
      } elsif ($element->{'type'} eq 'menu_entry') {
        push @$attribute, ['leadingtext',
                           $self->_convert($element->{'args'}->[0])];
      } elsif (($element->{'type'} eq 'menu_entry_node'
                or $element->{'type'} eq 'menu_entry_name')
               and $self->{'pending_menu_entry_separator'}) {
        push @$attribute, ['separator',
               $self->_convert($self->{'pending_menu_entry_separator'})];
        delete $self->{'pending_menu_entry_separator'};
      }
      $result
        .= $self->txi_markup_open_element($type_elements{$element->{'type'}},
                                          $attribute);
    }
    if ($element->{'type'} eq 'def_line') {
      if ($element->{'cmdname'}) {
        my $leading_spaces_attribute_spec = [];
        if ($element->{'extra'}
            and $element->{'extra'}->{'spaces_before_argument'}
            and $element->{'extra'}->{'spaces_before_argument'} ne '') {
          my $leading_spaces = $element->{'extra'}->{'spaces_before_argument'};
          # may happen without any argument, remove as a \n is added below
          $leading_spaces =~ s/\n//;
          $leading_spaces_attribute_spec = [['spaces',
                          _protect_in_spaces_attribute_text($leading_spaces)]]
            if ($leading_spaces ne '');
        }
        $result .= $self->txi_markup_open_element($element->{'cmdname'},
                                                  $leading_spaces_attribute_spec);
      }
      $result .= $self->txi_markup_open_element('definitionterm');
      $result .= $self->_index_entry($element);
      push @{$self->{'document_context'}->[-1]->{'monospace'}}, 1;
      if ($element->{'args'} and @{$element->{'args'}}
          and $element->{'args'}->[0]->{'contents'}) {
        my $main_command;
        my $alias;
        if ($Texinfo::Common::def_aliases{$element->{'extra'}->{'def_command'}}) {
          $main_command
            = $Texinfo::Common::def_aliases{$element->{'extra'}->{'def_command'}};
          $alias = 1;
        } else {
          $main_command = $element->{'extra'}->{'def_command'};
          $alias = 0;
        }
        foreach my $arg (@{$element->{'args'}->[0]->{'contents'}}) {
          my $type = $arg->{'extra'}->{'def_role'};
          next if !$type and $arg->{'type'} eq 'spaces';
          my $content = $self->_convert($arg);
          if ($type eq 'spaces') {
            $content =~ s/\n$//;
            $result .= $content;
          } else {
            my $attribute = [];
            if ($type eq 'category' and $alias) {
              push @$attribute, ['automatic', 'on'];
            }
            my $format_element;
            if ($type eq 'name') {
              $format_element = $defcommand_name_type{$main_command};
            } elsif ($type eq 'arg') {
              $format_element = 'param';
            } elsif ($type eq 'typearg') {
              $format_element = 'paramtype';
            } else {
              $format_element = $type;
            }
            if ($arg->{'type'}
                and ($arg->{'type'} eq 'bracketed_def_content'
                  or ($arg->{'type'} eq 'bracketed_inserted'))) {
              push @$attribute, ['bracketed', 'on'];
              push @$attribute, _leading_trailing_spaces_arg($arg);
            }
            $result
              .= $self->txi_markup_open_element("def$format_element", $attribute)
                      .$content
                      .$self->txi_markup_close_element("def$format_element");
          }
        }
      }
      pop @{$self->{'document_context'}->[-1]->{'monospace'}};
      $result .= $self->txi_markup_close_element('definitionterm');
      if ($element->{'cmdname'}) {
        $result .= $self->txi_markup_close_element($element->{'cmdname'});
      }
      chomp ($result);
      $result .= "\n";
    }
  }
  if ($element->{'contents'}) {
    my $in_code;
    if ($element->{'cmdname'}
        and ($Texinfo::Commands::preformatted_code_commands{$element->{'cmdname'}}
             or $Texinfo::Commands::math_commands{$element->{'cmdname'}})) {
      $in_code = 1;
    }
    push @{$self->{'document_context'}->[-1]->{'monospace'}}, 1
      if ($in_code);
    if (ref($element->{'contents'}) ne 'ARRAY') {
      cluck "contents not an array($element->{'contents'}).";
    }
    foreach my $content (@{$element->{'contents'}}) {
      $result .= $self->_convert($content);
    }
    pop @{$self->{'document_context'}->[-1]->{'monospace'}}
      if ($in_code);
  }
  my $arg_nr = -1;
  if ($element->{'type'} and $element->{'type'} eq 'menu_entry') {
    foreach my $arg (@{$element->{'args'}}) {
      $arg_nr++;
      # menu_entry_leading_text is added as attribute leadingtext of menu_entry
      # menu_entry_separator is recorded here and then added ass attribute
      # separator
      next if ($arg->{'type'} eq 'menu_entry_leading_text'
               or $arg->{'type'} eq 'menu_entry_separator');
      if ($element->{'args'}->[$arg_nr +1]
          and $element->{'args'}->[$arg_nr +1]->{'type'}
          and $element->{'args'}->[$arg_nr +1]->{'type'} eq 'menu_entry_separator') {
        $self->{'pending_menu_entry_separator'} = $element->{'args'}->[$arg_nr +1];
      }
      my $in_code;
      if ($arg->{'type'} eq 'menu_entry_node') {
        $in_code = 1;
      }
      push @{$self->{'document_context'}->[-1]->{'monospace'}}, 1
        if ($in_code);
      $result .= $self->_convert($arg);
      pop @{$self->{'document_context'}->[-1]->{'monospace'}}
        if ($in_code);
    }
  }
  if ($element->{'type'}) {
    if (defined($type_elements{$element->{'type'}})) {
      $result
        .= $self->txi_markup_close_element($type_elements{$element->{'type'}});
    }
  }
  $result = '{'.$result.'}'
     if ($element->{'type'} and $element->{'type'} eq 'bracketed'
         and (!$element->{'parent'}->{'type'} or
              ($element->{'parent'}->{'type'} ne 'block_line_arg'
               and $element->{'parent'}->{'type'} ne 'line_arg')));
  foreach my $format_element (@close_format_elements) {
    $result .= $self->txi_markup_close_element($format_element);
  }
  if ($element->{'cmdname'}
      and exists($Texinfo::Commands::block_commands{$element->{'cmdname'}})) {
    if ($self->{'expanded_formats_hash'}->{$element->{'cmdname'}}) {
    } else {
      if ($element->{'contents'} and scalar(@{$element->{'contents'}}) > 0
          and $element->{'contents'}->[-1]->{'cmdname'}
          and $element->{'contents'}->[-1]->{'cmdname'} eq 'end') {
        my $end_command = $element->{'contents'}->[-1];
        $result .= _end_line_spaces($self, $end_command);
        $result .= $self->format_comment_or_return_end_line($end_command);
      }
    }
    if ($self->{'context_block_commands'}->{$element->{'cmdname'}}) {
      pop @{$self->{'document_context'}};
    }
  # The command is closed either when the corresponding tree element
  # is done, and the command is not associated to an element, or when
  # the element is closed.
  } elsif ((($element->{'type'} and $element->{'type'} eq 'unit'
             and $element->{'extra'} and $element->{'extra'}->{'unit_command'}
             and !($element->{'extra'}->{'unit_command'}->{'cmdname'}
                   and $element->{'extra'}->{'unit_command'}->{'cmdname'} eq 'node'))
            or ($element->{'cmdname'}
                and $Texinfo::Commands::root_commands{$element->{'cmdname'}}
                and $element->{'cmdname'} ne 'node'
                and !($element->{'structure'}->{'associated_unit'}
                     and $element->{'structure'}->{'associated_unit'}->{'extra'}
                     and $element->{'structure'}->{'associated_unit'}->{'extra'}->{'unit_command'}
                     and $element->{'structure'}->{'associated_unit'}->{'extra'}->{'unit_command'} eq $element)))
           and !$self->get_conf('USE_NODES')) {
    if ($element->{'type'} and $element->{'type'} eq 'unit') {
      $element = $element->{'extra'}->{'unit_command'};
    }
    my $level_adjusted_cmdname
       = Texinfo::Structuring::section_level_adjusted_command_name($element);
    if (!($element->{'structure'}->{'section_childs'}
          and scalar(@{$element->{'structure'}->{'section_childs'}}))
        or $level_adjusted_cmdname eq 'top') {
      $result .= $self->txi_markup_close_element($level_adjusted_cmdname)."\n";
      my $current = $element;
      while ($current->{'structure'}->{'section_up'}
             # the most up element is a virtual sectioning root element, this
             # condition avoids getting into it
             and $current->{'structure'}->{'section_up'}->{'cmdname'}
             and !$current->{'structure'}->{'section_next'}
             and Texinfo::Structuring::section_level_adjusted_command_name(
                                     $current->{'structure'}->{'section_up'}) ne 'top') {
        $current = $current->{'structure'}->{'section_up'};
        my $level_adjusted_current_cmdname
            = Texinfo::Structuring::section_level_adjusted_command_name($current);
        $result .= $self->txi_markup_close_element($level_adjusted_current_cmdname) ."\n";
      }
    }
    if ($self->{'pending_bye'}) {
      $result .= $self->{'pending_bye'};
      delete $self->{'pending_bye'};
    }
  } elsif ((($element->{'type'} and $element->{'type'} eq 'unit'
             and $element->{'extra'} and $element->{'extra'}->{'unit_command'}
             and $element->{'extra'}->{'unit_command'}->{'cmdname'}
             and $element->{'extra'}->{'unit_command'}->{'cmdname'} eq 'node')
            or ($element->{'cmdname'}
                and $element->{'cmdname'} eq 'node'
                and !($element->{'structure'}->{'associated_unit'}
                     and $element->{'structure'}->{'associated_unit'}->{'extra'}
                     and $element->{'structure'}->{'associated_unit'}->{'extra'}->{'unit_command'}
                     and $element->{'structure'}->{'associated_unit'}->{'extra'}->{'unit_command'} eq $element)))
           and $self->get_conf('USE_NODES')) {
    $result .= $self->txi_markup_close_element('node');
    
    if ($self->{'pending_bye'}) {
      $result .= $self->{'pending_bye'};
      delete $self->{'pending_bye'};
    }
  }
  return $result;
}

1;

__END__

=head1 NAME

Texinfo::Convert::TexinfoMarkup - Convert Texinfo tree to element and attribute markup

=head1 SYNOPSIS

  package Texinfo::Convert::TexinfoMyMarkup;

  use Texinfo::Convert::TexinfoMarkup;

  @ISA = qw(Texinfo::Convert::TexinfoMarkup);

  sub converter_defaults ($$) {
    return %myconverter_defaults;
  }

  sub txi_markup_protect_text($$)
  {
    my $self = shift;
    ....
  }

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

C<Texinfo::Convert::TexinfoMarkup> converts a Texinfo tree to the Texinfo
Markup Language which is based on nested elements with attributes, similar
to XML.  All the information present in the Texinfo tree, after
expansion of C<@macro>, C<@value> and inclusion of include files is
kept.  C<Texinfo::Convert::TexinfoMarkup> is an abstract class, to be used as a
super class for modules implementing specific markup formatting functions
called by C<Texinfo::Convert::TexinfoMarkup>.

The Texinfo Markup Language elements and attributes are not documented,
but the Texinfo XML output by the C<Texinfo::Convert::TexinfoXML> subclass
(L<Texinfo::Convert::TexinfoXML>) is a straightforward formatting as XML,
and is described by the texinfo DTD.  Therefore the texinfo DTD can be
used as a description of the structure of both Texinfo XML and of the
more abstract Texinfo Markup Language.

=head1 METHODS

=head2 Markup formatting methods defined by subclasses

The following methods should be implemented by the modules inheriting
from C<Texinfo::Convert::TexinfoMarkup>:

=over

=item $result = $converter->txi_markup_atom($atom)

Format the I<$atom> symbol string in a simpler way than with an element.  For
example in XML the formatting of the symbol is achieved with an entity.

=item $result = $converter->txi_markup_comment($comment_string)

Format I<$comment_string> as a comment.

=item $result = $converter->txi_markup_convert_text($element)

Called to format the Texinfo tree I<$element> text, which is a
reference on a hash.  The I<$element> text is in the C<text> key.
The C<type> key value may also be set to distinguish the type of text
(L<Texinfo::Parser/Types for text elements>).
Texinfo tree elements are described in details in L<Texinfo::Parser/TEXINFO TREE>.

=item $result = $converter->txi_markup_element($format_element, $attributes)

=item $result = $converter->txi_markup_open_element($format_element, $attributes)

=item $result = $converter->txi_markup_close_element($format_element, $attributes)

C<txi_markup_element> is called for the formatting of empty elements.  Otherwise,
C<txi_markup_open_element> is called when an element is opened, and
C<txi_markup_close_element> is called when an element is closed.
I<$format_element> is the element name, I<$attributes> is a reference on an
array containing references on arrays of pairs, one pair for each attribute, with
the attribute name as the first item of the pair and the attribute text as the
second item of the pair.

=item $result = $converter->txi_markup_header()

Called to format a header at the beginning of output files.

=item $result = $converter->txi_markup_protect_text($string)

Protect special character in text for text fragments out of text
texinfo tree elements.  For example, for spaces at end of line that
are ignorable in most output formats, for C<@set> or C<@macro>
arguments.

=back

=head2 Formatting state information

A method is available for subclasses to gather information on the
formatting state:

=over

=item $converter->in_monospace()

Return 1 if in a context where spacing should be kept
and C<---> or C<''> left as is, for example in C<@code>, C<@example>.

=back

=head1 AUTHOR

Patrice Dumas, E<lt>pertusus@free.frE<gt>

=head1 SEE ALSO

L<Texinfo::Convert::Converter>. L<Texinfo::Convert::TexinfoXML>.  The
C<Texinfo::Convert::TexinfoSXML> is another subclass, which outputs
SXML.  It is not much documented.

=head1 COPYRIGHT AND LICENSE

Copyright 2010- Free Software Foundation, Inc.  See the source file for
all copyright years.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

=cut
