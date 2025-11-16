# DocBook.pm: output tree as DocBook.
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

package Texinfo::Convert::DocBook;

use 5.00405;
use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Texinfo::Commands;
use Texinfo::Common;

# for debugging
use Texinfo::Convert::Texinfo;

use Texinfo::Convert::Unicode;
use Texinfo::Convert::Utils;
use Texinfo::Convert::Text;
use Texinfo::Convert::Converter;
use Texinfo::Convert::Plaintext;
use Data::Dumper;
use Carp qw(cluck);

require Exporter;
use vars qw($VERSION @ISA);
@ISA = qw(Texinfo::Convert::Converter);

$VERSION = '7.0.3';

my %brace_commands = %Texinfo::Commands::brace_commands;

my $nbsp = '&#'.hex('00A0').';';

my %defaults = (
  #'ENABLE_ENCODING'      => 0,
  'FORMAT_MENU'          => 'nomenu',
  'EXTENSION'            => 'xml', # dbk?
  'OUTPUT_ENCODING_NAME' => 'utf-8',
  'converted_format'     => 'docbook',
  'SPLIT'                => 0,
  'documentlanguage'     => undef,
  'OPEN_QUOTE_SYMBOL'    => '&#'.hex('2018').';',
  'CLOSE_QUOTE_SYMBOL'   => '&#'.hex('2019').';',
  'USE_NUMERIC_ENTITY'   => 1,
  'NO_TOP_NODE_OUTPUT'   => 1,
);

my @docbook_image_extensions
  = ('eps', 'gif', 'jpg', 'jpeg', 'pdf', 'png', 'svg');

my %docbook_special_quotations;
foreach my $special_quotation (
    'caution', 'important', 'note', 'tip', 'warning') {
  $docbook_special_quotations{$special_quotation} = 1;
}

# For '*', there is no line break in DocBook, except in cmdsynopsis (in this
# case it is <sbr>).  But currently we don't use cmdsynopsis, and it is
# unlikely we ever will.
my %docbook_specific_formatting = (
  'TeX' => '&tex;',
  'LaTeX' => '&latex;',
  "\t" => $nbsp,
  "\n" => $nbsp,
  " " => $nbsp,
  'tie' => $nbsp,
);
my %docbook_no_arg_commands_formatting
  = %Texinfo::Convert::Converter::xml_text_entity_no_arg_commands_formatting;

foreach my $command (keys(%Texinfo::Convert::Unicode::unicode_entities)) {
  $docbook_no_arg_commands_formatting{$command}
   = $Texinfo::Convert::Unicode::unicode_entities{$command};
}

foreach my $command (keys(%docbook_specific_formatting)) {
  $docbook_no_arg_commands_formatting{$command}
    = $docbook_specific_formatting{$command};
}

my %quoted_style_commands = (
  'samp' => 1,
);

# FIXME allow customization? (as in HTML)
my %upper_case_style_commands = (
  'sc' => 1,
);

my @inline_elements = ('emphasis', 'abbrev', 'acronym', 'link', 
  'inlinemediaobject', 'firstterm', 'footnote', 'replaceable',
  'subscript', 'superscript', 'wordasword');
my %inline_elements;
foreach my $inline_element (@inline_elements) {
  $inline_elements{$inline_element} = 1;
};

my %style_attribute_commands;
%style_attribute_commands = (
      'b'           => 'emphasis role="bold"',
      'cite'        => 'citetitle',
      'code'        => 'literal',
      'command'     => 'command',
      'dfn'         => 'firstterm',
      'emph'        => 'emphasis',
      'env'         => 'envar',
      'file'        => 'filename',
      'footnote'    => 'footnote',   # brace context command
      'headitemfont' => 'emphasis role="bold"', # actually <th> instead of <td>
      'i'           => 'emphasis',
      'indicateurl' => 'literal',
      'kbd'         => 'userinput',
      'key'         => 'keycap',
      'math'        => 'mathphrase', # brace context command
      'option'      => 'option',
      'r'           => '',
      'samp'        => 'literal',
      'sansserif'   => '',
      'strong'      => 'emphasis role="bold"',
      'sub'         => 'subscript',
      'sup'         => 'superscript',
      't'           => 'literal',
      'var'         => 'replaceable',
      'verb'        => 'literal',     # brace other command
);

my %style_brace_types = map {$_ => 1} ('style_other', 'style_code', 'style_no_code');
# @all_style_commands is the union of style brace commands, commands
# in %style_attribute_commands and a few other, some not style brace commands.
# Using keys of a map generated hash does like uniq, it avoids duplicates.
# The first grep selects style brace commands, ie commands with %brace_commands
# type in %style_brace_types.
my @all_style_commands = keys %{{ map { $_ => 1 }
    ((grep {$style_brace_types{$brace_commands{$_}}} keys(%brace_commands)),
      keys(%style_attribute_commands), 'w', 'dmn', 'titlefont') }};

# special string for 'w'.
my $w_command_mark = '<!-- /@w -->';

my %style_commands_formatting;
foreach my $command(@all_style_commands) {
  $style_commands_formatting{$command} = {};
  if ($style_attribute_commands{$command}) {
    $style_commands_formatting{$command}->{'attribute'}
      = $style_attribute_commands{$command};
  }
  if ($quoted_style_commands{$command}) {
    $style_commands_formatting{$command}->{'quote'} = 1;
  }
  if ($upper_case_style_commands{$command}) {
    $style_commands_formatting{$command}->{'upper_case'} = 1;
  }
}

my %docbook_line_elements_with_arg_map = (
  'exdent' => 'simpara role="exdent"',
  'center' => 'simpara role="center"',
);

my %docbook_nobrace_commands = %Texinfo::Commands::nobrace_commands;
foreach my $command ('item', 'headitem', 'tab',
   keys(%docbook_no_arg_commands_formatting)) {
  delete $docbook_nobrace_commands{$command};
}

my %docbook_line_commands = %Texinfo::Commands::line_commands;
foreach my $command ('itemx', keys %Texinfo::Commands::def_commands) {
  delete $docbook_line_commands{$command};
}

my %docbook_global_commands = (
  'documentlanguage' => 1,
);

my %default_args_code_style
  = %Texinfo::Convert::Converter::default_args_code_style;

my %defcommand_name_type = (
 'defcv'     => 'property',
 'deffn'     => 'function',
 'defop'     => 'methodname',
 'deftp'     => 'structname',
 'deftypecv' => 'property',
 'deftypefn' => 'function',
 'deftypeop' => 'methodname',
 'deftypevr' => 'varname',
 'defvr'     => 'varname',
);

my %def_argument_types_docbook = (
  'type' => ['returnvalue'],
  'class' => ['ooclass', 'classname'],
  # FIXME or a simple emphasis?
  # replaceable is not used here, such that replaceable is only
  # used if there is an explicit @var{}
  'arg' => ['emphasis role="arg"'],
  'typearg' => ['type'],
);

my %ignored_block_commands;
foreach my $block_command ('copying', 'titlepage', 'documentdescription') {
  $ignored_block_commands{$block_command} = 1;
}

my %ignored_types;
foreach my $type (
            'ignorable_spaces_after_command',
            'spaces_after_close_brace',
            'spaces_before_paragraph',
            'menu_entry_leading_text',
            'menu_entry_separator',
            'preamble_before_beginning',
            'preamble_before_setfilename',
            'spaces_at_end',
  ) {
  $ignored_types{$type} = 1;
}

my %type_elements = (
  'paragraph' => 'para',
  'table_item' => 'listitem',
  'table_entry' => 'varlistentry',
  'row' => 'row',
  'multitable_head' => 'thead',
  'multitable_body' => 'tbody',
  # Unfortunatly there does not seem to be anything better in DocBook.
  'def_item' => 'blockquote',
);

my %default_context_block_commands = (
  'float' => 1,
);

my %docbook_preformatted_formats = (
# command
   'example' => 'screen',
   'smallexample' => 'screen',
   'display' => 'literallayout',
   'smalldisplay' => 'literallayout',
   'lisp' => 'programlisting',
   'smalllisp' => 'programlisting',
   'format' => 'literallayout',
   'smallformat' => 'literallayout',
# type
   'menu_comment' => 'literallayout',
   'menu_description' => 'literallayout',
);

my %sectioning_commands_done;

sub converter_defaults($$)
{
  return %defaults;
}


sub converter_initialize($)
{
  my $self = shift;

  $self->{'document_context'} = [{'monospace' => [0], 'upper_case' => [0]}];
  $self->{'context_block_commands'} = {%default_context_block_commands};
  foreach my $raw (grep {$Texinfo::Commands::block_commands{$_} eq 'format_raw'}
                        keys(%Texinfo::Commands::block_commands)) {
    $self->{'context_block_commands'}->{$raw} = 1
         if $self->{'expanded_formats_hash'}->{$raw};
  }
}

sub convert($$)
{
  my $self = shift;
  my $root = shift;

  if (! defined($self->{'lang_stack'})) {
    $self->{'lang_stack'} = [''];
  }
  # could even set to 0 if defined?
  $self->{'in_skipped_node_top'} = 0
    if (! defined($self->{'in_skipped_node_top'}));

  %sectioning_commands_done = ();
  return $self->convert_tree($root);
}

sub convert_tree($$)
{
  my $self = shift;
  my $root = shift;

  if (! defined($self->{'lang_stack'})) {
    $self->{'lang_stack'} = [''];
  }
  return $self->_convert($root);
}

# not the same as a default for @documentlanguage.  $DEFAULT_LANG
# is used in the lang attribute, but if there is no @documentlanguag,
# the lang_stack will start with an empty string, not with $DEFAULT_LANG.
my $DEFAULT_LANG = 'en';
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

  my $encoding = '';
  if ($self->get_conf('OUTPUT_ENCODING_NAME')
      and $self->get_conf('OUTPUT_ENCODING_NAME') ne 'utf-8') {
    $encoding = " encoding=\"".$self->get_conf('OUTPUT_ENCODING_NAME')."\" ";
  }

  my $id;
  if ($output_file ne '') {
    $id = " id=\"".$self->xml_protect_text($output_filename)."\"";
  } else {
    $id = '';
  }

  $self->{'lang_stack'} = [];
  $self->{'in_skipped_node_top'} = 0;
  my $lang = $DEFAULT_LANG;
  $self->set_global_document_commands('preamble', ['documentlanguage']);
  if (defined($self->get_conf('documentlanguage'))) {
    $lang = $self->get_conf('documentlanguage');
    push @{$self->{'lang_stack'}}, $self->get_conf('documentlanguage');
  } else {
    push @{$self->{'lang_stack'}}, '';
  }
  my $header =  "<?xml version=\"1.0\"${encoding}?>".'
<!DOCTYPE book PUBLIC "-//OASIS//DTD DocBook XML V4.5//EN" "http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd" [
  <!ENTITY tex "TeX">
  <!ENTITY latex "LaTeX">
]>
'. "<book${id} lang=\"$lang\">\n";

  my $legalnotice;
  if ($self->{'global_commands'}->{'copying'}) {
    my $copying_element = $self->{'global_commands'}->{'copying'};
    my $copying_result
     = $self->convert_tree({'contents' => $copying_element->{'contents'}});
    if ($copying_result ne '') {
      $legalnotice = "<legalnotice>$copying_result</legalnotice>";
    }
  }

  my $fulltitle_command;
  foreach my $title_cmdname ('title', 'shorttitlepage', 'titlefont') {
    if ($self->{'global_commands'}->{$title_cmdname}) {
      my $command = $self->{'global_commands'}->{$title_cmdname};
      next if (!$command->{'args'}
               or (!$command->{'args'}->[0]->{'contents'}
                   or $command->{'extra'}->{'missing_argument'}));
      $fulltitle_command = $command;
      last;
    }
  }

  # get informations from the @titlepage.  Since the fulltitle is gathered
  # independently, only author and subtitle are gathered here.
  my $subtitle_info = '';
  my $authors_info = '';
  if ($self->{'global_commands'}->{'titlepage'}) {
    my $collected_commands = Texinfo::Common::collect_commands_list_in_tree(
            $self->{'global_commands'}->{'titlepage'}, ['author', 'subtitle']);

    my @authors_elements;
    my $subtitle_text = '';
    if (scalar(@{$collected_commands})) {
      foreach my $element (@{$collected_commands}) {
        my $cmdname = $element->{'cmdname'};
        if ($cmdname eq 'author') {
          push @authors_elements, $element;
        } elsif ($cmdname eq 'subtitle') {
          # concatenate the text of @subtitle as DocBook only allows one.
          my ($arg, $end_line) = $self->_convert_argument_and_end_line($element);
          $subtitle_text .= $arg . $end_line
        }
      }
    }
    if ($subtitle_text ne '') {
      chomp ($subtitle_text);
      $subtitle_info = "<subtitle>$subtitle_text</subtitle>\n";
    }

    if (scalar(@authors_elements)) {
      # using authorgroup and collab is the best, because it doesn't require
      # knowing people name decomposition.  Also it should work for group names.
      # FIXME dblatex ignores collab/collabname.
      $authors_info .= "<authorgroup>\n";
      foreach my $element (@authors_elements) {
        my ($arg, $end_line) = $self->_convert_argument_and_end_line($element);
        my $result = "<collab><collabname>$arg</collabname></collab>$end_line";
        chomp ($result);
        $result .= "\n";
        $authors_info .= $result;
      }
      $authors_info .= "</authorgroup>\n";
    }
  }

  my $settitle_command;
  if ($self->{'global_commands'}->{'settitle'}) {
    my $command = $self->{'global_commands'}->{'settitle'};
    $settitle_command = $command
      unless (!$command->{'args'}
               or (!$command->{'args'}->[0]->{'contents'}
                   or $command->{'extra'}->{'missing_argument'}));

  }

  my $titleabbrev_command;
  if ($fulltitle_command) {
    $titleabbrev_command = $settitle_command;
  } elsif ($settitle_command) {
    $fulltitle_command = $settitle_command;
  } elsif (defined($legalnotice) and $self->{'global_commands'}->{'top'}) {
    # if there is a legalnotice, we really want to have a title
    # preceding it, so we also use @top
    my $command = $self->{'global_commands'}->{'top'};
    $fulltitle_command = $command
      unless (!$command->{'args'}
               or (!$command->{'args'}->[0]->{'contents'}
                   or $command->{'extra'}->{'missing_argument'}));
  }

  my $title_info = '';
  
  if ($fulltitle_command) {
    foreach my $element_command ([$fulltitle_command, 'title'],
                                 [$titleabbrev_command, 'titleabbrev']) {
      my ($element, $docbook_element) = @$element_command;
      if (defined($element)) {
        my ($arg, $end_line) = $self->_convert_argument_and_end_line($element);
        my $result = "<$docbook_element>$arg</$docbook_element>$end_line";
        chomp ($result);
        $result .= "\n";
        $title_info .= $result;
        if ($docbook_element eq 'title') {
          $title_info .= $subtitle_info;
        }
      }
    }
  }
  $self->set_global_document_commands('before', ['documentlanguage']);

  my $document_info = '';
  $document_info .= $title_info . $authors_info;
  $document_info .= $legalnotice if (defined($legalnotice));

  # we duplicate title info, as it is explicitly said in the DocBook manual
  # that it can be duplicated if exactly the same
  $header .= $title_info;

  if ($document_info ne '') {
    # FIXME DocBook 5 bookinfo->info
    $header .= "<bookinfo>$document_info</bookinfo>\n";
  }

  %sectioning_commands_done = ();
  my $result = '';
  $result .= $self->write_or_return($header, $fh);
  $result .= $self->write_or_return($self->convert_tree($root), $fh);
  $result .= $self->write_or_return("</book>\n", $fh);
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

my %docbook_sections = (
  'top'  => 'chapter',
  'part' => 'part',
  'chapter'  => 'chapter',
  'unnumbered'  => 'chapter',
  'centerchap'  => 'chapter',
  'appendix' => 'appendix',
  # chapter would have been the best choice here, but it is not a possibility
  # for renderas in bridgehead (possibilities are sect1 to 5 and other).
  # other is rendered small, smaller than sect1.  So use sect1, even though
  # it matches better with the @heading level.
  'majorheading' => 'sect1',
  'chapheading' => 'sect1',
  'heading' => 'sect1',
  'subheading' => 'sect2',
  'subsubheading' => 'sect3',
  2 => 'sect1',
  3 => 'sect2',
  4 => 'sect3'
);

my %docbook_special_unnumbered;
foreach my $special_unnumbered ('acknowledgements', 'colophon',
                                'dedication', 'preface') {
  $docbook_special_unnumbered{$special_unnumbered} = 1;
}

# element is not texinfo tree element here, but xml element
sub _docbook_section_element($$)
{
  my $self = shift;
  my $element = shift;

  my $heading_level = $element->{'structure'}->{'section_level'};
  if (exists $docbook_sections{$heading_level}) {
    return $docbook_sections{$heading_level};
  }
  my $level_adjusted_cmdname
     = Texinfo::Structuring::section_level_adjusted_command_name($element);
  if ($level_adjusted_cmdname eq 'unnumbered'
      and $element->{'extra'}->{'associated_node'}
      and $element->{'extra'}->{'associated_node'}->{'extra'}->{'normalized'}
      and $docbook_special_unnumbered{lc($element->{'extra'}->{'associated_node'}->{'extra'}->{'normalized'})}) {
    return lc($element->{'extra'}->{'associated_node'}->{'extra'}->{'normalized'});
  }

  return $docbook_sections{$level_adjusted_cmdname};
}

sub _index_entry($$)
{
  my $self = shift;
  my $element = shift;
  if ($element->{'extra'} and $element->{'extra'}->{'index_entry'}) {
    my $index_entry = $element->{'extra'}->{'index_entry'};
    # FIXME DocBook 5 role->type
    my $result = "<indexterm role=\"$index_entry->{'index_name'}\">";

    push @{$self->{'document_context'}}, {'monospace' => [0], 'upper_case' => [0]};
    $self->{'document_context'}->[-1]->{'monospace'}->[-1] = 1
      if ($index_entry->{'in_code'});

    $result .= "<primary>";
    $result .= $self->_convert({'contents' => $index_entry->{'entry_content'}});
    $result .= "</primary>";

    # Add any index subentries.
    my $tmp = $index_entry->{'entry_element'};
    my $level = "secondary";
    while ($tmp->{'extra'}->{'subentry'}) {
      $result .= "<$level>";
      $tmp = $tmp->{'extra'}->{'subentry'};
      $result .= $self->_convert($tmp->{'args'}->[0]);
      $result .= "</$level>";
      $level = "tertiary";
    }
    if ($index_entry->{'entry_element'}->{'extra'}->{'seeentry'}) {
      $result .= "<see>";
      $result .= $self->_convert({'contents'
        => $index_entry->{'entry_element'}->{'extra'}->{'seeentry'}->{'args'}});
      $result .= "</see>";
    }
    if ($index_entry->{'entry_element'}->{'extra'}->{'seealso'}) {
      $result .= "<seealso>";
      $result .= $self->_convert({'contents'
        => $index_entry->{'entry_element'}->{'extra'}->{'seealso'}->{'args'}});
      $result .= "</seealso>";
    }

    pop @{$self->{'document_context'}};

    $result .= "</indexterm>";
    return $result;
  }
  return '';
}

sub _parse_attribute($)
{
  my $element = shift;
  return ('', '') if (!defined($element));
  my $attributes = '';
  if ($element =~ /^(\w+)(\s+.*)/) {
    $element = $1;
    $attributes = $2;
  }
  return ($element, $attributes);
}

sub _protect_text($$)
{
  my $self = shift;
  my $text = shift;
  my $result = $self->xml_protect_text($text);
  # form feed not allowed in XML
  $result =~ s/\f/ /g;
  return $result;
}

sub _convert_argument_and_end_line($$)
{
  my $self = shift;
  my $element = shift;

  my $converted = $self->convert_tree($element->{'args'}->[-1]);
  my $end_line = $self->format_comment_or_return_end_line($element);
  return ($converted, $end_line);
}

my $debug_global_element_nr = 0;


sub _convert($$;$);

sub _convert($$;$)
{
  my $self = shift;
  my $element = shift;

  my $debug_element_nr;
  #if (1) {
  if (0) { # verbose even for debugging
    $debug_element_nr = $debug_global_element_nr++;
    print STDERR "element $debug_element_nr";
    print STDERR " cmd: $element->{'cmdname'}," if ($element->{'cmdname'});
    print STDERR " type: $element->{'type'}" if ($element->{'type'});
    my $text = $element->{'text'};
    if (defined($text)) {
      $text =~ s/\n/\\n/;
      print STDERR " text: $text";
    }
    print STDERR "\n";
  }

  return '' if ($element->{'type'} and $ignored_types{$element->{'type'}});
  my $result = '';
  if (defined($element->{'text'})) {
    if (defined($element->{'type'}) and $element->{'type'} eq '_converted') {
      return $element->{'text'};
    } elsif ($self->{'document_context'}->[-1]->{'raw'}) {
      return $element->{'text'};
    }
    $result = $element->{'text'};
    if ($self->{'document_context'}->[-1]->{'upper_case'}->[-1]) {
      $result = uc($result);
    }
    $result = $self->_protect_text($result);
    if (! defined($element->{'type'}) or $element->{'type'} ne 'raw') {
      if (!$self->{'document_context'}->[-1]->{'monospace'}->[-1]) {
        $result = $self->xml_format_text_with_numeric_entities($result);
      }
    }
    #warn "had text `$element->{'text'}', returning $result\n";
    return $result;
  }

  #warn " onto main conditional\n";
  my @close_format_elements;
  if ($element->{'cmdname'}) {
    #warn "  got cmdname $element->{'cmdname'}\n";
    if (defined($docbook_no_arg_commands_formatting{$element->{'cmdname'}})) {
      #warn "  has no_arg_commands_formatting \n";
      my $command;
      if ($element->{'cmdname'} eq 'click'
          and $element->{'extra'} 
          and defined($element->{'extra'}->{'clickstyle'})) {
        $command = $element->{'extra'}->{'clickstyle'};
      } elsif ($self->{'document_context'}->[-1]->{'upper_case'}->[-1]
               and $Texinfo::Commands::letter_no_arg_commands{$element->{'cmdname'}}
               and $Texinfo::Commands::letter_no_arg_commands{uc($element->{'cmdname'})}) {
        $command = uc($element->{'cmdname'})
      } else {
        $command = $element->{'cmdname'};
      }
      my $translated_tree = Texinfo::Convert::Utils::translated_command_tree($self,
                                                                           $command);
      if ($translated_tree) {
        return $self->_convert($translated_tree);
      } else {
        return $docbook_no_arg_commands_formatting{$command};
      }
    } elsif ($element->{'cmdname'} eq 'today') {
      return $self->_convert(Texinfo::Convert::Utils::expand_today($self));
    } elsif ($Texinfo::Commands::accent_commands{$element->{'cmdname'}}) {
      return $self->xml_accents($element,
               $self->{'document_context'}->[-1]->{'upper_case'}->[-1]);
    } elsif ($element->{'cmdname'} eq 'item' or $element->{'cmdname'} eq 'itemx'
             or $element->{'cmdname'} eq 'headitem' or $element->{'cmdname'} eq 'tab') {
      if ($element->{'cmdname'} eq 'item'
          and $element->{'parent'}->{'cmdname'}
          and ($element->{'parent'}->{'cmdname'} eq 'itemize'
               or $element->{'parent'}->{'cmdname'} eq 'enumerate')) {
        $result .= "<listitem>";
        if ($element->{'parent'}->{'cmdname'} eq 'itemize'
            and $element->{'parent'}->{'extra'}
            and !($element->{'parent'}->{'extra'}->{'command_as_argument'}
                  and $element->{'parent'}->{'extra'}->{'command_as_argument'}->{'cmdname'} eq 'bullet')
            and $element->{'parent'}->{'args'}
            and $element->{'parent'}->{'args'}->[0]) {
          $self->{'pending_prepend'}
            = $self->_convert($element->{'parent'}->{'args'}->[0]);
          $self->{'pending_prepend'} .= " ";
        }
        push @close_format_elements, 'listitem';
      } elsif (($element->{'cmdname'} eq 'item' or $element->{'cmdname'} eq 'itemx')
               and $element->{'parent'}->{'type'}
               and $element->{'parent'}->{'type'} eq 'table_term') {

        $result .= "<term>";
        $result .= $self->_index_entry($element);
        if ($element->{'args'}->[0]
            and $element->{'args'}->[0]->{'contents'}) {
          my $table_item_tree = $self->table_item_content_tree($element,
                                         $element->{'args'}->[0]->{'contents'});

          $result .= $self->_convert($table_item_tree);
        }
        chomp ($result);
        $result .= "\n";
        $result .= "</term>";
      } else {
        unless (($element->{'cmdname'} eq 'item'
                     or $element->{'cmdname'} eq 'headitem'
                     or $element->{'cmdname'} eq 'tab')
                    and $element->{'parent'}->{'type'}
                    and $element->{'parent'}->{'type'} eq 'row') {
          warn "BUG: multitable cell command not in a row "
            .Texinfo::Common::debug_print_element($element);
        }
        $result .= "<entry>";
        push @close_format_elements, 'entry';
      }
    } elsif ($element->{'type'} and $element->{'type'} eq 'index_entry_command') {
      my $end_line;
      if ($element->{'args'}->[0]) {
        $end_line = $self->format_comment_or_return_end_line($element);
        if ($self->{'document_context'}->[-1]->{'in_preformatted'}) {
          chomp($end_line);
        }
      } else {
        # May that happen?
        $end_line = '';
      }
      return $self->_index_entry($element).${end_line};

    } elsif (exists($docbook_nobrace_commands{$element->{'cmdname'}})) {
      return '';
    } elsif (exists($docbook_line_commands{$element->{'cmdname'}})) {
      #warn "  is dbk line command\n";
      if ($docbook_global_commands{$element->{'cmdname'}}) {
        Texinfo::Common::set_informative_command_value($self, $element);
        return '';
      }
      if ($Texinfo::Commands::root_commands{$element->{'cmdname'}}) {
        if ($self->get_conf('NO_TOP_NODE_OUTPUT')) {
          my $node_element;
          if ($element->{'cmdname'} eq 'node') {
            $node_element = $element;
          } elsif ($element->{'cmdname'} eq 'part' and $element->{'extra'}
                 and $element->{'extra'}->{'part_following_node'}) {
            $node_element = $element->{'extra'}->{'part_following_node'};
          }
          if ($node_element or $element->{'cmdname'} eq 'part') {
            # $node_element->{'extra'}->{'normalized'} not defined happens for
            # empty nodes
            if ($node_element and $node_element->{'extra'}
                and $node_element->{'extra'}->{'normalized'}
                and $node_element->{'extra'}->{'normalized'} eq 'Top') {
              $self->{'in_skipped_node_top'} = 1;
            } elsif (defined($self->{'in_skipped_node_top'})
                     and $self->{'in_skipped_node_top'} == 1) {
              $self->{'in_skipped_node_top'} = -1;
            }
          }
        }
        if ($element->{'cmdname'} eq 'node'
            and (not $element->{'extra'}
                 or not $element->{'extra'}->{'associated_section'})) {
          if ($element->{'extra'} and defined($element->{'extra'}->{'normalized'})) {
            $result .= "<anchor id=\"$element->{'extra'}->{'normalized'}\"/>\n";
          }
        } else {
          # start the section at the associated node or part, or at the sectioning
          # command if there is no associated node nor part
          my $section_element;
          my $part;
          if ($element->{'cmdname'} eq 'node') {
            $section_element = $element->{'extra'}->{'associated_section'};
          } elsif ($element->{'cmdname'} eq 'part') {
            $part = $element;
            if ($element->{'extra'}->{'part_associated_section'}) {
              $section_element = $element->{'extra'}->{'part_associated_section'};
            }
          } else {
            $section_element = $element;
          }
          if ($section_element and $section_element->{'extra'}
              and $section_element->{'extra'}->{'associated_part'}) {
            $part = $section_element->{'extra'}->{'associated_part'};
          }
          my @opened_elements;
          # we need to check if the section was already done in case there is
          # both a node and a part, we do not know in which order they appear.
          if (not ($section_element
                   and $sectioning_commands_done{$section_element})) {
            push @opened_elements, $part if $part;
            if ($section_element) {
              push @opened_elements, $section_element;
            }
          }
          foreach my $opened_element (@opened_elements) {
            if (not (defined($self->{'in_skipped_node_top'})
                     and $self->{'in_skipped_node_top'} == 1)) {
              if ($section_element and $opened_element eq $section_element) {
                $sectioning_commands_done{$section_element} = 1;
              }
              my $section_attribute = '';
              # FIXME it is not clear that a label should be set for
              # @appendix* or @chapter/@*section as the formatter should be
              # able to figure it out.  For @unnumbered or if ! NUMBER_SECTIONS
              # having a label (empty) is important.
              my $label = '';
              if (defined($opened_element->{'structure'}->{'section_number'})
                and ($self->get_conf('NUMBER_SECTIONS')
                     or !defined($self->get_conf('NUMBER_SECTIONS')))) {
                # Looking at docbook2html output, Appendix is appended in the
                # section title, so only the letter is used.
                $label = $opened_element->{'structure'}->{'section_number'};
              }
              my $docbook_sectioning_element
                 = $self->_docbook_section_element($opened_element);
              if (! $docbook_special_unnumbered{$docbook_sectioning_element}) {
                $section_attribute .= " label=\"$label\"";
              }
              if ($opened_element->{'extra'} and $opened_element->{'extra'}->{'associated_node'}) {
                $section_attribute
                 .= " id=\"$opened_element->{'extra'}->{'associated_node'}->{'extra'}->{'normalized'}\"";
              }
              my $language = '';
              if (defined($self->get_conf('documentlanguage'))) {
                $language = $self->get_conf('documentlanguage');
                if ($self->{'lang_stack'}->[-1] ne $language) {
                  $section_attribute .= ' lang="'.$language.'"';
                }
              }
              push @{$self->{'lang_stack'}}, $language;
              $result .= "<$docbook_sectioning_element${section_attribute}>\n";
              if ($opened_element->{'args'} and $opened_element->{'args'}->[0]) {
                my ($arg, $end_line) = $self->_convert_argument_and_end_line($opened_element);
                $result .= "<title>$arg</title>$end_line";
                chomp ($result);
                $result .= "\n";
              }
              # if associated with a sectioning element, the part is opened before the
              # sectioning element, such that the part content appears after the sectioning
              # command opening, no need for partintro.
              if ($docbook_sectioning_element eq 'part'
                  and not ($opened_element->{'extra'}
                           and $opened_element->{'extra'}->{'part_associated_section'})
                  and !Texinfo::Common::is_content_empty($opened_element)) {
                $result .= "<partintro>\n";
              }
            } else {
              push @{$self->{'lang_stack'}}, $self->{'lang_stack'}->[-1];
            }
          }
        }
      } elsif ($element->{'cmdname'} eq 'c' or $element->{'cmdname'} eq 'comment') {
        return $self->xml_comment($element->{'args'}->[0]->{'text'})
      } elsif ($Texinfo::Commands::sectioning_heading_commands{$element->{'cmdname'}}) {
        if ($element->{'args'} and $element->{'args'}->[0]) {
          my ($arg, $end_line) = $self->_convert_argument_and_end_line($element);
          $result .=
            "<bridgehead renderas=\"$docbook_sections{$element->{'cmdname'}}\">$arg</bridgehead>$end_line";
          chomp ($result);
          $result .= "\n";
          return $result;
        }
        return '';
      } elsif (exists ($docbook_line_elements_with_arg_map{$element->{'cmdname'}})) {
        my ($docbook_element, $attribute_text)
          = _parse_attribute($docbook_line_elements_with_arg_map{$element->{'cmdname'}});
        my ($arg, $end_line)
          = $self->_convert_argument_and_end_line($element);
        if ($docbook_element eq '') {
          $result .= "$arg$end_line";
        } else {
          $result .= "<$docbook_element${attribute_text}>$arg</$docbook_element>$end_line";
        }
        chomp ($result);
        $result .= "\n";
        return $result;
      } elsif ($element->{'cmdname'} eq 'insertcopying') {
        if ($self->{'global_commands'}
           and $self->{'global_commands'}->{'copying'}) {
         return $self->_convert({'contents'
            => $self->{'global_commands'}->{'copying'}->{'contents'}});
        } else {
          return '';
        }
      } elsif ($element->{'cmdname'} eq 'verbatiminclude') {
        my $verbatim_include_verbatim
          = Texinfo::Convert::Utils::expand_verbatiminclude($self, $self, $element);
        if (defined($verbatim_include_verbatim)) {
          $result .= $self->_convert($verbatim_include_verbatim);
        } else {
          return '';
        }
      } elsif ($element->{'cmdname'} eq 'printindex') {
        if (defined($element->{'extra'})
            and defined($element->{'extra'}->{'misc_args'})) {
          # FIXME DocBook 5
          #return "<index type=\"$element->{'extra'}->{'misc_args'}->[0]\"></index>\n";
          return "<index role=\"$element->{'extra'}->{'misc_args'}->[0]\"></index>\n";
        } else {
          return "<index></index>\n";
        }
      } else {
        # ignore all the other line commands
        return '';
      }
    } elsif ($element->{'type'}
             and $element->{'type'} eq 'definfoenclose_command') {
      my $arg = $self->_convert($element->{'args'}->[0]);
      $result .= $arg;

    } elsif ($element->{'args'}
             and exists($Texinfo::Commands::brace_commands{$element->{'cmdname'}})) {
      #Texinfo::Common::debug_list(" brace command with args", $element->{'args'});
      if ($style_commands_formatting{$element->{'cmdname'}}) {
        if ($Texinfo::Commands::brace_commands{$element->{'cmdname'}} eq 'context') {
          push (@{$self->{'document_context'}},
                {'monospace' => [0], 'upper_case' => [0]});
        }
        my $formatting = $style_commands_formatting{$element->{'cmdname'}};

        my $in_monospace_not_normal;
        if (defined($default_args_code_style{$element->{'cmdname'}})
            and $default_args_code_style{$element->{'cmdname'}}->[0]) {
           $in_monospace_not_normal = 1;
        } elsif ($brace_commands{$element->{'cmdname'}}
                 and $brace_commands{$element->{'cmdname'}} eq 'style_no_code') {
          $in_monospace_not_normal = 0;
        }
        if ($formatting->{'upper_case'}) {
          push @{$self->{'document_context'}->[-1]->{'upper_case'}}, 1;
        }
        push @{$self->{'document_context'}->[-1]->{'monospace'}}, 
          $in_monospace_not_normal
            if (defined($in_monospace_not_normal));

        my ($style, $attribute_text) = _parse_attribute($formatting->{'attribute'});
        my $result = $self->_convert($element->{'args'}->[0]);
        if ($style ne '' and (!$self->{'document_context'}->[-1]->{'inline'}
                               or $inline_elements{$style})) {
          $result = "<$style${attribute_text}>$result</$style>";
          if ($element->{'cmdname'} eq 'math') {
            $result = "<inlineequation>$result</inlineequation>";
          }
        }
        if (defined($formatting->{'quote'})) {
          $result = $self->get_conf('OPEN_QUOTE_SYMBOL') . $result
                   . $self->get_conf('CLOSE_QUOTE_SYMBOL');
        }
        if (defined($formatting->{'upper_case'})) {
          pop @{$self->{'document_context'}->[-1]->{'upper_case'}};
        }
        pop @{$self->{'document_context'}->[-1]->{'monospace'}}
          if (defined($in_monospace_not_normal));
        if ($Texinfo::Commands::brace_commands{$element->{'cmdname'}} eq 'context') {
          pop @{$self->{'document_context'}};
        }
        if ($element->{'cmdname'} eq 'w') {
          $result .= $w_command_mark;
        }
        return $result;
      } elsif ($element->{'cmdname'} eq 'anchor') {
        if ($element->{'extra'} and defined($element->{'extra'}->{'normalized'})) {
          return "<anchor id=\"$element->{'extra'}->{'normalized'}\"/>";
        } else {
          return '';
        }
      } elsif ($Texinfo::Commands::ref_commands{$element->{'cmdname'}}) {
        if ($element->{'args'}) {
          my $cmdname;
          my $book_contents;
          my ($section_name, $node_name);
          my $manual_file_index = 3;
          if ($element->{'cmdname'} eq 'inforef') {
            $manual_file_index = 2;
            $cmdname = 'xref';
          } else {
            if (scalar(@{$element->{'args'}}) == 5
                and defined($element->{'args'}->[-1])
                and $element->{'args'}->[-1]->{'contents'}
                and @{$element->{'args'}->[-1]->{'contents'}}) {
              $book_contents = $element->{'args'}->[-1]->{'contents'};
            }
            if (defined($element->{'args'}->[2])
                and $element->{'args'}->[2]->{'contents'}
                and @{$element->{'args'}->[2]->{'contents'}}) {
              my $section_name_contents = $element->{'args'}->[2]->{'contents'};
              $section_name = $self->_convert(
                   {'contents' => $section_name_contents});
            }
            $cmdname = $element->{'cmdname'};
          }
          my $manual_file_contents;
          if (scalar(@{$element->{'args'}}) >= $manual_file_index+1
              and defined($element->{'args'}->[$manual_file_index])
              and $element->{'args'}->[$manual_file_index]->{'contents'}
              and @{$element->{'args'}->[$manual_file_index]->{'contents'}}) {
            $manual_file_contents = $element->{'args'}->[$manual_file_index]->{'contents'};
          }
          if (! defined($section_name) and defined($element->{'args'}->[1])
              and $element->{'args'}->[1]->{'contents'}
              and @{$element->{'args'}->[1]->{'contents'}}) {
            my $section_name_contents
              = $element->{'args'}->[1]->{'contents'};
            $section_name = $self->_convert(
                   {'contents' => $section_name_contents});
          } elsif (defined($element->{'args'}->[0])
                   and $element->{'args'}->[0]->{'contents'}
                   and @{$element->{'args'}->[0]->{'contents'}}) {
            my $node_contents
              = $element->{'args'}->[0]->{'contents'};
            $node_name = $self->_convert(
                   {'contents' => $node_contents});

            if (($book_contents or $manual_file_contents)
                and $node_name eq 'Top') {
              $node_name = undef;
            }
          }

          # external book ref
          if ($book_contents) {
            if ($section_name) {
              if ($cmdname eq 'ref') {
                return $self->_convert(
                  $self->gdt('section ``{section_name}\'\' in @cite{{book}}',
                    { 'section_name' => {'type' => '_converted', 'text' => $section_name},
                      'book' => $book_contents }));
              } elsif ($cmdname eq 'xref') {
                return $self->_convert(
                  $self->gdt('See section ``{section_name}\'\' in @cite{{book}}',
                    { 'section_name' => {'type' => '_converted', 'text' => $section_name},
                      'book' => $book_contents }));
              } elsif ($cmdname eq 'pxref') {
                return $self->_convert(
                  $self->gdt('see section ``{section_name}\'\' in @cite{{book}}',
                    { 'section_name' => {'type' => '_converted', 'text' => $section_name},
                      'book' => $book_contents }));
              }
            } elsif ($node_name) {
              if ($cmdname eq 'ref') {
                return $self->_convert(
                  $self->gdt('``{node_name}\'\' in @cite{{book}}',
                    { 'node_name' => {'type' => '_converted', 'text' => $node_name},
                      'book' => $book_contents }));
              } elsif ($cmdname eq 'xref') {
                return $self->_convert(
                  $self->gdt('See ``{node_name}\'\' in @cite{{book}}',
                    { 'node_name' => {'type' => '_converted', 'text' => $node_name},
                      'book' => $book_contents }));
              } elsif ($cmdname eq 'pxref') {
                return $self->_convert(
                  $self->gdt('see ``{node_name}\'\' in @cite{{book}}',
                    { 'node_name' => {'type' => '_converted', 'text' => $node_name},
                      'book' => $book_contents }));
              }
            } else {
              if ($cmdname eq 'ref') {
                return $self->_convert(
                  $self->gdt('@cite{{book}}',
                    {'book' => $book_contents }));
              } elsif ($cmdname eq 'xref') {
                return $self->_convert(
                  $self->gdt('See @cite{{book}}',
                    {'book' => $book_contents }));
              } elsif ($cmdname eq 'pxref') {
                return $self->_convert(
                  $self->gdt('see @cite{{book}}',
                    {'book' => $book_contents }));
              }
            }
          } elsif ($manual_file_contents) {
            if ($section_name) {
              if ($cmdname eq 'ref') {
                return $self->_convert(
                  $self->gdt('section ``{section_name}\'\' in @file{{manual}}',
                    { 'section_name' => {'type' => '_converted', 'text' => $section_name},
                      'manual' => $manual_file_contents }));
              } elsif ($cmdname eq 'xref') {
                return $self->_convert(
                  $self->gdt('See section ``{section_name}\'\' in @file{{manual}}',
                    { 'section_name' => {'type' => '_converted', 'text' => $section_name},
                      'manual' => $manual_file_contents }));
              } elsif ($cmdname eq 'pxref') {
                return $self->_convert(
                  $self->gdt('see section ``{section_name}\'\' in @file{{manual}}',
                    { 'section_name' => {'type' => '_converted', 'text' => $section_name},
                      'manual' => $manual_file_contents }));
              }
            } elsif ($node_name) {
              if ($cmdname eq 'ref') {
                return $self->_convert(
                  $self->gdt('``{node_name}\'\' in @file{{manual}}',
                    { 'node_name' => {'type' => '_converted', 'text' => $node_name},
                      'manual' => $manual_file_contents }));
              } elsif ($cmdname eq 'xref') {
                return $self->_convert(
                  $self->gdt('See ``{node_name}\'\' in @file{{manual}}',
                    { 'node_name' => {'type' => '_converted', 'text' => $node_name},
                      'manual' => $manual_file_contents }));
              } elsif ($cmdname eq 'pxref') {
                return $self->_convert(
                  $self->gdt('see ``{node_name}\'\' in @file{{manual}}',
                    { 'node_name' => {'type' => '_converted', 'text' => $node_name},
                      'manual' => $manual_file_contents }));
              }
            } else {
              if ($cmdname eq 'ref') {
                return $self->_convert(
                  $self->gdt('@file{{manual}}',
                    {'manual' => $manual_file_contents }));
              } elsif ($cmdname eq 'xref') {
                return $self->_convert(
                  $self->gdt('See @file{{manual}}',
                    {'manual' => $manual_file_contents }));
              } elsif ($cmdname eq 'pxref') {
                return $self->_convert(
                  $self->gdt('see @file{{manual}}',
                    {'manual' => $manual_file_contents }));
              }
            }
          } elsif ($element->{'cmdname'} eq 'inforef') {
            return '';
          } else {
            my $linkend = '';
            if ($element->{'extra'}
                and $element->{'extra'}->{'node_argument'}
                and defined($element->{'extra'}->{'node_argument'}->{'normalized'})
                and !$element->{'extra'}->{'node_argument'}->{'manual_content'}) {
              $linkend = " linkend=\"$element->{'extra'}->{'node_argument'}->{'normalized'}\"";
            }
            my $link_text = $section_name;
            $link_text = $node_name if (! defined($link_text));
            my $argument = "<link${linkend}>".$link_text."</link>";
            if ($element->{'cmdname'} eq 'ref') {
              return $self->_convert(
                      $self->gdt('{title_ref}', {'title_ref' =>
                           {'type' => '_converted',
                            'text' => $argument}}));
            } elsif ($element->{'cmdname'} eq 'xref') {
              return $self->_convert(
                      $self->gdt('See {title_ref}', {'title_ref' =>
                           {'type' => '_converted',
                            'text' => $argument}}));
            } elsif ($element->{'cmdname'} eq 'pxref') {
              return $self->_convert(
                      $self->gdt('see {title_ref}', {'title_ref' =>
                           {'type' => '_converted',
                            'text' => $argument}}));
            }
          }
        } else {
          return '';
        }
      } elsif ($element->{'cmdname'} eq 'image') {
        if (defined($element->{'args'}->[0])
            and $element->{'args'}->[0]->{'contents'}
            and @{$element->{'args'}->[0]->{'contents'}}) {
          my $basefile = Texinfo::Convert::Text::convert_to_text(
           {'contents' => $element->{'args'}->[0]->{'contents'}},
           {'code' => 1,
            Texinfo::Convert::Text::copy_options_for_convert_text($self, 1)});
          my $is_inline = Texinfo::Common::element_is_inline($element);
          if ($is_inline) {
            $result .= "<inlinemediaobject>";
          } else {
            $result .= "<informalfigure><mediaobject>";
          }
          my @files;
          foreach my $extension (@docbook_image_extensions) {
            my ($file_name, $file_name_encoding)
               = $self->encoded_input_file_name("$basefile.$extension");
            if ($self->Texinfo::Common::locate_include_file($file_name)) {
              push @files, ["$basefile.$extension", uc($extension)];
            }
          }
          my $image_file_found = scalar(@files);;
          if (!$image_file_found) {
            push @files, ["$basefile.jpg", 'JPG'];
          }
          foreach my $file (@files) {
            $result .= "<imageobject><imagedata fileref=\""
               .$self->xml_protect_text($file->[0])
               ."\" format=\"$file->[1]\"></imagedata></imageobject>";
          }
          my ($image_text, $image_width) = $self->txt_image_text($element, $basefile);
          if (defined($image_text)) {
            $result .= "<textobject><literallayout>"
               .$self->_protect_text($image_text)
               .'</literallayout></textobject>';
          }
          if (!defined($image_text) and !$image_file_found) {
            $self->line_warn($self, sprintf(
                     __("\@image file `%s' not found, using `%s'"), 
                       $basefile, "$basefile.jpg"), $element->{'source_info'});
          }

          if ($is_inline) {
            $result .= "</inlinemediaobject>";
          } else {
            $result .= "</mediaobject></informalfigure>";
          }
        }
      } elsif ($element->{'cmdname'} eq 'email') {
        if ($element->{'args'}) {
          my $name;
          my $email;
          my $email_text;
          if (scalar(@{$element->{'args'}}) == 2
              and defined($element->{'args'}->[-1])
              and $element->{'args'}->[-1]->{'contents'}
              and @{$element->{'args'}->[-1]->{'contents'}}) {
            $name = $element->{'args'}->[1]->{'contents'};
          }
          if (defined($element->{'args'}->[0])
              and $element->{'args'}->[0]->{'contents'}
              and @{$element->{'args'}->[0]->{'contents'}}) {
            $email = $element->{'args'}->[0]->{'contents'};
            $email_text
              = $self->_protect_text(Texinfo::Convert::Text::convert_to_text(
                 {'contents' => $email},
                 {'code' => 1,
                  Texinfo::Convert::Text::copy_options_for_convert_text($self, 1)}));
          }
          if ($name and $email) {
            return "<ulink url=\"mailto:$email_text\">"
              .$self->_convert({'contents' => $name}).'</ulink>';
          } elsif ($email) {
            return "<email>$email_text</email>";
          } elsif ($name) {
            return $self->_convert({'contents' => $name});
          }
        } else {
          return '';
        }

      } elsif ($element->{'cmdname'} eq 'uref' or $element->{'cmdname'} eq 'url') {
        if ($element->{'args'}) {
          my ($url_text, $url_content);
          if (defined($element->{'args'}->[0])
              and $element->{'args'}->[0]->{'contents'}
              and @{$element->{'args'}->[0]->{'contents'}}) {
            $url_content = $element->{'args'}->[0]->{'contents'};
            $url_text = $self->_protect_text(
              Texinfo::Convert::Text::convert_to_text(
                {'contents' => $url_content},
                {'code' => 1,
                 Texinfo::Convert::Text::copy_options_for_convert_text($self, 1)}));
          } else {
            $url_text = '';
          }
          my $replacement;
          if (scalar(@{$element->{'args'}}) >= 2 
              and defined($element->{'args'}->[1])
              and $element->{'args'}->[1]->{'contents'}
              and @{$element->{'args'}->[1]->{'contents'}}) {
            $replacement = $self->_convert({'contents' 
                      => $element->{'args'}->[1]->{'contents'}});
          }
          if (!defined($replacement) or $replacement eq '') {
            if (scalar(@{$element->{'args'}}) == 3
                and defined($element->{'args'}->[2])
                and $element->{'args'}->[2]->{'contents'}
                and @{$element->{'args'}->[2]->{'contents'}}) {
              $replacement = $self->_convert({'contents' 
                      => $element->{'args'}->[2]->{'contents'}});
            }
          }
          if (!defined($replacement) or $replacement eq '') {
            $replacement = $url_text;
          }
          return "<ulink url=\"$url_text\">$replacement</ulink>";
          # FIXME DocBook 5
          # return "<link xlink:href=\"$url_text\">$replacement</link>";
        }

      } elsif ($element->{'cmdname'} eq 'abbr' or $element->{'cmdname'} eq 'acronym') {
        my $argument;
        if (scalar(@{$element->{'args'}}) >= 1
            and defined($element->{'args'}->[0])
            and $element->{'args'}->[0]->{'contents'}
            and @{$element->{'args'}->[0]->{'contents'}}) {
          my $arg = $self->_convert({'contents' 
                      => $element->{'args'}->[0]->{'contents'}});
          if ($arg ne '') {
            my $format_element;
            if ($element->{'cmdname'} eq 'abbr') {
              $format_element = 'abbrev';
            } else {
              $format_element = $element->{'cmdname'};
            }
            $argument = "<$format_element>$arg</$format_element>";
          }
        }
        #
        if (scalar(@{$element->{'args'}}) == 2
           and defined($element->{'args'}->[-1])
           and $element->{'args'}->[-1]->{'contents'}
           and @{$element->{'args'}->[-1]->{'contents'}}) {
          if (defined($argument)) {
            my $tree = $self->gdt('{abbr_or_acronym} ({explanation})',
                           {'abbr_or_acronym' => {'type' => '_converted',
                                                  'text' => $argument},
                            'explanation' =>
                             $element->{'args'}->[-1]->{'contents'}});
            return $self->_convert($tree);
          } else {
            return $self->_convert({'contents' 
                    => $element->{'args'}->[-1]->{'contents'}});
          }
        } elsif (defined($argument)) {
          return $argument;
        } else {
          return '';
        }

      } elsif ($element->{'cmdname'} eq 'U') {
        my $argument;
        if ($element->{'args'}
            and $element->{'args'}->[0]
            and $element->{'args'}->[0]->{'contents'}
            and $element->{'args'}->[0]->{'contents'}->[0]
            and $element->{'args'}->[0]->{'contents'}->[0]->{'text'}) {
          $argument = $element->{'args'}->[0]->{'contents'}->[0]->{'text'};
        }
        if ($argument) {
          $result = "&#x$argument;";
        } else {
          $self->line_warn($self,
                  __("no argument specified for \@U"),
                           $element->{'source_info'});
          $result = '';
        }
        return $result;

      } elsif ($Texinfo::Commands::brace_commands{$element->{'cmdname'}} eq 'inline') {
        my $expand = 0;
        if ($Texinfo::Common::inline_format_commands{$element->{'cmdname'}}) {
          if ($element->{'cmdname'} eq 'inlinefmtifelse'
              or ($element->{'extra'} and $element->{'extra'}->{'format'}
                  and $self->{'expanded_formats_hash'}->{$element->{'extra'}->{'format'}})) {
            $expand = 1;
          }
        } elsif (defined($element->{'extra'}->{'expand_index'})) {
          $expand = 1;
        }
        return '' if (! $expand);
        my $arg_index = 1;
        if ($element->{'cmdname'} eq 'inlineraw') {
          push @{$self->{'document_context'}}, {'monospace' => [0], 'upper_case' => [0]};
          $self->{'document_context'}->[-1]->{'raw'} = 1;
        } elsif ($element->{'cmdname'} eq 'inlinefmtifelse'
                 and ! $self->{'expanded_formats_hash'}->{$element->{'extra'}->{'format'}}) {
          $arg_index = 2;
        }
        if (scalar(@{$element->{'args'}}) > $arg_index
            and defined($element->{'args'}->[$arg_index])
            and $element->{'args'}->[$arg_index]->{'contents'}
            and @{$element->{'args'}->[$arg_index]->{'contents'}}) {
          $result .= $self->_convert({'contents'
                        => $element->{'args'}->[$arg_index]->{'contents'}});
        }
        if ($element->{'cmdname'} eq 'inlineraw') {
          pop @{$self->{'document_context'}};
        }
        #warn "  returning braced cmd result $result\n";
        return $result;
      } else {
        # ignored brace command
        #warn "  returning empty string for ignored braced cmd\n";
        return '';
      }

    # special case to ensure that @w leads to something even if empty
    } elsif ($element->{'cmdname'} eq 'w') {
      return $w_command_mark;

    } elsif (exists($Texinfo::Commands::block_commands{$element->{'cmdname'}})) {
      if ($ignored_block_commands{$element->{'cmdname'}}) {
        return '';
      }
      if ($self->{'context_block_commands'}->{$element->{'cmdname'}}) {
        push (@{$self->{'document_context'}},
              {'monospace' => [0], 'upper_case' => [0]});
      }
      my @attributes;
      my $appended = '';
      my @format_elements;
      if (exists($docbook_preformatted_formats{$element->{'cmdname'}})) {
        push @{$self->{'document_context'}->[-1]->{'preformatted_stack'}}, 
           $docbook_preformatted_formats{$element->{'cmdname'}};
      } elsif ($element->{'cmdname'} eq 'enumerate') {
        push @format_elements, 'orderedlist'; 
        my $numeration;
        if ($element->{'extra'}
               and $element->{'extra'}->{'enumerate_specification'}) {
          if ($element->{'extra'}->{'enumerate_specification'} =~ /^[A-Z]/) {
            $numeration = 'upperalpha';
          } elsif ($element->{'extra'}->{'enumerate_specification'} =~ /^[a-z]/) {
            $numeration = 'loweralpha';
          } else {
            $numeration = 'arabic';
          }
        } else {
          $numeration = 'arabic';
        }
        push @attributes, " numeration=\"$numeration\"";
      } elsif ($Texinfo::Commands::block_commands{$element->{'cmdname'}} eq 'item_line') {
        push @format_elements, 'variablelist';
      } elsif ($element->{'cmdname'} eq 'itemize') {
        push @format_elements, 'itemizedlist';
        #push @attributes, " mark=\"\"";
      } elsif ($element->{'cmdname'} eq 'multitable') {
        push @format_elements, "informaltable";
        push @attributes, '';
        my $columns_count;
        if ($element->{'extra'} and defined($element->{'extra'}->{'max_columns'})) {
          $columns_count = $element->{'extra'}->{'max_columns'};
        } else {
          $columns_count = 0;
        }
        push @format_elements, 'tgroup';
        push @attributes, " cols=\"$columns_count\"";
        if ($element->{'extra'}) {
          my @fractions;
          my $multiply;
          if ($element->{'extra'}->{'prototypes'}) {
            $multiply = 1;
            foreach my $prototype (@{$element->{'extra'}->{'prototypes'}}) {
              my $prototype_text
               = Texinfo::Convert::Text::convert_to_text($prototype,
                  {Texinfo::Convert::Text::copy_options_for_convert_text($self, 1)});
              push @fractions,
                Texinfo::Convert::Unicode::string_width($prototype_text);
            }
          } elsif ($element->{'extra'}->{'columnfractions'}) {
            @fractions = @{$element->{'extra'}->{'columnfractions'}->{'extra'}->{'misc_args'}};
            $multiply = 100;
          }
          foreach my $fraction (@fractions) {
            $appended .= '<colspec colwidth="'.($fraction*$multiply)
                         .'*"></colspec>';
          }
        }
      } elsif ($element->{'cmdname'} eq 'float') {
        if ($element->{'extra'} and defined($element->{'extra'}->{'node_content'})) {
          my $normalized =
            Texinfo::Convert::NodeNameNormalization::normalize_node (
                   { 'contents' => $element->{'extra'}->{'node_content'} });
          $result .= "<anchor id=\"$normalized\"/>\n";
        }
      } elsif ($element->{'cmdname'} eq 'verbatim') {
        push @format_elements, 'screen';
      } elsif ($element->{'cmdname'} eq 'displaymath') {
        push @format_elements, 'informalequation';
        push @format_elements, 'mathphrase';
      } elsif ($element->{'cmdname'} eq 'quotation'
               or $element->{'cmdname'} eq 'smallquotation') {
        my $format_element;
        if ($element->{'extra'}) {
          if ($element->{'extra'}->{'authors'}) {
            foreach my $author (@{$element->{'extra'}->{'authors'}}) {
              if ($author->{'extra'} and $author->{'args'}->[0]->{'contents'}) {
                $appended .= '<attribution>'.$self->_convert(
                  {'contents' => $author->{'args'}->[0]->{'contents'}})
                           ."</attribution>\n";
              }
            }
          }
          if ($element->{'args'} and $element->{'args'}->[0]
              and $element->{'args'}->[0]->{'contents'}
              and @{$element->{'args'}->[0]->{'contents'}}) {
            my $quotation_arg_text
              = Texinfo::Convert::Text::convert_to_text(
                 $element->{'args'}->[0],
                 {Texinfo::Convert::Text::copy_options_for_convert_text($self)});
            if ($docbook_special_quotations{lc($quotation_arg_text)}) {
              $format_element = lc($quotation_arg_text);
            } else {
              $self->{'pending_prepend'}
                = $self->_convert($self->gdt('@b{{quotation_arg}:} ',
                              {'quotation_arg' =>
                    $element->{'args'}->[0]->{'contents'}}));
            }
          }
        }
        $format_element = 'blockquote' if (!defined($format_element));
        push @format_elements, $format_element;
      } elsif ($Texinfo::Commands::block_commands{$element->{'cmdname'}} eq 'format_raw') {
        return '' if (!$self->{'expanded_formats_hash'}->{$element->{'cmdname'}});
        # the context is here only for the command, so this is forgotten
        # once all the raw internal text has been formatted
        $self->{'document_context'}->[-1]->{'raw'} = 1;
      } elsif ($Texinfo::Commands::block_commands{$element->{'cmdname'}} eq 'raw'
               or $Texinfo::Commands::block_commands{$element->{'cmdname'}} eq 'menu') {
        return '';
      }
      foreach my $format_element (@format_elements) {
        my $attribute = shift @attributes;
        $attribute = '' if (!defined($attribute));
        $result .= "<$format_element${attribute}>";
        unshift @close_format_elements, $format_element;
      }
      $result .= $appended if (defined($appended));
    }
  }
  #warn " end of cmdname\n";


  if ($element->{'type'}) {
    #warn " have type $element->{'type'}\n";
    if (exists($docbook_preformatted_formats{$element->{'type'}})) {
      push @{$self->{'document_context'}->[-1]->{'preformatted_stack'}}, 
         $docbook_preformatted_formats{$element->{'type'}};
    }
    if (defined($type_elements{$element->{'type'}})) {
      $result .= "<$type_elements{$element->{'type'}}>";
    } elsif ($element->{'type'} eq 'preformatted') {
      $result .= "<$self->{'document_context'}->[-1]->{'preformatted_stack'}->[-1]>";
      $self->{'document_context'}->[-1]->{'in_preformatted'} = 1;
    } elsif ($element->{'type'} eq 'def_line') {
      $result .= "<synopsis>";
      $result .= $self->_index_entry($element);
      push @{$self->{'document_context'}}, {'monospace' => [1], 'upper_case' => [0]};
      $self->{'document_context'}->[-1]->{'inline'}++;
      if ($element->{'args'} and @{$element->{'args'}}
          and $element->{'args'}->[0]->{'contents'}) {
        my $main_command;
        if ($Texinfo::Common::def_aliases{$element->{'extra'}->{'def_command'}}) {
          $main_command = $Texinfo::Common::def_aliases{$element->{'extra'}->{'def_command'}};
        } else {
          $main_command = $element->{'extra'}->{'def_command'};
        }
        foreach my $arg (@{$element->{'args'}->[0]->{'contents'}}) {
          my $type = $arg->{'extra'}->{'def_role'};
          next if !$type and $arg->{'type'} eq 'spaces';

          my $content = $self->_convert($arg);
          if ($type eq 'spaces' or $type eq 'delimiter') {
            $result .= $content;
          } elsif ($type eq 'category') {
            $result .= "<phrase role=\"category\"><emphasis role=\"bold\">$content</emphasis>:</phrase>";
          } elsif ($type eq 'name') {
            $result .= "<$defcommand_name_type{$main_command}>$content</$defcommand_name_type{$main_command}>";
          } else {
            if (!defined($def_argument_types_docbook{$type})) {
              warn "BUG: no def_argument_types_docbook for $type";
              next;
            }
            foreach my $element_attribute (reverse (
                                   @{$def_argument_types_docbook{$type}})) {
              my ($element, $attribute_text) = _parse_attribute($element_attribute);
              $content = "<$element${attribute_text}>$content</$element>";
            }
            $result .= $content;
          }
        }
      }
      pop @{$self->{'document_context'}};
      $result .= "</synopsis>";
      $result .= "\n";
    }
  }

  if ($element->{'contents'}) {
    #warn " have contents $element->{'contents'}\n";
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
    if (defined($self->{'pending_prepend'})
        and Texinfo::Common::element_is_inline($element, 1)) {
      $result .= $self->{'pending_prepend'};
      delete $self->{'pending_prepend'};
    }
    #my $nr = -1;
    foreach my $content (@{$element->{'contents'}}) {
      #$nr++;
      #print STDERR "C$debug_element_nr[$nr] ".Texinfo::Common::debug_print_element_short($content)."\n";
      $result .= $self->_convert($content);
    }
    pop @{$self->{'document_context'}->[-1]->{'monospace'}}
      if ($in_code);
  }

  if ($element->{'type'}) {
    if (defined($type_elements{$element->{'type'}})) {
      $result .= "</$type_elements{$element->{'type'}}>";
    } elsif ($element->{'type'} eq 'preformatted') {
      $result .= "</$self->{'document_context'}->[-1]->{'preformatted_stack'}->[-1]>";
      delete $self->{'document_context'}->[-1]->{'in_preformatted'};
    }
  }
  $result = '{'.$result.'}' 
     if ($element->{'type'} and $element->{'type'} eq 'bracketed'
         and (!$element->{'parent'}->{'type'} or
              ($element->{'parent'}->{'type'} ne 'block_line_arg'
               and $element->{'parent'}->{'type'} ne 'line_arg')));
  foreach my $format_element (@close_format_elements) {
    $result .= "</$format_element>";
  }
  
  if ($element->{'cmdname'}
      and exists($Texinfo::Commands::block_commands{$element->{'cmdname'}})) {
    # a pending_prepend still there may happen if a quotation is empty.
    delete $self->{'pending_prepend'};
    #$result .= "</$element->{'cmdname'}>\n";
    if (!$self->{'document_context'}->[-1]->{'raw'}
        and exists($docbook_preformatted_formats{$element->{'cmdname'}})) {
      my $format = pop @{$self->{'document_context'}->[-1]->{'preformatted_stack'}};
      die "BUG $format ne $docbook_preformatted_formats{$element->{'cmdname'}}"
        if ($format ne $docbook_preformatted_formats{$element->{'cmdname'}});
    }
    if ($self->{'context_block_commands'}->{$element->{'cmdname'}}) {
      pop @{$self->{'document_context'}};
    }

  } elsif ($element->{'type'} and exists($docbook_preformatted_formats{$element->{'type'}})) {
    my $format = pop @{$self->{'document_context'}->[-1]->{'preformatted_stack'}};
    die "BUG $format ne $docbook_preformatted_formats{$element->{'type'}}"
      if ($format ne $docbook_preformatted_formats{$element->{'type'}});
  # close sectioning command
  } elsif ($element->{'cmdname'} and $element->{'cmdname'} ne 'node'
           and $Texinfo::Commands::root_commands{$element->{'cmdname'}}) {
    my $docbook_sectioning_element = $self->_docbook_section_element($element);
    if ($docbook_sectioning_element eq 'part'
        and not ($element->{'extra'}
                 and $element->{'extra'}->{'part_associated_section'})
        and !Texinfo::Common::is_content_empty($element)) {
      $result .= "</partintro>\n";
    }
    my $level_adjusted_cmdname
        = Texinfo::Structuring::section_level_adjusted_command_name($element);
    if (!($element->{'structure'}->{'section_childs'}
          and scalar(@{$element->{'structure'}->{'section_childs'}}))
        or $level_adjusted_cmdname eq 'top') {
      $result .= "</$docbook_sectioning_element>\n";
      pop @{$self->{'lang_stack'}};
      my $current = $element;
      while ($current->{'structure'}->{'section_up'}
             # the most up element is a virtual sectioning root element, this
             # condition avoids getting into it
             and $current->{'structure'}->{'section_up'}->{'cmdname'}
             and !$current->{'structure'}->{'section_next'}
             and Texinfo::Structuring::section_level_adjusted_command_name($current->{'structure'}->{'section_up'}) ne 'top') {
        $current = $current->{'structure'}->{'section_up'};
        $result .= '</'.$self->_docbook_section_element($current) .">\n";
        pop @{$self->{'lang_stack'}};
      }
    }
  } elsif ($element->{'type'} and $element->{'type'} eq 'before_node_section') {
    # ignore text before the first @node or sectioning command
    # as DocBook does not allow content not within some semantic
    # markup
    return '';
  }

  if ($element->{'cmdname'}
      and $Texinfo::Commands::root_commands{$element->{'cmdname'}}
      and defined($self->{'in_skipped_node_top'})
      and $self->{'in_skipped_node_top'} == 1) {
    return '';
  }

  #warn " returning $result\n";
  return $result;
}

# figure: mandatory title->use it with shortcaption?. Has a caption. 

1;

__END__
# Automatically generated from maintain/template.pod

=head1 NAME

Texinfo::Convert::DocBook - Convert Texinfo tree to DocBook

=head1 SYNOPSIS

  my $converter
    = Texinfo::Convert::DocBook->converter({'parser' => $parser});

  $converter->output($tree);
  $converter->convert($tree);
  $converter->convert_tree($tree);

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

Texinfo::Convert::DocBook converts a Texinfo tree to DocBook.

=head1 METHODS

=over

=item $converter = Texinfo::Convert::DocBook->converter($options)

Initialize converter from Texinfo to DocBook.

The I<$options> hash reference holds options for the converter.  In
this option hash reference a L<parser object|Texinfo::Parser>
may be associated with the I<parser> key.  The other options
are Texinfo customization options and a few other options that can
be passed to the converter. Most of the customization options are described in
the Texinfo manual.  Those customization options, when appropriate, override
the document content.  The parser should not be available directly anymore
after getting the associated information.

See L<Texinfo::Convert::Converter> for more information.

=item $converter->output($tree)

Convert a Texinfo tree I<$tree> and output the result in files as
described in the Texinfo manual.

=item $result = $converter->convert($tree)

Convert a Texinfo tree I<$tree> and return the resulting output.

=item $result = $converter->convert_tree($tree)

Convert a Texinfo tree portion I<$tree> and return the resulting
output.  This function does not try to output a full document but only
portions.  For a full document use C<convert>.

=back

=head1 AUTHOR

Patrice Dumas, E<lt>pertusus@free.frE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright 2010- Free Software Foundation, Inc.  See the source file for
all copyright years.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

=cut
