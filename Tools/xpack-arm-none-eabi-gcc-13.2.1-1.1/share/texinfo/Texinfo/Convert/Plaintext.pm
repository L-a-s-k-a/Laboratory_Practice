# Plaintext.pm: output tree as text with filling.
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
# Names of methods overriden in Texinfo::Convert::Info begin with format_.

package Texinfo::Convert::Plaintext;

use 5.00405;

# See comment at start of HTML.pm
use if $] >= 5.012, feature => qw(unicode_strings);

use if $] >= 5.014, re => '/a';

use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Texinfo::Commands;
use Texinfo::Common;
use Texinfo::Convert::Texinfo;
use Texinfo::Convert::Utils;
use Texinfo::Convert::Text;
use Texinfo::Convert::Utils;
use Texinfo::Convert::Converter;
use Texinfo::Convert::Paragraph;

require Exporter;
use vars qw($VERSION @ISA);
@ISA = qw(Texinfo::Convert::Converter);

# Some extra initialization for the first time this module is loaded.
# This could be done in a UNITCHECK block, introduced in Perl 5.10.
our $module_loaded = 0;
sub import {
  if (!$module_loaded) {
    Texinfo::XSLoader::override(
      "Texinfo::Convert::Plaintext::_process_text_internal",
      "Texinfo::MiscXS::process_text");
    $module_loaded = 1;
  }
  # The usual import method
  goto &Exporter::import;
}

$VERSION = '7.0.3';


# commands that are of use for formatting.
my %formatted_line_commands = %Texinfo::Common::formatted_line_commands;
my %formatted_nobrace_commands = %Texinfo::Common::formatted_nobrace_commands;
my %formattable_line_commands = %Texinfo::Common::formattable_line_commands;
my %brace_commands = %Texinfo::Commands::brace_commands;


my $NO_NUMBER_FOOTNOTE_SYMBOL = '*';

# documentlanguage is used through gdt().
my @informative_global_commands = ('paragraphindent', 'firstparagraphindent',
'frenchspacing', 'footnotestyle', 'documentlanguage', 'deftypefnnewline');

my %informative_commands;
foreach my $informative_command (@informative_global_commands) {
  $informative_commands{$informative_command} = 1;
}

# used to pass to Texinfo::Convert::Info
sub get_informative_global_commands($)
{
  my $self = shift;
  return @informative_global_commands;
}

my %brace_no_arg_commands;
foreach my $command (keys (%brace_commands)) {
  $brace_no_arg_commands{$command} = 1 
    if ($brace_commands{$command} eq 'noarg');
}
my %accent_commands = %Texinfo::Commands::accent_commands;
my %line_commands = %Texinfo::Commands::line_commands;
my %nobrace_commands = %Texinfo::Commands::nobrace_commands;
my %sectioning_heading_commands = %Texinfo::Commands::sectioning_heading_commands;
my %def_commands = %Texinfo::Commands::def_commands;
my %ref_commands = %Texinfo::Commands::ref_commands;
my %block_commands = %Texinfo::Commands::block_commands;
my %root_commands = %Texinfo::Commands::root_commands;
my %preformatted_commands = %Texinfo::Commands::preformatted_commands;
my %math_commands = %Texinfo::Commands::math_commands;
my %preformatted_code_commands = %Texinfo::Commands::preformatted_code_commands;
my %default_index_commands = %Texinfo::Commands::default_index_commands;
my %letter_no_arg_commands = %Texinfo::Commands::letter_no_arg_commands;

my %nobrace_symbol_text = %Texinfo::Common::nobrace_symbol_text;
my %explained_commands = %Texinfo::Common::explained_commands;
my %inline_format_commands = %Texinfo::Common::inline_format_commands;
my %brace_code_commands       = %Texinfo::Common::brace_code_commands;

my @contents_commands = ('contents', 'shortcontents', 'summarycontents');

foreach my $kept_command (keys (%informative_commands), @contents_commands,
  keys (%default_index_commands), keys(%formattable_line_commands)) {
  $formatted_line_commands{$kept_command} = 1;
}

# formatted/formattable @-commands not formatted in Plaintext/Info
foreach my $non_formatted_line_command ('page', 'need', 'vskip',
   'author', 'subtitle', 'title') {
  delete $formatted_line_commands{$non_formatted_line_command};
}

foreach my $def_command (keys(%def_commands)) {
  $formatted_line_commands{$def_command} = 1 if ($line_commands{$def_command});
}

# There are 6 stacks that define the context.
# context:   relevant for alignement of text.  Set in math, footnote, 
#            listoffloats, flush_commands, preformatted_context_commands 
#            (preformatted + menu + verbatim), and raw commands if 
#            on top level.
# format_context: used for the count of paragraphs and for the indentation.
#            Set in footnote, for all commands relevant for indenting, like
#            @*table, @itemize, @enumerate, preformatted commands, 
#            @*quotation, @def*, and also menu commands, @flushleft, 
#            @flushright, @float, in multitable cell and raw commands if at
#            toplevel.
# text_element_context: for the max columns and the counter in the line
#            position (although the counter in the line is taken over by 
#            the formatter once a formatter is opened).
#            Set in footnote and in multitable cells.
# formatters: a formatter environment has stacks for formatting context.
#            Also holds a container, an objects that does the counting 
#            of columns, actual indentation.  In general, it is better not 
#            to have formatters in parallel, but it may happen.
# count_context: holds the bytes count, the lines count and the location
#            of the commands that have their byte count or lines count
#            recorded.  It is set for out of document formatting to avoid
#            counting some converted text, but it is also set when it has
#            to be modified afterwards, for aligned commands or multitable
#            cells for example.
# document_context: Used to keep track if we are in a multitable.

# formatters have their own stack
# in container
# 'upper_case'
# 'var'
# 'font_type_stack'
# 
# paragraph number incremented with paragraphs, center, listoffloats
# and block commands except: html and such, group, raggedright, menu*, float

my %menu_commands;

my %format_raw_commands;
foreach my $block_command (keys(%block_commands)) {
  $menu_commands{$block_command} = 1
    if ($block_commands{$block_command} eq 'menu');
  $format_raw_commands{$block_command} = 1
    if ($block_commands{$block_command} eq 'format_raw');
}

my %default_preformatted_context_commands = (%preformatted_commands,
                                             %format_raw_commands);
foreach my $preformatted_command ('verbatim', keys(%menu_commands)) {
  $default_preformatted_context_commands{$preformatted_command} = 1;
}

my %block_math_commands;
foreach my $block_math_command (keys(%math_commands)) {
  if (exists($block_commands{$block_math_command})) {
    $block_math_commands{$block_math_command} = 1;
  }
}

my %ignored_line_commands;
foreach my $line_command (keys(%line_commands)) {
  $ignored_line_commands{$line_command} = 1
    unless ($formatted_line_commands{$line_command});
}

my %ignored_nobrace_commands;
foreach my $nobrace_command (keys(%nobrace_commands)) {
  $ignored_nobrace_commands{$nobrace_command} = 1
    unless ($formatted_nobrace_commands{$nobrace_command});
}

my %ignored_commands = (%ignored_line_commands, %ignored_nobrace_commands);
foreach my $ignored_brace_commands ('caption', 'shortcaption', 
  'hyphenation', 'sortas') {
  $ignored_commands{$ignored_brace_commands} = 1;
}

my %item_indent_format_length = ('enumerate' => 2,
    'itemize' => 3,
    'table' => 0,
    'vtable' => 0,
    'ftable' => 0,
 );

my $indent_length = 5;

my %indented_commands;
foreach my $indented_command (keys(%item_indent_format_length), 
           keys(%preformatted_commands), 'quotation', 'smallquotation', 
           'indentedblock', 'smallindentedblock',
           keys(%def_commands)) {
  $indented_commands{$indented_command} = 1 
    if exists($block_commands{$indented_command});
}

my %default_format_context_commands = %indented_commands;

foreach my $non_indented('format', 'smallformat') {
  delete $indented_commands{$non_indented};
}

# FIXME should keys(%math_brace_commands) be added here?
# How can this be tested?
foreach my $format_context_command (keys(%menu_commands), 'verbatim',
 'flushleft', 'flushright', 'multitable', 'float') {
  $default_format_context_commands{$format_context_command} = 1;
}

my %flush_commands = (
  'flushleft'  => 1,
  'flushright' => 1
);

# commands that leads to advancing the paragraph number.  This is mostly
# used to determine the first line, in fact.
my %advance_paragraph_count_commands;
foreach my $command (keys(%block_commands)) {
  next if ($menu_commands{$command}
            or $block_commands{$command} eq 'raw');
  $advance_paragraph_count_commands{$command} = 1;
}

# group and raggedright do more than not advancing para, they should also
# be transparent with respect to paragraph number counting.
foreach my $not_advancing_para ('group', 'raggedright',
  'titlepage', 'copying', 'documentdescription', 'float') {
  delete $advance_paragraph_count_commands{$not_advancing_para};
}

foreach my $advancing_para('center', 'verbatim', 'listoffloats') {
  $advance_paragraph_count_commands{$advancing_para} = 1;
}

foreach my $ignored_block_commands ('ignore', 'macro', 'rmacro', 'copying',
  'documentdescription', 'titlepage', 'direntry') {
  $ignored_commands{$ignored_block_commands} = 1;
}

my %punctuation_no_arg_commands;
foreach my $punctuation_command('enddots', 'exclamdown', 'questiondown') {
  $punctuation_no_arg_commands{$punctuation_command} = 1;
}

my %upper_case_commands = (
 'sc' => 1,
 'var' => 1
);

my %ignorable_space_types;
foreach my $type ('ignorable_spaces_after_command',
            'spaces_at_end',
            'spaces_before_paragraph',
            'spaces_after_close_brace') {
  $ignorable_space_types{$type} = 1;
}

my %ignored_types;
foreach my $type ('preamble_before_beginning',
            'preamble_before_setfilename') {
  $ignored_types{$type} = 1;
}

my %ignorable_types = %ignorable_space_types;
foreach my $ignored_type(keys(%ignored_types)) {
  $ignorable_types{$ignored_type} = 1;
}

# All those commands run with the text.
my %style_map = (
  'strong' => '*',
  'dfn'    => '"',
  'emph'   => '_',
);

foreach my $command (keys(%style_map)) {
  $style_map{$command} = [$style_map{$command}, $style_map{$command}];
}

# math is special
my @asis_commands = ('asis', 'w', 'b', 'i', 'sc', 't', 'r',
  'slanted', 'sansserif', 'var', 'verb', 'clicksequence',
  'headitemfont', 'dmn');

foreach my $asis_command (@asis_commands) {
  $style_map{$asis_command} = ['', ''];
}

my @quoted_commands = ('cite', 'code', 'command', 'env', 'file', 'kbd',
  'option', 'samp', 'indicateurl');

# %non_quoted_commands_when_nested have no quote when in code command contexts
my %non_quoted_commands_when_nested;

# Quotes are reset in converter_initialize and unicode quotes are used 
# if @documentencoding utf-8 is used.
foreach my $quoted_command (@quoted_commands) {
  $style_map{$quoted_command} = ["'", "'"];
  if ($brace_code_commands{$quoted_command}) {
    $non_quoted_commands_when_nested{$quoted_command} = 1;
  }
}
# always quoted even when nested
delete $non_quoted_commands_when_nested{'samp'};
delete $non_quoted_commands_when_nested{'indicateurl'};

$style_map{'key'} = ['<', '>'];
$style_map{'sub'} = ['_{', '}'];
$style_map{'sup'} = ['^{', '}'];

# Commands producing styles that are output in node names and index entries.
my %index_style_commands;
for my $index_style_command ('strong', 'emph', 'sub', 'sup', 'key') {
  $index_style_commands{$index_style_command} = 1;
}


# in those commands, there is no addition of double space after a dot.
# math is special
my %no_punctation_munging_commands;
foreach my $command ('var', 'cite', 'dmn', keys(%brace_code_commands)) {
  $no_punctation_munging_commands{$command} = 1;
}

my %defaults = (
  'ENABLE_ENCODING'      => 1,
  'ASCII_PUNCTUATION'    => 0,
  'FORMAT_MENU'          => 'nomenu',
  #'EXTENSION'            => 'info',
  'EXTENSION'            => 'txt',
  #'USE_SETFILENAME_EXTENSION' => 1,
  'INFO_SPECIAL_CHARS_WARNING' => undef,
  # different from the default, which is undef
  'OUTFILE'              => '-',
  'documentlanguage'     => undef,

  'converted_format'     => '',
  'USE_NODES'            => 1,
);

sub push_top_formatter($$)
{
  my ($self, $top_context) = @_;

  push @{$self->{'context'}}, $top_context;
  push @{$self->{'format_context'}}, {
                                     'cmdname' => '_top_format',
                                     'indent_level' => 0,
                                     'paragraph_count' => 0,
                                   };
  push @{$self->{'text_element_context'}}, {
                                     'max' => $self->{'fillcolumn'}
                                   };
  push @{$self->{'document_context'}}, {
                                     'in_multitable' => 0
                                   };

  # This is not really meant to be used, as contents should open 
  # their own formatters, however it happens that there is some text
  # outside any content that needs to be formatted, as @sp for example.
  push @{$self->{'formatters'}}, $self->new_formatter('line');
  $self->{'formatters'}->[-1]->{'_top_formatter'} = 1;
}

sub converter_defaults($$)
{
  return %defaults;
}

sub converter_initialize($)
{
  my $self = shift;

  $self->{'context'} = [];
  $self->{'format_context'} = [];
  $self->{'empty_lines_count'} = undef;
  push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0,
                                     'locations' => []};

  %{$self->{'ignored_types'}} = %ignored_types;
  %{$self->{'ignorable_space_types'}} = %ignorable_space_types;
  %{$self->{'ignored_commands'}} = %ignored_commands;
  # this is dynamic because raw formats may either be full commands if
  # isolated, or simple text if in a paragraph
  %{$self->{'format_context_commands'}} = %default_format_context_commands;
  %{$self->{'preformatted_context_commands'}} 
     = %default_preformatted_context_commands;
  $self->{'footnote_index'} = 0;
  $self->{'pending_footnotes'} = [];
  $self->{'index_entry_node_colon'} = {};
  $self->{'index_entries_no_node'} = {};

  foreach my $format (keys(%format_raw_commands)) {
    $self->{'ignored_commands'}->{$format} = 1 
       unless ($self->{'expanded_formats_hash'}->{$format});
  }

  %{$self->{'style_map'}} = %style_map;
  $self->{'convert_text_options'} 
      = {Texinfo::Convert::Text::copy_options_for_convert_text($self)};

  if ($self->get_conf('ASCII_PUNCTUATION')) {
    $self->{'convert_text_options'}->{'ascii_punctuation'} = 1;
    # cache to avoid calling get_conf
    $self->{'ascii_punctuation'} = 1;
  } else {
    $self->{'ascii_punctuation'} = 0;
  }

  if ($self->get_conf('ENABLE_ENCODING')
      and $self->get_conf('OUTPUT_ENCODING_NAME')
      and $self->get_conf('OUTPUT_ENCODING_NAME') eq 'utf-8') {
    # cache this to avoid redoing calls to get_conf
    $self->{'to_utf8'} = 1;
    if (!$self->{'ascii_punctuation'}) {
      foreach my $quoted_command (@quoted_commands) {
        # Directed single quotes
        $self->{'style_map'}->{$quoted_command} = ["\x{2018}", "\x{2019}"];
      }
      # Directed double quotes
      $self->{'style_map'}->{'dfn'} = ["\x{201C}", "\x{201D}"];
    }
  }
  if (defined($self->get_conf('OPEN_QUOTE_SYMBOL'))) {
    foreach my $quoted_command (@quoted_commands) {
      $self->{'style_map'}->{$quoted_command}->[0] 
       = $self->get_conf('OPEN_QUOTE_SYMBOL');
    }
  }
  if (defined($self->get_conf('CLOSE_QUOTE_SYMBOL'))) {
    foreach my $quoted_command (@quoted_commands) {
      $self->{'style_map'}->{$quoted_command}->[1] 
       = $self->get_conf('CLOSE_QUOTE_SYMBOL');
    }
  }
  if ($self->get_conf('FILLCOLUMN')) {
    $self->{'fillcolumn'} = $self->get_conf('FILLCOLUMN');
    # else it's already set via the defaults
  }

  if ($self->get_conf('INFO_SPECIAL_CHARS_QUOTE')) {
    $self->{'info_special_chars_quote'}
       = $self->get_conf('INFO_SPECIAL_CHARS_QUOTE');
    if (!defined($self->get_conf('INFO_SPECIAL_CHARS_WARNING'))) {
      $self->set_conf('INFO_SPECIAL_CHARS_WARNING', 0);
    }
  } else {
    $self->{'info_special_chars_quote'} = '';
    if (!defined($self->get_conf('INFO_SPECIAL_CHARS_WARNING'))) {
      $self->set_conf('INFO_SPECIAL_CHARS_WARNING', 1);
    }
  }
  if ($self->get_conf('INFO_SPECIAL_CHARS_WARNING')) {
    $self->{'info_special_chars_warning'}
       = $self->get_conf('INFO_SPECIAL_CHARS_WARNING');
  } else {
    $self->{'info_special_chars_warning'} = '';
  }

  # This needs to be here to take into account $self->{'fillcolumn'}.
  $self->push_top_formatter('_Root_context');
  # some caching to avoid calling get_conf
  if ($self->get_conf('OUTPUT_PERL_ENCODING')) {
    $self->{'output_perl_encoding'} = $self->get_conf('OUTPUT_PERL_ENCODING');
  } else {
    $self->{'output_perl_encoding'} = '';
  }
  $self->{'enable_encoding'} = $self->get_conf('ENABLE_ENCODING');
  $self->{'output_encoding_name'} = $self->get_conf('OUTPUT_ENCODING_NAME');
  $self->{'debug'} = $self->get_conf('DEBUG');

  return $self;
}

sub count_context_bug_message($$$)
{
  my ($self, $precision, $element) = @_;

  if (scalar(@{$self->{'count_context'}}) != 1) {
    my $element_text;
    if ($element) {
      $element_text
         = Texinfo::Structuring::root_or_external_element_cmd_texi($element);
    } else {
      $element_text = '';
    }
    $self->present_bug_message("Too much count_context ${precision}(".
      scalar(@{$self->{'count_context'}}). "): ". $element_text, $element);
    die;
  }
}

sub _convert_root_element($$)
{
  my ($self, $element) = @_;

  my $result = '';
  $result .= $self->_convert($element);
  $self->count_context_bug_message('', $element);
  $result .= $self->process_footnotes($element);
  $self->count_context_bug_message('footnotes ', $element);

  return $result;
}

sub convert($$)
{
  my ($self, $root) = @_;

  my $result = '';

  my $tree_units = Texinfo::Structuring::split_by_node($root);
  $self->{'seenmenus'} = {};
  $self->{'empty_lines_count'} = 1;
  $self->{'index_entries_line_location'} = {};
  if (!defined($tree_units)) {
    $result = $self->_convert($root);
    $self->count_context_bug_message('no element ');
    my $footnotes = $self->process_footnotes();
    $self->count_context_bug_message('no element footnotes ');
    $result .= $footnotes;
  } else {
    foreach my $node_root_element (@$tree_units) {
      my $node_text = _convert_root_element($self, $node_root_element);
      $result .= $node_text;
    }
  }

  return $result;
}

# the initialization of module specific state is not done in output()
# as output() is generic, so it needs to be done here
sub convert_tree($$)
{
  my ($self, $root) = @_;

  if (!defined($self->{'empty_lines_count'})) {
    # setting to 1 ensures that nothing is done, as there is
    # something done (a newline added) if equal to 0.
    $self->{'empty_lines_count'} = 1;
  }
  $self->{'seenmenus'} = {}
    if (!$self->{'seenmenus'});
  $self->{'index_entries_line_location'} = {}
    if (!$self->{'index_entries_line_location'});
  my $result;
  if ($root->{'type'} and $root->{'type'} eq 'unit') {
    $result = _convert_root_element($self, $root);
  } else {
    $result = $self->_convert($root);
  }
  return $result;
}

my $end_sentence = quotemeta('.?!');
my $after_punctuation = quotemeta('"\')]');

sub _protect_sentence_ends ($) {
  my $text = shift;
  # Avoid suppressing end of sentence, by inserting a control character
  # in front of the full stop.  The choice of BS for this is arbitrary.
  $text =~ s/(?<=[^\p{Upper}])
             (?=[$end_sentence][$after_punctuation]*(?:\s|$))
             /\x08/xg;

  # Also insert a control character at end of string, to protect a full stop 
  # that may follow later.

  #$text =~ s/(?<=[^\p{Upper}\s][$after_punctuation]*)$/\x08/;
  # Perl doesn't support "variable length lookbehind"

  $text = reverse $text;
  $text =~ s/^(?=[$after_punctuation]*(?:[^\p{Upper}\s]))/\x08/;
  $text = reverse $text;

  return $text;
}

sub _process_text_internal {
  my ($text) = @_;

  $text =~ s/---/\x{1F}/g;
  $text =~ s/--/-/g;
  $text =~ s/\x{1F}/--/g;
  $text =~ s/``/"/g;
  $text =~ s/\'\'/"/g;
  $text =~ s/`/'/g;

  return $text;
}

# Convert ``, '', `, ', ---, -- in $COMMAND->{'text'} to their output,
# possibly coverting to upper case as well.
sub _process_text($$$)
{
  my ($self, $command, $context) = @_;

  my $text = $command->{'text'};

  if ($context->{'upper_case'}
      or $self->{'formatters'}[-1]->{'var'}) {
    $text = _protect_sentence_ends($text);
    $text = uc($text);
  }

  if ($self->{'to_utf8'}
      and !$self->{'ascii_punctuation'}) {
    return Texinfo::Convert::Unicode::unicode_text($text, 
            $context->{'font_type_stack'}->[-1]->{'monospace'});
  } elsif (!$context->{'font_type_stack'}->[-1]->{'monospace'}) {
    return _process_text_internal($text);
  }
  return $text;
}

sub new_formatter($$;$)
{
  my ($self, $type, $conf) = @_;

  my $first_indent_length;
  if ($conf) {
    $first_indent_length = $conf->{'first_indent_length'};
    delete $conf->{'first_indent_length'};
  }
  
  my $container;
  my $container_conf = {
         'max'               => $self->{'text_element_context'}->[-1]->{'max'},
         'indent_level'      => $self->{'format_context'}->[-1]->{'indent_level'}, 
  };
  $container_conf->{'frenchspacing'} = 1 
    if ($self->{'conf'}->{'frenchspacing'} eq 'on');
    #if ($self->get_conf('frenchspacing') eq 'on');
    # access 'conf' hash directly for efficiency

  $container_conf->{'counter'} 
    = $self->{'text_element_context'}->[-1]->{'counter'}
      if (defined($self->{'text_element_context'}->[-1]->{'counter'}));
  $container_conf->{'DEBUG'} = 1 if ($self->{'debug'});
  if ($conf) {
    foreach my $key (keys(%$conf)) {
      $container_conf->{$key} = $conf->{$key};
    }
  }
  my $indent = $container_conf->{'indent_length'};
  $indent = $indent_length*$container_conf->{'indent_level'}
    if (!defined($indent));

  if ($first_indent_length) {
    $container_conf->{'indent_length'} = $first_indent_length;
    $container_conf->{'indent_length_next'} = $indent;
  } else {
    $container_conf->{'indent_length'} = $indent;
  }
    
  if ($type eq 'line') {
    $container_conf->{'max'} = 10000001;
    $container_conf->{'keep_end_lines'} = 1;
    $container_conf->{'no_final_newline'} = 1;
    $container_conf->{'add_final_space'} = 1;

    $container = Texinfo::Convert::Paragraph->new($container_conf);
  } elsif ($type eq 'paragraph') {
    $container = Texinfo::Convert::Paragraph->new($container_conf);
  } elsif ($type eq 'unfilled') {
    $container_conf->{'max'} = 10000000;
    $container_conf->{'ignore_columns'} = 1;
    $container_conf->{'keep_end_lines'} = 1;
    $container_conf->{'frenchspacing'} = 1;
    $container_conf->{'unfilled'} = 1;
    $container_conf->{'no_final_newline'} = 1;

    $container = Texinfo::Convert::Paragraph->new($container_conf);
  } else {
    die "Unknown container type $type\n";
  }

  if ($flush_commands{$self->{'context'}->[-1]}) {
    set_space_protection($container, undef, 1, 1);
  }

  my $formatter = {'container' => $container, 'upper_case' => 0,
                   'font_type_stack' => [{}],
                   'w' => 0, 'type' => $type,
              'frenchspacing_stack' => [$self->{'conf'}->{'frenchspacing'}],
              #'frenchspacing_stack' => [$self->get_conf('frenchspacing')],
              # access 'conf' hash directly for efficiency
              'suppress_styles' => undef,
              'no_added_eol' => undef};
  if ($conf) {
    foreach my $configuration ('suppress_styles', 'no_added_eol') {
      $formatter->{$configuration} = $conf->{$configuration};
    }
  }

  if ($type eq 'unfilled') {
    foreach my $context (reverse(@{$self->{'context'}})) {
      if ($menu_commands{$context}) {
        last;
      } elsif ($preformatted_code_commands{$context}
               or $format_raw_commands{$context}
               or $math_commands{$context}) {
        $formatter->{'font_type_stack'}->[-1]->{'monospace'} = 1;
        $formatter->{'font_type_stack'}->[-1]->{'code_command'} = 1 
          if ($preformatted_code_commands{$context}
              or $math_commands{$context});
        last;
      }
    }
  }
  return $formatter;
}

# intercept messages, in case some Texinfo is processed twice
sub converter_line_warn($$$$)
{
  my $self = shift;
  my $configuration_information = shift;
  my $text = shift;
  my $error_location_info = shift;

  if (!$self->{'silent'}) {
    $self->line_warn($configuration_information, $text, $error_location_info);
  }
}

sub converter_line_error($$$$)
{
  my $self = shift;
  my $configuration_information = shift;
  my $text = shift;
  my $error_location_info = shift;

  if (!$self->{'silent'}) {
    $self->line_error($configuration_information, $text, $error_location_info);
  }
}

sub convert_line($$;$)
{
  my ($self, $converted, $conf) = @_;
  my $formatter = $self->new_formatter('line', $conf);
  push @{$self->{'formatters'}}, $formatter;
  my $text = $self->_convert($converted);
  $text .= _count_added($self, $formatter->{'container'},
                Texinfo::Convert::Paragraph::end($formatter->{'container'}));
  pop @{$self->{'formatters'}};
  return $text;
}

sub _convert_unfilled($$;$)
{
  my ($self, $converted, $conf) = @_;
  my $formatter = $self->new_formatter('unfilled', $conf);
  $formatter->{'font_type_stack'}->[-1]->{'monospace'} = 1;
  push @{$self->{'formatters'}}, $formatter;
  my $result = $self->_convert($converted);
  $result .= _count_added($self, $formatter->{'container'},
                Texinfo::Convert::Paragraph::end($formatter->{'container'}));
  pop @{$self->{'formatters'}};
  return $result;
}

sub count_bytes($$) 
{
  my ($self, $string) = @_;

  return Texinfo::Common::count_bytes($self, $string, 
                                      $self->{'output_perl_encoding'});
}

sub add_text_to_count($$)
{
  my ($self, $text) = @_;
  if (!$self->{'count_context'}->[-1]->{'pending_text'}) {
    $self->{'count_context'}->[-1]->{'pending_text'} = '';
  }
  $self->{'count_context'}->[-1]->{'pending_text'} .= $text;
}

sub _add_lines_count($$)
{
  my ($self, $lines_count) = @_;
  $self->{'count_context'}->[-1]->{'lines'} += $lines_count;
}

# Update $SELF->{'count_context'}->[-1]->{'bytes'} by counting the text that
# hasn't been counted yet.  It is faster to count the text all together than
# piece by piece in add_text_to_count.
sub update_count_context($)
{
  my $self = shift;
  if ($self->{'count_context'}->[-1]->{'pending_text'}) {
    $self->{'count_context'}->[-1]->{'bytes'} +=
      Texinfo::Common::count_bytes($self,
        $self->{'count_context'}->[-1]->{'pending_text'},
        $self->{'output_perl_encoding'});
    $self->{'count_context'}->[-1]->{'pending_text'} = '';
  }
}

# Save the line and byte offset of $ELEMENT.
sub add_location($$)
{
  my ($self, $element) = @_;
  my $location = { 'lines' => $self->{'count_context'}->[-1]->{'lines'} };
  push @{$self->{'count_context'}->[-1]->{'locations'}}, $location;
  if (!($element->{'extra'} and $element->{'extra'}->{'index_entry'})) {
    update_count_context($self);
    $location->{'bytes'} = $self->{'count_context'}->[-1]->{'bytes'};
    $location->{'root'} = $element;
  } else {
    $location->{'index_entry'} = $element;
  }
  return $location;
}

sub add_image($$$$;$)
{
  my ($self, $element, $lines_count, $image_width, $no_align) = @_;

  push @{$self->{'count_context'}->[-1]->{'images'}}, {
    'lines' => $self->{'count_context'}->[-1]->{'lines'},
    'lines_count' => $lines_count,
    'image_width'  => $image_width,
    'no_align' =>  $no_align,
  };
}

sub _count_added($$$)
{
  my ($self, $container, $text) = @_;

  my $count_context = $self->{'count_context'}->[-1];
  $count_context->{'lines'}
    += Texinfo::Convert::Paragraph::end_line_count($container);

  if (!defined $count_context->{'pending_text'}) {
    $count_context->{'pending_text'} = '';
  }
  $count_context->{'pending_text'} .= $text;
  return $text;
}

sub _update_locations_counts($$)
{
  my ($self, $locations) = @_;

  update_count_context($self);
  foreach my $location (@$locations) {
    $location->{'bytes'} += $self->{'count_context'}->[-1]->{'bytes'}
       if (defined($location->{'bytes'}));
    $location->{'lines'} += $self->{'count_context'}->[-1]->{'lines'}
      if (defined($location->{'lines'}));
  }
}

sub _add_newline_if_needed($) {
  my $self = shift;
  if (defined($self->{'empty_lines_count'}) 
       and $self->{'empty_lines_count'} == 0) {
    add_text_to_count($self, "\n");
    _add_lines_count($self, 1);
    $self->{'empty_lines_count'} = 1;
    return "\n";
  }
  return '';
}

sub _open_code($)
{
  my $formatter = shift;

  if (!$formatter->{'font_type_stack'}->[-1]->{'monospace'}) {
    push @{$formatter->{'font_type_stack'}}, {'monospace' => 1};
  } else {
    $formatter->{'font_type_stack'}->[-1]->{'monospace'}++;
  }
  push @{$formatter->{'frenchspacing_stack'}}, 'on';
  set_space_protection($formatter->{'container'}, undef, undef, undef, 1);
}

sub _close_code($)
{
  my $formatter = shift;

  $formatter->{'font_type_stack'}->[-1]->{'monospace'}--;
  pop @{$formatter->{'font_type_stack'}}
    if !$formatter->{'font_type_stack'}->[-1]->{'monospace'};
  pop @{$formatter->{'frenchspacing_stack'}};
  my $frenchspacing = 0;
  $frenchspacing = 1 if ($formatter->{'frenchspacing_stack'}->[-1] eq 'on');
  set_space_protection($formatter->{'container'}, undef,
                       undef, undef, $frenchspacing);
}

my $footnote_indent = 3;
sub process_footnotes($;$)
{
  my ($self, $element) = @_;

  $element = undef if ($element and (not defined ($element->{'extra'})
                          or not defined $element->{'extra'}->{'unit_command'}));

  my $result = '';
  if (scalar(@{$self->{'pending_footnotes'}})) {
    $result .= _add_newline_if_needed($self);
    if ($self->get_conf('footnotestyle') eq 'end' or !defined($element)) {
      my $footnotes_header = "   ---------- Footnotes ----------\n\n";
      $result .= $footnotes_header;
      add_text_to_count($self, $footnotes_header);
      _add_lines_count($self, 2);
      $self->{'empty_lines_count'} = 1;
    } else {

      my $node_contents = [@{$element->{'extra'}->{'unit_command'}->{'extra'}->{'node_content'}},
                                     {'text' => '-Footnotes'}];
      my $footnotes_node = {
        'cmdname' => 'node',
        'structure' => {'node_up' => $element->{'extra'}->{'unit_command'}},
        'extra' => {'node_content' => $node_contents }
      };
      $result .= $self->format_node($footnotes_node);
      $self->{'current_node'} = $footnotes_node;
    }
    while (@{$self->{'pending_footnotes'}}) {
      my $footnote = shift (@{$self->{'pending_footnotes'}});

      # If nested within another footnote and footnotestyle is separate,
      # the element here will be the parent element and not the footnote
      # element, while the pxref will point to the name with the
      # footnote node taken into account.  Not really problematic as
      # nested footnotes are not right.
      if ($element) {
        my $node_contents = [@{$element->{'extra'}->{'unit_command'}->{'extra'}->{'node_content'}},
                    {'text' => "-Footnote-$footnote->{'number'}"}];
        $self->add_location({'cmdname' => 'anchor',
                        'extra' => {'node_content' => $node_contents }
                       });
      }
      # this pushes on 'context', 'formatters', 'format_context',
      # 'text_element_context' and 'document_context'
      $self->push_top_formatter('footnote');
      my $formatted_footnote_number;
      if ($self->get_conf('NUMBER_FOOTNOTES')) {
        $formatted_footnote_number = $footnote->{'number'};
      } else {
        $formatted_footnote_number = $NO_NUMBER_FOOTNOTE_SYMBOL;
      }
      my $footnote_text = ' ' x $footnote_indent 
               . "($formatted_footnote_number) ";
      $result .= $footnote_text;
      $self->{'text_element_context'}->[-1]->{'counter'} +=
         Texinfo::Convert::Unicode::string_width($footnote_text);
      add_text_to_count($self, $footnote_text);
      $self->{'empty_lines_count'} = 0;

      $result .= $self->_convert($footnote->{'root'}->{'args'}->[0]); 
      $result .= _add_newline_if_needed($self);
      
      my $old_context = pop @{$self->{'context'}};
      die if ($old_context ne 'footnote');
      pop @{$self->{'formatters'}};
      pop @{$self->{'format_context'}};
      pop @{$self->{'text_element_context'}};
      pop @{$self->{'document_context'}};
    }
  }
  $self->{'footnote_index'} = 0;

  return $result;
}

sub _compute_spaces_align_line($$$;$)
{
  my ($line_width, $max_column, $direction, $no_align) = @_;

  my $spaces_prepended;
  if ($line_width >= $max_column or $no_align) {
    $spaces_prepended = 0;
  } elsif ($direction eq 'center') {
    # if no int we may end up with floats...
    $spaces_prepended = int(($max_column -1 - $line_width) /2);
  } else {
    $spaces_prepended = ($max_column -1 - $line_width);
  }
  return $spaces_prepended;
}

sub _align_lines($$$$$$)
{
  my ($self, $text, $max_column, $direction, $locations, $images) = @_;

  my $result = '';

  my $updated_locations = {};
  if ($locations and @$locations) {
    foreach my $location (@$locations) {
      next unless (defined($location->{'bytes'}));
      push @{$updated_locations->{$location->{'lines'}}}, $location;
    }
  }
  my $images_marks = {};
  if ($images and @$images) {
    foreach my $image (@$images) {
      if ($image->{'lines_count'} > 1) {
        if (!$images_marks->{$image->{'lines'}}) {
          $images_marks->{$image->{'lines'}} = $image;
        }# else {
        # Happens in Info with the special construct as, in that 
        # case, there are no lines!  So no error...
        #  $self->present_bug_message("more than one image with lines on $image->{'lines'}");
        # in that case, the $image->{'lines'} is not in sync with the
        # lines count.  So the second image will be treated as simple text.
        #}
      }
    }
  }

  my $bytes_count = 0;
  my $delta_bytes = 0;
  my $line_index = 0;
  my $image;
  my $image_lines_count;
  my $image_prepended_spaces;
  foreach my $line (split /^/, $text) {
    my $line_bytes_begin = 0;
    my $line_bytes_end = 0;
    my $removed_line_bytes_end = 0;
    my $removed_line_bytes_begin = 0;

    my ($new_image, $new_image_prepended_spaces);
    if ($images_marks->{$line_index}) {
      $new_image = $images_marks->{$line_index};
      $image_lines_count = 0;
      $new_image_prepended_spaces
       = _compute_spaces_align_line($new_image->{'image_width'}, $max_column, 
                                    $direction, $new_image->{'no_align'});
      if (!defined($image)) {
        $image = $new_image;
        $image_prepended_spaces = $new_image_prepended_spaces;
        $new_image = undef;
      }
    }

    my $orig_line;
    if (!$image) {
      my $chomped = chomp($line);
      # for debugging.
      $orig_line = $line;
      $removed_line_bytes_end -= count_bytes($self, $chomped);
      $line =~ s/^(\s*)//;
      $removed_line_bytes_begin -= count_bytes($self, $1);
      $line =~ s/(\s*)$//;
      $removed_line_bytes_end -= count_bytes($self, $1);
      my $line_width = Texinfo::Convert::Unicode::string_width($line);
      if ($line_width == 0) {
        $result .= "\n";
        $line_bytes_end += count_bytes($self, "\n");
        $bytes_count += count_bytes($self, "\n");
      } else {
        my $spaces_prepended 
         = _compute_spaces_align_line($line_width, $max_column, $direction);
        $result .= ' ' x$spaces_prepended . $line ."\n";
        $line_bytes_begin += count_bytes($self, ' ' x$spaces_prepended);
        $line_bytes_end += count_bytes($self, "\n");
        $bytes_count += $line_bytes_begin + $line_bytes_end 
                        + count_bytes($self, $line);
      }
    } else {
      $image_lines_count++;
      my $prepended_spaces = $image_prepended_spaces;
      # adjust if there is something else that the image on the first or
      # last line.  The adjustment is approximate.
      if (($image_lines_count == 1 or $image_lines_count == $image->{'lines_count'})
          and Texinfo::Convert::Unicode::string_width($line) > $image->{'image_width'}) {
        $prepended_spaces 
         -= Texinfo::Convert::Unicode::string_width($line) - $image->{'image_width'};
        $prepended_spaces = 0 if ($prepended_spaces < 0);
      }
      $result .= ' ' x$prepended_spaces . $line;
      $line_bytes_begin += count_bytes($self, ' ' x$prepended_spaces);
      $bytes_count += $line_bytes_begin + count_bytes($self, $line);
      if ($new_image) {
        $image = $new_image;
        $image_prepended_spaces = $new_image_prepended_spaces;
      } elsif ($image_lines_count == $image->{'lines_count'}) {
        $image = undef;
        $image_lines_count = undef;
        $image_prepended_spaces = undef;
      }
    }

    if ($updated_locations->{$line_index}) {
      foreach my $location (@{$updated_locations->{$line_index}}) {
        $location->{'bytes'} += $line_bytes_begin + $removed_line_bytes_begin 
                                + $delta_bytes;
      }
    }
    $delta_bytes += $line_bytes_begin + $line_bytes_end 
             + $removed_line_bytes_begin + $removed_line_bytes_end;
    $line_index++;
  }
  return ($result, $bytes_count);
}

sub _align_environment($$$$)
{
  my ($self, $result, $max, $align) = @_;

  update_count_context($self);
  my $counts = pop @{$self->{'count_context'}};
  my $bytes_count;
  ($result, $bytes_count) = $self->_align_lines($result, $max,
                      $align, $counts->{'locations'}, $counts->{'images'});
  $self->_update_locations_counts($counts->{'locations'});
  $self->{'count_context'}->[-1]->{'bytes'} += $bytes_count;
  $self->{'count_context'}->[-1]->{'lines'} += $counts->{'lines'};
  push @{$self->{'count_context'}->[-1]->{'locations'}},
                       @{$counts->{'locations'}};
  return $result;
}

sub format_contents($$$)
{
  my ($self, $section_root, $contents_or_shortcontents) = @_;

  my $contents = 1 if ($contents_or_shortcontents eq 'contents');

  # no sections
  return ('', 0) if (!$section_root
                     or !$section_root->{'structure'}->{'section_childs'});
  my $root_level = $section_root->{'structure'}->{'section_childs'}->[0]
                                                     ->{'structure'}->{'section_level'};
  foreach my $top_section (@{$section_root->{'structure'}->{'section_childs'}}) {
    $root_level = $top_section->{'structure'}->{'section_level'}
      if ($top_section->{'structure'}->{'section_level'} < $root_level);
  }

  my $result = '';
  my $lines_count = 0;
  # This is done like that because the tree may not be well formed if
  # there is a @part after a @chapter for example.
  foreach my $top_section (@{$section_root->{'structure'}->{'section_childs'}}) {
    my $section = $top_section;
 SECTION:
    while ($section) {
      push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0};
      my $section_title_tree;
      if (defined($section->{'structure'}->{'section_number'})
          and ($self->get_conf('NUMBER_SECTIONS')
               or !defined($self->get_conf('NUMBER_SECTIONS')))) {
        if ($section->{'cmdname'} eq 'appendix'
            and $section->{'structure'}->{'section_level'} == 1) {
          $section_title_tree = $self->gdt('Appendix {number} {section_title}',
                           {'number' => {'text' => $section->{'structure'}->{'section_number'}},
                            'section_title'
                              => {'contents' => $section->{'args'}->[0]->{'contents'}}});
        } else {
          $section_title_tree = $self->gdt('{number} {section_title}',
                           {'number' => {'text' => $section->{'structure'}->{'section_number'}},
                            'section_title'
                              => {'contents' => $section->{'args'}->[0]->{'contents'}}});
        }
      } else {
        $section_title_tree = {'contents' => $section->{'args'}->[0]->{'contents'}};
      }
      my $section_title = $self->convert_line(
            {'contents' => [$section_title_tree],
             'type' => 'frenchspacing'});
      pop @{$self->{'count_context'}};
      my $text = $section_title;
      chomp ($text);
      $text .= "\n";
      my $repeat_count = 2 * ($section->{'structure'}->{'section_level'} - ($root_level+1));
      ($result .= (' ' x $repeat_count)) if $repeat_count > 0;
      $result .= $text;
      $lines_count++;
      if ($section->{'structure'}->{'section_childs'}
          and ($contents or $section->{'structure'}->{'section_level'} < $root_level+1)) {
        $section = $section->{'structure'}->{'section_childs'}->[0];
      } elsif ($section->{'structure'}->{'section_next'}) {
        last if ($section eq $top_section);
        $section = $section->{'structure'}->{'section_next'};
      } else {
        last if ($section eq $top_section);
        while ($section->{'structure'}->{'section_up'}) {
          $section = $section->{'structure'}->{'section_up'};
          last SECTION if ($section eq $top_section);
          if ($section->{'structure'}->{'section_next'}) {
            $section = $section->{'structure'}->{'section_next'};
            last;
          }
        }
      }
    }
  }
  return ($result, $lines_count);
}

sub _menu($$)
{
  my ($self, $menu_command) = @_;

  if ($menu_command->{'cmdname'} eq 'menu') {
    my $result = "* Menu:\n\n";
    add_text_to_count($self, $result);
    _add_lines_count($self, 2);
    if ($self->{'current_node'}) {
      $self->{'seenmenus'}->{$self->{'current_node'}} = 1;
    }
    return $result;
  } else {
    return '';
  }
}

sub format_printindex($$)
{
  my ($self, $printindex) = @_;
  return $self->process_printindex($printindex);
}

sub _normalize_top_node($)
{
  my $node_name = shift;
  return Texinfo::Common::normalize_top_node_name($node_name);
}

# convert and cache a node name.  $NODE is a node element.
sub node_line($$)
{
  my ($self, $node) = @_;
  $self->{'node_lines_text'} = {} if (!$self->{'node_lines_text'});
  if (!$self->{'node_lines_text'}->{$node}) {
    my $node_text = {'type' => '_code',
              'contents' => $node->{'extra'}->{'node_content'}};
    push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0};
    $self->{'node_lines_text'}->{$node}
      = {'text' => _normalize_top_node($self->convert_line($node_text,
                                                 {'suppress_styles' => 1,
                                                  'no_added_eol' => 1,}))};
    update_count_context($self);
    my $end_context = pop @{$self->{'count_context'}};
    $self->{'node_lines_text'}->{$node}->{'count'} 
      = $end_context->{'bytes'};
  }
  return ($self->{'node_lines_text'}->{$node}->{'text'},
          $self->{'node_lines_text'}->{$node}->{'count'});
}

my $index_length_to_node = 41;

sub process_printindex($$;$)
{
  my ($self, $printindex, $in_info) = @_;

  my $index_name;

  if ($printindex->{'extra'} and $printindex->{'extra'}->{'misc_args'}
      and defined($printindex->{'extra'}->{'misc_args'}->[0])) {
    $index_name = $printindex->{'extra'}->{'misc_args'}->[0];
  } else {
    return '';
  }

  # this is not redone for each index, only once
  my $index_names = $self->{'indices_information'};
  if (!defined($self->{'index_entries'}) and $index_names) {

    my $merged_index_entries 
      = Texinfo::Structuring::merge_indices($index_names);
    my $index_entries_sort_strings;
    ($self->{'index_entries'}, $index_entries_sort_strings)
      = Texinfo::Structuring::sort_indices($self, $self,
                                           $merged_index_entries);
  }
  if (!$self->{'index_entries'} or !$self->{'index_entries'}->{$index_name}
      or ! @{$self->{'index_entries'}->{$index_name}}) {
    return '';
  }

  # first determine the line numbers for the spacing of their formatting
  my %line_nrs;
  my %entry_nodes;
  my $max_index_line_nr_string_length = 0;
  my %ignored_entries;
  foreach my $entry (@{$self->{'index_entries'}->{$index_name}}) {
    # FIXME format in a way instead of ignoring
    if ($entry->{'entry_element'}->{'extra'}
         and ($entry->{'entry_element'}->{'extra'}->{'seeentry'}
              or $entry->{'entry_element'}->{'extra'}->{'seealso'})) {
      $ignored_entries{$entry} = 1;
      next;
    }
    my $line_nr;

    if ($self->{'index_entries_line_location'}
        and defined($self->{'index_entries_line_location'}->{$entry->{'entry_element'}})) {
      $line_nr = $self->{'index_entries_line_location'}->{$entry->{'entry_element'}}->{'lines'};
      # ignore index entries in special regions that haven't been seen
    } elsif ($entry->{'entry_region'}) {
      $ignored_entries{$entry} = 1;
      next;
    }

    my $node;
    # priority given to the location determined dynamically as the
    # index entry may be in footnote.
    if ($self->{'index_entries_line_location'}
        and $self->{'index_entries_line_location'}->{$entry->{'entry_element'}}
        and defined($self->{'index_entries_line_location'}->{$entry->{'entry_element'}}->{'node'})) {
      $node = $self->{'index_entries_line_location'}->{$entry->{'entry_element'}}->{'node'};
    } elsif (defined($entry->{'entry_node'})) {
      $node = $entry->{'entry_node'};
    }
    $entry_nodes{$entry} = $node;
    if (!defined($node)) {
      $line_nr = 0;
    } elsif($in_info) {
      $line_nr = 3 if (defined($line_nr) and $line_nr < 3);
      $line_nr = 4 if (!defined($line_nr));
    } else {
      $line_nr = 0 if (!defined($line_nr));
    }
    my $index_line_nr_string_length = 
      Texinfo::Convert::Unicode::string_width($line_nr);
    $max_index_line_nr_string_length = $index_line_nr_string_length 
     if ($max_index_line_nr_string_length < $index_line_nr_string_length);
    $line_nrs{$entry} = $line_nr;
  }

  return '' if (scalar(keys(%line_nrs)) == 0);

  my $result = '';
  $result .= _add_newline_if_needed($self);
  if ($in_info) {
    my $info_printindex_magic = "\x{00}\x{08}[index\x{00}\x{08}]\n";
    $result .= $info_printindex_magic;
    add_text_to_count($self, $info_printindex_magic);
    _add_lines_count($self, 1);
  }
  my $heading = "* Menu:\n\n";

  $result .= $heading;
  add_text_to_count($self, $heading);
  _add_lines_count($self, 2);

  # this is used to count entries that are the same
  my %entry_counts = ();

  # Use the same line formatter for all the index entries.  This is
  # slightly faster than making a new one for each entry.
  my $formatter = $self->new_formatter('line',
                                 {'indent' => 0, 'suppress_styles' => 1,
                                  'no_added_eol' => 1});
  push @{$self->{'formatters'}}, $formatter;

  foreach my $entry (@{$self->{'index_entries'}->{$index_name}}) {
    next if ($ignored_entries{$entry});
    my $entry_tree = {'contents' => $entry->{'entry_content'}};
    my $subentries_tree = $self->comma_index_subentries_tree($entry);
    if ($entry->{'in_code'}) {
      $entry_tree->{'type'} = '_code';
      $subentries_tree->{'type'} = '_code'
        if (defined($subentries_tree));
    } else {
      $entry_tree->{'type'} = 'frenchspacing';
       $subentries_tree->{'type'} = 'frenchspacing'
        if (defined($subentries_tree));
    }
    my $entry_text = '';

    $entry_text = $self->_convert($entry_tree);
    $entry_text .= $self->_convert($subentries_tree)
      if (defined($subentries_tree));

    $entry_text .= _count_added($self, $formatter->{'container'},
                  Texinfo::Convert::Paragraph::end($formatter->{'container'}));
    next if ($entry_text !~ /\S/);

    # No need for protection, the Info readers should find the last : on
    # the line.  : in the node following the index entry node should be
    # protected, however, as done below, such that : in the node are not
    # mistaken as being part of the index entry.
    if ($entry_text =~ /:/ and $self->get_conf('INDEX_SPECIAL_CHARS_WARNING')) {
      $self->converter_line_warn ($self,
        sprintf(__("Index entry in \@%s with : produces invalid Info: %s"),
                $entry->{'index_at_command'},
                Texinfo::Convert::Texinfo::convert_to_texinfo($entry_tree)),
                        $entry->{'entry_element'}->{'source_info'});
    }

    my $entry_nr = '';
    if (!defined($entry_counts{$entry_text})) {
      $entry_counts{$entry_text} = 0;
    } else {
      $entry_counts{$entry_text}++;
      $entry_nr = ' <'.$entry_counts{$entry_text}.'>';
      add_text_to_count($self, $entry_nr);
    }
    my $entry_line = "* $entry_text${entry_nr}: ";
    add_text_to_count($self, "* ".": ");
    
    my $line_width = Texinfo::Convert::Unicode::string_width($entry_line);
    my $entry_line_addition = '';
    if ($line_width < $index_length_to_node) {
      my $spaces = ' ' x ($index_length_to_node - $line_width);
      $entry_line_addition .= $spaces;
      add_text_to_count($self, $spaces);
    }
    my $node = $entry_nodes{$entry};

    if (!defined($node)) {
      # cache the transformation to text and byte counting, as
      # it is likeky that there is more than one such entry
      if (!$self->{'outside_of_any_node_text'}) {
        push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0};
        my $node_text = $self->gdt('(outside of any node)');
        $self->{'outside_of_any_node_text'}
          = {'text' => $self->convert_line($node_text)};
        update_count_context($self);
        my $end_context = pop @{$self->{'count_context'}};
        $self->{'outside_of_any_node_text'}->{'count'} 
          = $end_context->{'bytes'};
      }
      $entry_line_addition .= $self->{'outside_of_any_node_text'}->{'text'};
      $self->{'count_context'}->[-1]->{'bytes'} 
            += $self->{'outside_of_any_node_text'}->{'count'};
      # FIXME when outside of sectioning commands this message was already
      # done by the Parser.
      # Warn, only once.
      if (!$self->{'index_entries_no_node'}->{$entry}) {
        $self->converter_line_warn($self,
             sprintf(__("entry for index `%s' outside of any node"),
                        $index_name), $entry->{'entry_element'}->{'source_info'});
        $self->{'index_entries_no_node'}->{$entry} = 1;
      }
    } else {
      my ($node_line, $byte_count) = $self->node_line($node);
      $self->{'count_context'}->[-1]->{'bytes'} += $byte_count;
      # protect characters that need to be protected in menu node entry
      # after menu entry name and also :, as the Info readers
      # should consider text up to : to be part of the index entry.
      if ($node_line =~ /([,\t:]|\.\s)/) {
        my $warned_char = $1;
        if ($self->{'info_special_chars_warning'}) {
          # Warn only once
          if (! $self->{'index_entry_node_colon'}->{$node_line}) {
            $self->converter_line_warn($self, sprintf(__(
             "node name with index entries should not contain `%s'"),
                                            $warned_char),
                           $node->{'source_info'});
          }
          $self->{'index_entry_node_colon'}->{$node_line} = 1;
        }
        if ($self->{'info_special_chars_quote'}) {
          my $pre_quote = "\x{7f}";
          my $post_quote = $pre_quote;
          $self->{'count_context'}->[-1]->{'bytes'} += 2;
          $node_line = $pre_quote . $node_line . $post_quote;
        }
      }
      $entry_line_addition .= $node_line;
    }
    $entry_line_addition .= '.';
    add_text_to_count($self, '.');

    $entry_line .= $entry_line_addition;
    $result .= $entry_line;

    my $line_nr = $line_nrs{$entry};
    my $line_nr_spaces = sprintf("%${max_index_line_nr_string_length}d", $line_nr);
    my $line_part = "(line ${line_nr_spaces})";
    $line_width += Texinfo::Convert::Unicode::string_width($entry_line_addition);
    my $line_part_width = Texinfo::Convert::Unicode::string_width($line_part);
    if ($line_width + $line_part_width +1 > $self->{'fillcolumn'}) {
      $line_part = "\n" . ' ' x ($self->{'fillcolumn'} - $line_part_width) 
           . "$line_part\n";
      _add_lines_count($self, 1);
    } else { 
      $line_part 
        = ' ' x ($self->{'fillcolumn'} - $line_part_width - $line_width)
           . "$line_part\n";
    }
    _add_lines_count($self, 1);
    add_text_to_count($self, $line_part);
    $result .= $line_part;
  }
  pop @{$self->{'formatters'}};


  $result .= "\n"; 
  add_text_to_count($self, "\n");
  _add_lines_count($self, 1);
  
  return $result;
}


sub format_node($$)
{
  my $self = shift;
  my $node = shift;

  return '';
}

# no error in plaintext
sub format_error_outside_of_any_node($$)
{
  my $self = shift;
  my $element = shift;
}

sub _anchor($$)
{
  my $self = shift;
  my $anchor = shift;

  if (!($self->{'multiple_pass'} or $self->{'in_copying_header'})) {
    $self->add_location($anchor); 
    $self->format_error_outside_of_any_node($anchor);
  }
  return '';
}

my $listoffloat_entry_length = 41;
my $listoffloat_append = '...';

sub ensure_end_of_line($$)
{
  my ($self, $text) = @_;

  my $chomped = chomp ($text);
  if ($chomped) {
    $self->{'count_context'}->[-1]->{'bytes'} -= count_bytes($self, $chomped);
    $self->{'count_context'}->[-1]->{'lines'} -= 1;
  }
  $text .= "\n";
  $self->{'text_element_context'}->[-1]->{'counter'} = 0;
  add_text_to_count($self, "\n");
  _add_lines_count($self, 1);
  return $text;
}

sub image_formatted_text($$$$)
{
  my ($self, $element, $basefile, $text) = @_;

  my $result;
  if (defined($text)) {
    $result = $text;
  } elsif (defined($element->{'args'}->[3])
           and $element->{'args'}->[3]->{'contents'}
           and @{$element->{'args'}->[3]->{'contents'}}) {
    $result = '[' .Texinfo::Convert::Text::convert_to_text(
      {'contents' => $element->{'args'}->[3]->{'contents'}},
      $self->{'convert_text_options'}) .']';
  } else {
    $self->converter_line_warn($self, sprintf(__(
                    "could not find \@image file `%s.txt' nor alternate text"),
                             $basefile), $element->{'source_info'});
    $result = '['.$basefile.']';
  }
  return $result;
}

sub format_image($$)
{
  my ($self, $element) = @_;

  if (defined($element->{'args'}->[0])
      and $element->{'args'}->[0]->{'contents'}
      and @{$element->{'args'}->[0]->{'contents'}}) {
    my $basefile = Texinfo::Convert::Text::convert_to_text(
     {'contents' => $element->{'args'}->[0]->{'contents'}},
     {'code' => 1, %{$self->{'convert_text_options'}}});
    my ($text, $width) = $self->txt_image_text($element, $basefile);
    my $result = $self->image_formatted_text($element, $basefile, $text);
    my $lines_count = ($result =~ tr/\n/\n/);
    if (!defined($width)) {
      $width = Texinfo::Convert::Unicode::string_width($result);
    }
    # the last line is part of the image but do not have a new line,
    # so 1 is added to $lines_count to have the number of lines of
    # the image
    $self->add_image($element, $lines_count+1, $width);
    return ($result, $lines_count);
  }
  return ('', 0);
}

sub _get_form_feeds($)
{
  my $form_feeds = shift;
  $form_feeds =~ s/^[^\f]*//;
  $form_feeds =~ s/[^\f]$//;
  return $form_feeds;
}

sub _convert($$);

# Convert the Texinfo tree under $ROOT to plain text.
sub _convert($$)
{
  my ($self, $element) = @_;

  my $formatter = $self->{'formatters'}->[-1];

  if (($element->{'type'} and $self->{'ignored_types'}->{$element->{'type'}})
       or ($element->{'cmdname'}
            and ($self->{'ignored_commands'}->{$element->{'cmdname'}}
                 or ($brace_commands{$element->{'cmdname'}}
                     and $brace_commands{$element->{'cmdname'}} eq 'inline'
                     and $element->{'cmdname'} ne 'inlinefmtifelse'
                     and (($inline_format_commands{$element->{'cmdname'}}
                          and (!$element->{'extra'}->{'format'}
                               or !$self->{'expanded_formats_hash'}->{$element->{'extra'}->{'format'}}))
                         or (!$inline_format_commands{$element->{'cmdname'}}
                             and !defined($element->{'extra'}->{'expand_index'}))))))) {
    return '';
  }
  my $result = '';

  my $type = $element->{'type'};
  my $command = $element->{'cmdname'};

  # in ignorable spaces, keep only form feeds.
  if ($type and $self->{'ignorable_space_types'}->{$type}
      and ($type ne 'spaces_before_paragraph'
           or $self->get_conf('paragraphindent') ne 'asis')) {
    if ($type eq 'spaces_after_close_brace'
        and $element->{'text'} =~ /\f/) {
      # FIXME also in spaces_before_paragraph?  Does not seems to be
      # relevant to keep form feeds in other ignorable spaces.
      $result = _get_form_feeds($element->{'text'});
    }
    add_text_to_count($self, $result);
    return $result;
  }

  # First handle empty lines. This has to be done before the handling
  # of text below to be sure that an empty line is always processed
  # especially
  if ($type and ($type eq 'empty_line' 
                           or $type eq 'after_menu_description_line')) {
    delete $self->{'text_element_context'}->[-1]->{'counter'};
    $self->{'empty_lines_count'}++;
    if ($self->{'empty_lines_count'} <= 1
        or $self->{'preformatted_context_commands'}->{$self->{'context'}->[-1]}) {
      $result = "";
      if ($element->{'text'} =~ /\f/) {
        $result .= _get_form_feeds($element->{'text'});
        add_text_to_count($self, $result);
      }
      $result .= _count_added($self, $formatter->{'container'},
                add_text($formatter->{'container'}, "\n"));
      return $result;
    } else {
      return '';
    }
  }

  # process text
  if (defined($element->{'text'})) {
    if (!$type or $type ne 'untranslated') {
      if (!$formatter->{'_top_formatter'}) {
        if ($type and $type eq 'raw') {
          $result = _count_added($self, $formatter->{'container'},
                      add_next($formatter->{'container'}, $element->{'text'}));
        } else {
          my $text = _process_text($self, $element, $formatter);
          $result = _count_added($self, $formatter->{'container'},
                      add_text ($formatter->{'container'}, $text));
        }
        return $result;
      # the following is only possible if paragraphindent is set to asis
      } elsif ($type and $type eq 'spaces_before_paragraph') {
        add_text_to_count($self, $element->{'text'});
        return $element->{'text'};
      # ignore text outside of any format, but warn if ignored text not empty
      } elsif ($element->{'text'} =~ /\S/) {
        $self->present_bug_message("ignored text not empty `$element->{'text'}'", $element);
        return '';
      } else {
        # miscellaneous top-level whitespace - possibly after an @image
        return _count_added($self, $formatter->{'container'},
                  add_text($formatter->{'container'}, $element->{'text'}));
      }
    } else {
      my $tree = $self->gdt($element->{'text'});
      my $converted = _convert($self, $tree);
      return $converted;
    }
  }

  if ($element->{'extra'}) {
    # REMARK it is not that wise to rely on {'extra'}->{'missing_argument'}
    # being set, additional tests of $element->{'contents'}
    # being defined could be added, in addition to be more robust in case
    # {'extra'}->{'missing_argument'} is removed
    if ($element->{'extra'}->{'missing_argument'}
             and (!$element->{'contents'} or !@{$element->{'contents'}})) {
      return '';
    }
  }

  if ($element->{'extra'} and $element->{'extra'}->{'index_entry'}
      and !$self->{'multiple_pass'} and !$self->{'in_copying_header'}) {
    my $location = $self->add_location($element);
    # remove a 'lines' from $location if at the very end of a node
    # since it will lead to the next node otherwise.
    if ($command and $command =~ /index/) {
      my $following_not_empty;
      my @parents = @{$self->{'current_roots'}};
      my @parent_contents = @{$self->{'current_contents'}};
      while (@parents) {
        my $parent = pop @parents;
        my $parent_content = pop @parent_contents;
        if ($parent->{'type'} and $parent->{'type'} eq 'paragraph') {
          $following_not_empty = 1;
          last;
        }
        foreach my $following_content (@$parent_content) {
          unless (($following_content->{'type'} 
                and ($following_content->{'type'} eq 'empty_line'
                    or $ignorable_types{$following_content->{'type'}}))
              or ($following_content->{'cmdname'} 
                  and ($following_content->{'cmdname'} eq 'c'  
                       or $following_content->{'cmdname'} eq 'comment'))) {
            $following_not_empty = 1;
            last;
          }
        }
        last if $following_not_empty;
        if ($parent->{'cmdname'} and $root_commands{$parent->{'cmdname'}}) {
          last;
        }
      }
      if (! $following_not_empty) {
        $location->{'lines'}--;
      }
    }
    # this covers the special case for index entry not associated with a 
    # node but seen.  this will be an index entry in @copying, 
    # in @insertcopying.
    # This also covers the case of an index entry in a node added by a 
    # @footnote with footnotestyle separate.
    if ($self->{'current_node'}) {
      $location->{'node'} = $self->{'current_node'};
    }
    $self->{'index_entries_line_location'}->{$element} = $location;
  }

  my $cell;
  my $preformatted;
  if ($command) {
    my $unknown_command;
    if (defined($nobrace_symbol_text{$command})) {
      if ($command eq ':') {
        remove_end_sentence($formatter->{'container'});
        return '';
      } elsif ($command eq '*') {
        $result = _count_added($self, $formatter->{'container'},
                               add_pending_word($formatter->{'container'}));
        # added eol in some line oriented constructs, such as @node, menu
        # entry and therefore index entry would lead to end of line on
        # node pointers line, in tag table, or on menu, all being invalid.
        if ($formatter->{'no_added_eol'}) {
          $result .= _count_added($self, $formatter->{'container'},
                                 add_text($formatter->{'container'}, ' '));
        } else {
          $result .= _count_added($self, $formatter->{'container'},
                                 end_line($formatter->{'container'}));
        }
      } elsif ($command eq '.' or $command eq '?' or $command eq '!') {
        $result .= _count_added($self, $formatter->{'container'},
            add_next($formatter->{'container'}, $command));
        add_end_sentence($formatter->{'container'}, 1);
      } elsif ($command eq ' ' or $command eq "\n" or $command eq "\t") {
        $result .= _count_added($self, $formatter->{'container'}, 
            add_next($formatter->{'container'}, $nobrace_symbol_text{$command}));
      } else {
        $result .= _count_added($self, $formatter->{'container'}, 
            add_text($formatter->{'container'}, $nobrace_symbol_text{$command}));
      }
      return $result;
    } elsif ($command eq 'today') {
      my $today = $self->Texinfo::Convert::Utils::expand_today();
      unshift @{$self->{'current_contents'}->[-1]}, $today;
    } elsif (exists($brace_no_arg_commands{$command})) {
      my $text;
      
      if ($command eq 'dots' or $command eq 'enddots') {
        # Don't use Unicode ellipsis character.
        $text = '...';
      } else {
        $text = Texinfo::Convert::Text::brace_no_arg_command($element,
                                           $self->{'convert_text_options'});
      }

      # @AA{} should suppress an end sentence, @aa{} shouldn't.  This
      # is the case whether we are in @sc or not.
      if ($formatter->{'upper_case'}
          and $letter_no_arg_commands{$command}) {
        $text = _protect_sentence_ends($text);
        $text = uc($text);
      }

      if ($punctuation_no_arg_commands{$command}) {
        $result .= _count_added($self, $formatter->{'container'},
                    add_next($formatter->{'container'}, $text));
        add_end_sentence($formatter->{'container'}, 1);
      } elsif ($command eq 'tie') {
        $formatter->{'w'}++;
        set_space_protection($formatter->{'container'}, 1, undef)
          if ($formatter->{'w'} == 1);
        $result .= _count_added($self, $formatter->{'container'}, 
                       add_text($formatter->{'container'}, $text));
        $formatter->{'w'}--;
        set_space_protection($formatter->{'container'}, 0, undef)
          if ($formatter->{'w'} == 0);
      } else {
        $result .= _count_added($self, $formatter->{'container'}, 
                       add_text($formatter->{'container'}, $text));

        # This is to have @TeX{}, for example, not to prevent end sentences.
        if (!$letter_no_arg_commands{$command}) {
          allow_end_sentence($formatter->{'container'});
        }

        if ($command eq 'dots') {
          remove_end_sentence($formatter->{'container'});
        }
      }
      if ($formatter->{'var'} 
          or $formatter->{'font_type_stack'}->[-1]->{'monospace'}) {
        allow_end_sentence($formatter->{'container'});
      }
      return $result;
    # commands with braces
    } elsif ($accent_commands{$command}) {
      my $encoding;
      if ($self->{'enable_encoding'}) {
        $encoding = $self->{'output_encoding_name'};
      }
      my $sc;
      if ($formatter->{'upper_case'}) {
        $sc = 1;
      }
      my $accented_text 
         = Texinfo::Convert::Text::text_accents($element, $encoding, $sc);
      $result .= _count_added($self, $formatter->{'container'},
         add_text($formatter->{'container'}, $accented_text));

      my $accented_text_original;
      if ($formatter->{'upper_case'}) {
        $accented_text_original
         = Texinfo::Convert::Text::text_accents($element, $encoding);
      }

      if ($accented_text_original
            and $accented_text_original !~ /\p{Upper}/
          or $formatter->{'var'} 
          or $formatter->{'font_type_stack'}->[-1]->{'monospace'}) {
        allow_end_sentence($formatter->{'container'});
      }

      # in case the text added ends with punctuation.  
      # If the text is empty (likely because of an error) previous 
      # punctuation will be cancelled, we don't want that.
      remove_end_sentence($formatter->{'container'})
        if ($accented_text ne '');
      return $result;
    } elsif ($self->{'style_map'}->{$command} 
         or ($element->{'type'} and $element->{'type'} eq 'definfoenclose_command')) {
      if ($brace_code_commands{$command}) {
        if (!$formatter->{'font_type_stack'}->[-1]->{'monospace'}) {
          push @{$formatter->{'font_type_stack'}}, {'monospace' => 1};
        } else {
          $formatter->{'font_type_stack'}->[-1]->{'monospace'}++;
        }
      } elsif ($brace_commands{$command}
               and $brace_commands{$command} eq 'style_no_code') {
        if ($formatter->{'font_type_stack'}->[-1]->{'monospace'}) {
          push @{$formatter->{'font_type_stack'}}, {'monospace' => 0, 
                                                    'normal' => 1};
        } elsif ($formatter->{'font_type_stack'}->[-1]->{'normal'}) {
          $formatter->{'font_type_stack'}->[-1]->{'normal'}++;
        }
      }
      if ($no_punctation_munging_commands{$command}) {
        push @{$formatter->{'frenchspacing_stack'}}, 'on';
        set_space_protection($formatter->{'container'}, undef,
          undef,undef,1);
      }
      if ($upper_case_commands{$command}) {
        $formatter->{'upper_case'}++;
        $formatter->{'var'}++ if ($command eq 'var');
      }
      if ($command eq 'w') {
        $formatter->{'w'}++;
        set_space_protection($formatter->{'container'}, 1,undef)
          if ($formatter->{'w'} == 1);
      }
      my ($text_before, $text_after);
      if ($element->{'type'} and $element->{'type'} eq 'definfoenclose_command') {
        $text_before = $element->{'extra'}->{'begin'};
        $text_after = $element->{'extra'}->{'end'};
      } elsif ($non_quoted_commands_when_nested{$command} 
            and $formatter->{'font_type_stack'}->[-1]->{'code_command'}) {
        $text_before = '';
        $text_after = '';
      } elsif ($formatter->{'suppress_styles'}
               and !$index_style_commands{$command}) {
        $text_before = '';
        $text_after = '';
      } else {
        $text_before = $self->{'style_map'}->{$command}->[0];
        $text_after = $self->{'style_map'}->{$command}->[1];
      }
      # do this after determining $text_before/$text_after such that it
      # doesn't impact the current command, but only commands nested within
      if ($non_quoted_commands_when_nested{$command}) {
        $formatter->{'font_type_stack'}->[-1]->{'code_command'}++;
      }
      $result .= _count_added($self, $formatter->{'container'},
               add_next($formatter->{'container'}, $text_before, 1))
         if ($text_before ne '');
      if ($element->{'args'}) {
        $result .= _convert($self, $element->{'args'}->[0]);
        if ($command eq 'strong' 
             and $element->{'args'}->[0]->{'contents'}
             and scalar (@{$element->{'args'}->[0]->{'contents'}})
             and $element->{'args'}->[0]->{'contents'}->[0]->{'text'}
             and $element->{'args'}->[0]->{'contents'}->[0]->{'text'} =~ /^Note\s/i
             and $self->{'converted_format'}
             and $self->{'converted_format'} eq 'info') {
          $self->converter_line_warn($self, __(
    "\@strong{Note...} produces a spurious cross-reference in Info; reword to avoid that"), 
                           $element->{'source_info'});
        }
      }
      $result .= _count_added($self, $formatter->{'container'},
               add_next($formatter->{'container'}, $text_after, 1))
         if ($text_after ne '');
      if ($command eq 'w') {
        $formatter->{'w'}--;
        set_space_protection($formatter->{'container'},0,undef)
          if ($formatter->{'w'} == 0);
      }
      if ($brace_code_commands{$command}) {
        $formatter->{'font_type_stack'}->[-1]->{'monospace'}--;
        allow_end_sentence($formatter->{'container'});
        pop @{$formatter->{'font_type_stack'}}
          if !$formatter->{'font_type_stack'}->[-1]->{'monospace'};
      } elsif ($brace_commands{$command}
               and $brace_commands{$command} eq 'style_no_code') {
        if ($formatter->{'font_type_stack'}->[-1]->{'normal'}) {
          $formatter->{'font_type_stack'}->[-1]->{'normal'}--;
          pop @{$formatter->{'font_type_stack'}}
            if !$formatter->{'font_type_stack'}->[-1]->{'normal'};
        }
      }
      if ($non_quoted_commands_when_nested{$command}) {
        #$formatter->{'code_command'}--;
        $formatter->{'font_type_stack'}->[-1]->{'code_command'}--;
      }
      if ($no_punctation_munging_commands{$command}) {
        pop @{$formatter->{'frenchspacing_stack'}};
        my $frenchspacing = 0;
        $frenchspacing = 1 if ($formatter->{'frenchspacing_stack'}->[-1] eq 'on');
        set_space_protection($formatter->{'container'}, undef,
          undef, undef, $frenchspacing);
      }
      if ($upper_case_commands{$command}) {
        $formatter->{'upper_case'}--;
        if ($command eq 'var') {
          $formatter->{'var'}--;
          # Allow a following full stop to terminate a sentence.
          allow_end_sentence($formatter->{'container'});
        }
      }
      return $result;
    } elsif ($command eq 'image') {
      $result = _count_added($self, $formatter->{'container'},
                   add_pending_word($formatter->{'container'}, 1));
      # add an empty word so that following spaces aren't lost
      add_next($formatter->{'container'},'');
      my ($image, $lines_count) = $self->format_image($element);
      _add_lines_count($self, $lines_count);
      add_text_to_count($self, $image);
      if ($image ne '' and $formatter->{'type'} ne 'paragraph') {
        $self->{'empty_lines_count'} = 0;
      }
      $result .= $image; 
      return $result;
    } elsif ($command eq 'email') {
      # nothing is output for email, instead the command is substituted.
      my @email_contents;
      if ($element->{'args'}) {
        my $name;
        my $email;
        if (scalar (@{$element->{'args'}}) == 2
            and defined($element->{'args'}->[1])
            and $element->{'args'}->[1]->{'contents'}
            and @{$element->{'args'}->[1]->{'contents'}}) {
          $name = $element->{'args'}->[1]->{'contents'};
        }
        if (defined($element->{'args'}->[0])
            and $element->{'args'}->[0]->{'contents'}
            and @{$element->{'args'}->[0]->{'contents'}}) {
          $email = $element->{'args'}->[0]->{'contents'};
        }
        my $prepended;
        if ($name and $email) {
          $prepended = $self->gdt('{name} @url{{email}}', 
                           {'name' => $name, 'email' => $email});
        } elsif ($email) {
          $prepended = $self->gdt('@url{{email}}', 
                           {'email' => $email});
        } elsif ($name) {
          $prepended = {'contents' => $name};
        } else {
          return '';
        }
        unshift @{$self->{'current_contents'}->[-1]}, $prepended;
      }
      return '';
    } elsif ($command eq 'uref' or $command eq 'url') {
      if ($element->{'args'}) {
        if (scalar(@{$element->{'args'}}) == 3
             and defined($element->{'args'}->[2])
             and $element->{'args'}->[2]->{'contents'}
             and @{$element->{'args'}->[2]->{'contents'}}) {
          unshift @{$self->{'current_contents'}->[-1]}, 
            {'contents' => $element->{'args'}->[2]->{'contents'}};
        } elsif ($element->{'args'}->[0]->{'contents'}
                 and @{$element->{'args'}->[0]->{'contents'}}) {
          # no mangling of --- and similar in url.
          my $url = {'type' => '_code',
              'contents' => $element->{'args'}->[0]->{'contents'}};
          if (scalar(@{$element->{'args'}}) == 2
             and defined($element->{'args'}->[1])
             and $element->{'args'}->[1]->{'contents'}
             and @{$element->{'args'}->[1]->{'contents'}}) {
            my $prepended = $self->gdt('{text} ({url})', 
                 {'text' => $element->{'args'}->[1]->{'contents'},
                  'url' => $url });
            unshift @{$self->{'current_contents'}->[-1]}, $prepended;
          } else {
            my $prepended = $self->gdt('@t{<{url}>}', 
                                        {'url' => $url});
            unshift @{$self->{'current_contents'}->[-1]}, $prepended
          }
        } elsif (scalar(@{$element->{'args'}}) == 2
                 and defined($element->{'args'}->[1])
                 and $element->{'args'}->[1]->{'contents'}
                 and @{$element->{'args'}->[1]->{'contents'}}) {
          unshift @{$self->{'current_contents'}->[-1]}, 
            {'contents' => $element->{'args'}->[1]->{'contents'}};
        }
      }
      return '';
    } elsif ($command eq 'footnote') {
      $self->{'footnote_index'}++ unless ($self->{'multiple_pass'});
      my $formatted_footnote_number;
      if ($self->get_conf('NUMBER_FOOTNOTES')) {
        $formatted_footnote_number = $self->{'footnote_index'};
      } else {
        $formatted_footnote_number = $NO_NUMBER_FOOTNOTE_SYMBOL;
      }
      push @{$self->{'pending_footnotes'}}, {'root' => $element,
                                    'number' => $self->{'footnote_index'}}
          unless ($self->{'multiple_pass'});
      if (!$self->{'in_copying_header'}) {
        $self->format_error_outside_of_any_node($element);
      }
      $result .= _count_added($self, $formatter->{'container'},
           add_next($formatter->{'container'},
                    "($formatted_footnote_number)", 1));
      if ($self->get_conf('footnotestyle') eq 'separate' and $self->{'current_node'}) {
        $result .= _convert($self, {'contents' => 
         [{'text' => ' ('},
          {'cmdname' => 'pxref',
           'args' => [
             {'type' => 'brace_command_arg',
              'contents' => [
                 @{$self->{'current_node'}->{'extra'}->{'node_content'}},
                 {'text' => "-Footnote-$self->{'footnote_index'}"}
              ]
             }
           ]
          },
          {'text' => ')'}],
          });
      }
      return $result;
    } elsif ($command eq 'anchor') {
      $result = _count_added($self, $formatter->{'container'},
                   add_pending_word($formatter->{'container'}));
      $result .= $self->_anchor($element);
      return $result;
    } elsif ($ref_commands{$command}) {
      if (scalar(@{$element->{'args'}})) {
        my @args;
        for my $a (@{$element->{'args'}}) {
          if (defined $a->{'contents'} and @{$a->{'contents'}}) {
            push @args, $a->{'contents'};
          } else {
            push @args, undef;
          }
        }
        $args[0] = [{'text' => ''}] if (!defined($args[0]));

        # normalize node name, to get a ref with the right formatting
        # NOTE as a consequence, the line numbers appearing in case of errors
        # correspond to the node lines numbers, and not the @ref.
        my $node_content;
        if ($element->{'extra'}
            and $element->{'extra'}->{'label'}) {
          $node_content = $element->{'extra'}->{'label'}->{'extra'}->{'node_content'};
        } else { 
          $node_content = $args[0];
        }

        # if it a reference to a float with a label, $arg[1] is
        # set to '$type $number' or '$number' if there is no type.
        if (! defined($args[1]) 
            and $element->{'extra'}
            and $element->{'extra'}->{'label'}
            and $element->{'extra'}->{'label'}->{'cmdname'}
            and $element->{'extra'}->{'label'}->{'cmdname'} eq 'float') {
          my $float = $element->{'extra'}->{'label'};

          my $name = $self->float_type_number($float);
          $args[1] = $name->{'contents'};
        }
        if ($command eq 'inforef' and scalar(@args) == 3) {
          $args[3] = $args[2];
          $args[2] = undef;
        }

        # Treat cross-reference commands in a multitable cell as if they
        # were surrounded by @w{ ... }, so the output will not be split across
        # lines, leading text from other columns appearing to be part of the
        # cross-reference.
        my $in_multitable = 0;
        if ($self->{'document_context'}->[-1]->{'in_multitable'}) {
          $in_multitable = 1;
          $formatter->{'w'}++;
          set_space_protection($formatter->{'container'}, 1, undef)
            if ($formatter->{'w'} == 1);
        }
        # Disallow breaks in runs of Chinese text in node names, because a 
        # break would be normalized to a single space by the Info reader, and 
        # the node wouldn't be found.
        set_space_protection($formatter->{'container'},
                    undef, undef, undef, undef, 1);

        if ($command eq 'xref') {
          $result = _convert($self, {'contents' => [{'text' => '*Note '}]});
        } else {
          $result = _convert($self, {'contents' => [{'text' => '*note '}]});
        }
        my $name;
        if (defined($args[1])) {
          $name = $args[1];
        } elsif (defined($args[2])) {
          $name = $args[2];
        }
        my $file;
        if (defined($args[3])) {
          $file = [{'text' => '('},
                   {'type' => '_code',
                    'contents' => $args[3]},
                   {'text' => ')'},];
        } elsif (defined($args[4])) {
          # add a () such that the node is considered to be external, 
          # even though the manual name is not known.
          $file = [{'text' => '()'}];
        }
         
        if ($name) {
          my $name_text = _convert($self, {'contents' => $name});
          # needed, as last word is added only when : is added below
          my $name_text_checked = $name_text
             .get_pending($self->{'formatters'}->[-1]->{'container'});
          my $quoting_required = 0;
          if ($name_text_checked =~ /:/m) { 
            if ($self->{'info_special_chars_warning'}) {
              $self->converter_line_warn($self, sprintf(__(
                 "\@%s cross-reference name should not contain `:'"),
                                       $command), $element->{'source_info'});
            }
            if ($self->{'info_special_chars_quote'}) {
              $quoting_required = 1;
            }
          }
          my $pre_quote = $quoting_required ? "\x{7f}" : '';
          my $post_quote = $pre_quote;
          $name_text .= _convert($self, {'contents' => [
                {'text' => "$post_quote: "}]});
          $name_text =~ s/^(\s*)/$1$pre_quote/ if $pre_quote;
          $result .= $name_text;
          _count_added($self,$self->{'formatters'}[-1]{'container'},
                       $pre_quote)
            if $pre_quote;
        }

        if ($file) {
          $result .= _convert($self, {'contents' => $file});
        }

        my $node_line_name;
        # Get the node name to be output.
        # Due to the way the paragraph formatter holds pending text, converting
        # the node name with the current formatter does not yield all the
        # converted text.  To get the full node name (and no more), we
        # can convert in a new context, using convert_line.
        # However, it is slow to do this for every node.  So in the most
        # frequent case when the node name is a simple text element, use
        # that text instead.
        if (scalar(@{$node_content}) == 1
            and defined($node_content->[0]->{'text'})) {
          $node_line_name =  $node_content->[0]->{'text'};
        } else {
          $self->{'silent'} = 0 if (!defined($self->{'silent'}));
          $self->{'silent'}++;
          push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0};

          $node_line_name = $self->convert_line({'type' => '_code',
                                          'contents' => $node_content},
                                        {'suppress_styles' => 1,
                                          'no_added_eol' => 1});
          pop @{$self->{'count_context'}};
          $self->{'silent'}--;
        }

        my $check_chars;
        if ($name) {
          $check_chars = quotemeta ",\t.";
        } else {
          $check_chars = quotemeta ":";
        }

        my $quoting_required = 0;
        if ($node_line_name =~ /([$check_chars])/m) {
          if ($self->{'info_special_chars_warning'}) {
            $self->converter_line_warn($self, sprintf(__(
               "\@%s node name should not contain `%s'"), $command, $1),
                             $element->{'source_info'});
          }
          if ($self->{'info_special_chars_quote'}) {
            $quoting_required = 1;
          }
        }

        my $pre_quote = $quoting_required ? "\x{7f}" : '';
        my $post_quote = $pre_quote;

        # node name
        my $node_text = '';
        $node_text .= _count_added($self,
          $self->{'formatters'}[-1]{'container'},
          add_next($self->{'formatters'}->[-1]->{'container'}, $pre_quote))
               if $pre_quote;

        $self->{'formatters'}->[-1]->{'suppress_styles'} = 1;
        $node_text .= _convert($self, {'type' => '_code',
                                         'contents' => $node_content});
        delete $self->{'formatters'}->[-1]->{'suppress_styles'};

        $node_text .= _count_added($self,
          $self->{'formatters'}[-1]{'container'},
          add_next($self->{'formatters'}->[-1]->{'container'}, $post_quote))
               if $post_quote;

        if (!$name) {
          $node_text .= _count_added($self,
            $self->{'formatters'}[-1]{'container'},
            add_next($self->{'formatters'}->[-1]->{'container'}, '::'));
        }

        $result .= $node_text;

        # Check if punctuation follows the ref command.
        #
        # FIXME: is @xref really special here?  Original comment:
        # "If command is @xref, the punctuation must always follow the
        # command, for other commands it may be in the argument..."

        if ($name) {
          my $next = $self->{'current_contents'}->[-1]->[0];
          if (!($next and $next->{'text'} and $next->{'text'} =~ /^[\.,]/)) {
            if ($command eq 'xref') {
              if ($next and defined($next->{'text'}) and $next->{'text'} =~ /\S/) {
                my $text = $next->{'text'};
                $text =~ s/^\s*//;
                my $char = substr($text, 0, 1);
                $self->converter_line_warn($self, sprintf(__(
                            "`.' or `,' must follow \@xref, not %s"),
                                         $char), $element->{'source_info'});
              } else {
                $self->converter_line_warn($self,
                           __("`.' or `,' must follow \@xref"),
                                 $element->{'source_info'});
              }
            }
            my @added = ({'text' => '.'});
            # The added full stop does not end a sentence.  Info readers will
            # have a chance of guessing correctly whether the full stop was
            # added by whether it is followed by 2 spaces (although this
            # doesn't help at the end of a line).
            push @added, {'cmdname' => ':'};
            unshift @{$self->{'current_contents'}->[-1]}, @added;
          }
        }

        if ($in_multitable) {
          $formatter->{'w'}--;
          set_space_protection($formatter->{'container'}, 0, undef)
            if ($formatter->{'w'} == 0);
        }
        set_space_protection($formatter->{'container'},
          undef,undef,undef,undef,0); # double_width_no_break
        return $result;
      }
      return '';
    } elsif ($explained_commands{$command}) {
      if ($element->{'args'}
          and defined($element->{'args'}->[0])
          and $element->{'args'}->[0]->{'contents'}
          and @{$element->{'args'}->[0]->{'contents'}}) {
        # in abbr spaces never end a sentence.
        my $argument;
        if ($command eq 'abbr') {
          $argument = {'type' => 'frenchspacing',
                       'contents' => $element->{'args'}->[0]->{'contents'}};
        } else {
          $argument = { 'contents' => $element->{'args'}->[0]->{'contents'}};
        }
        if (scalar (@{$element->{'args'}}) == 2
            and defined($element->{'args'}->[-1])
            and $element->{'args'}->[-1]->{'contents'}
            and @{$element->{'args'}->[-1]->{'contents'}}) {
          my $prepended = $self->gdt('{abbr_or_acronym} ({explanation})', 
                           {'abbr_or_acronym' => $argument, 
                            'explanation' => $element->{'args'}->[-1]->{'contents'}});
          unshift @{$self->{'current_contents'}->[-1]}, $prepended;
          return '';
        } else {
          $result = _convert($self, $argument);

          # We want to permit an end of sentence, but not force it as @. does.
          allow_end_sentence($formatter->{'container'});
          return $result;
        }
      }
      return '';
    } elsif ($brace_commands{$command}
             and $brace_commands{$command} eq 'inline') {
      my $arg_index = 1;
      if ($command eq 'inlinefmtifelse'
          and (!$element->{'extra'}->{'format'}
               or !$self->{'expanded_formats_hash'}->{$element->{'extra'}->{'format'}})) {
        $arg_index = 2;
      }
      if (scalar(@{$element->{'args'}}) > $arg_index
         and defined($element->{'args'}->[$arg_index])
         and $element->{'args'}->[$arg_index]->{'contents'}
         and @{$element->{'args'}->[$arg_index]->{'contents'}}) {
        my $argument = {};
        if ($command eq 'inlineraw') {
          $argument->{'type'} = '_code';
        }
        $argument->{'contents'} = $element->{'args'}->[$arg_index]->{'contents'};
        unshift @{$self->{'current_contents'}->[-1]}, ($argument);
      }
      return '';
      # condition should actually be that the $command is inline
    } elsif ($math_commands{$command} and not exists($block_commands{$command})) {
      push @{$self->{'context'}}, $command;
      if ($element->{'args'}) {
        $result .= _convert($self, {'type' => 'frenchspacing',
             'contents' => [{'type' => '_code',
                            'contents' => [$element->{'args'}->[0]]}]});
      }
      my $old_context = pop @{$self->{'context'}};
      die if ($old_context ne $command);
      return $result;
    } elsif ($command eq 'titlefont') {
      push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0};
      $result = $self->convert_line ({'type' => 'frenchspacing',
               'contents' => [$element->{'args'}->[0]]});
      pop @{$self->{'count_context'}};
      $result = Texinfo::Convert::Text::text_heading(
                          {'structure' => {'section_level' => 0},
                           'cmdname' => 'titlefont'},
                            $result, $self, $self->get_conf('NUMBER_SECTIONS'),
          ($self->{'format_context'}->[-1]->{'indent_level'}) * $indent_length);
      $result =~ s/\n$//; # final newline has its own tree element
      $self->{'empty_lines_count'} = 0 unless ($result eq '');
      add_text_to_count($self, $result);
      _add_lines_count($self, 1);
      return $result;

    } elsif ($command eq 'U') {
      my $arg;
      if ($element->{'args'}
          and $element->{'args'}->[0]
          and $element->{'args'}->[0]->{'contents'}
          and $element->{'args'}->[0]->{'contents'}->[0]
          and $element->{'args'}->[0]->{'contents'}->[0]->{'text'}) {
        $arg = $element->{'args'}->[0]->{'contents'}->[0]->{'text'};
      }
      if ($arg) {
        # Syntactic checks on the value were already done in Parser.pm,
        # but we have one more thing to test: since this is the one
        # place where we might output actual UTF-8 binary bytes, we have
        # to check that it is possible.  If not, silently fall back to
        # plain text, on the theory that the user wants something.
        my $res;
        if ($self->{'to_utf8'}) {
          my $possible_conversion
            = Texinfo::Convert::Unicode::check_unicode_point_conversion($arg,
                                                             $self->{'DEBUG'});
          if ($possible_conversion) {
            $res = chr(hex($arg)); # ok to call chr
          } else {
            $res = "U+$arg";
          }
        } else {
          $res = "U+$arg";  # not outputting UTF-8
        }
        $result .= _count_added($self, $formatter->{'container'}, 
                   add_text($formatter->{'container'}, $res));
      } else {
        $result = '';  # arg was not defined
      }
      return $result;

    } elsif ($command eq 'value') {
      my $expansion = $self->gdt('@{No value for `{value}\'@}', 
                         {'value' => $element->{'extra'}->{'flag'}});
      if ($formatter->{'_top_formatter'}) {
        $expansion = {'type' => 'paragraph',
                      'contents' => [$expansion]};
      }
      $result .= _convert($self, $expansion);
      #  unshift @{$self->{'current_contents'}->[-1]}, $expansion;
      #return '';
      return $result;
    } elsif ($element->{'args'} and $element->{'args'}->[0]
             and $element->{'args'}->[0]->{'type'}
             and $element->{'args'}->[0]->{'type'} eq 'brace_command_arg') {
    # block commands
    } elsif (exists($block_commands{$command})) {
      # remark:
      # cartouche group and raggedright -> nothing on format stack

      if ($menu_commands{$command} and $self->get_conf('FORMAT_MENU') eq 'nomenu') {
        return '';
      }
      if ($self->{'preformatted_context_commands'}->{$command}
          or $command eq 'float') {
        if ($self->{'formatters'}->[-1]->{'type'} eq 'paragraph'
            and $format_raw_commands{$command}) {
          $result .= _count_added($self, $formatter->{'container'},
                              add_pending_word($formatter->{'container'}, 1));
          $result .= _count_added($self, $formatter->{'container'},
                              end_line($formatter->{'container'}));
        }
        push @{$self->{'context'}}, $command;
      } elsif ($flush_commands{$command}) {
        push @{$self->{'context'}}, $command;
      } elsif ($block_commands{$command} eq 'raw' # can only be @verbatim in practice
               or $block_math_commands{$command}) {
        if (!$self->{'formatters'}->[-1]->{'_top_formatter'}) {
          # reuse the current formatter if not in top level
          $result .= _count_added($self, $formatter->{'container'},
                              add_pending_word($formatter->{'container'}, 1));
          $result .= _count_added($self, $formatter->{'container'},
                              end_line($formatter->{'container'}));
        } else {
          # if in top level, the raw block command is turned into a 
          # simple preformatted command (alike @verbatim), to have a 
          # formatter container being created.
          push @{$self->{'context'}}, $command;
          $self->{'format_context_commands'}->{$command} = 1;
          $self->{'preformatted_context_commands'}->{$command} = 1;
        }
      }

      if ($self->{'format_context_commands'}->{$command}) {
        push @{$self->{'format_context'}}, 
             { 'cmdname' => $command,
               'paragraph_count' => 0,
               'indent_level' => 
                   $self->{'format_context'}->[-1]->{'indent_level'},
             };
        $self->{'format_context'}->[-1]->{'indent_level'}++
           if ($indented_commands{$command});
        # open a preformatted container, if the command opening the 
        # preformatted context is not a classical preformatted 
        # command (ie if it is menu or verbatim, and not example or  
        # similar)
        if ($self->{'preformatted_context_commands'}->{$command}
            and ! $preformatted_commands{$command}
            and ! $format_raw_commands{$command}) {
          $preformatted = $self->new_formatter('unfilled');
          push @{$self->{'formatters'}}, $preformatted;
        }
      }
      if ($command eq 'quotation'
          or $command eq 'smallquotation') {
        if ($element->{'args'} and $element->{'args'}->[0]
            and $element->{'args'}->[0]->{'contents'}
            and @{$element->{'args'}->[0]->{'contents'}}) {
          my $prepended = $self->gdt('@b{{quotation_arg}:} ', 
             {'quotation_arg' => $element->{'args'}->[0]->{'contents'}});
          $prepended->{'type'} = 'frenchspacing';
          $result .= $self->convert_line($prepended);
          $self->{'text_element_context'}->[-1]->{'counter'} +=
             Texinfo::Convert::Unicode::string_width($result);
          $self->{'empty_lines_count'} = 0 unless ($result eq '');
        }
      } elsif ($menu_commands{$command}) {
        $result .= $self->_menu($element);
      } elsif ($command eq 'multitable') {
        my $columnsize;
        if ($element->{'extra'}->{'columnfractions'}) {
          foreach my $fraction (@{$element->{'extra'}->{'columnfractions'}
                                       ->{'extra'}->{'misc_args'}}) {
            push @$columnsize, int($fraction * $self->{'text_element_context'}->[-1]->{'max'} +0.5);
          }
        } elsif ($element->{'extra'}->{'prototypes'}) {
          foreach my $prototype (@{$element->{'extra'}->{'prototypes'}}) {
            push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0};
            my ($formatted_prototype) = $self->convert_line($prototype, 
                                                        {'indent_length' => 0});
            pop @{$self->{'count_context'}};
            push @$columnsize, 
                 2+Texinfo::Convert::Unicode::string_width($formatted_prototype);
          }
        }
        $self->{'format_context'}->[-1]->{'columns_size'} = $columnsize;
        $self->{'format_context'}->[-1]->{'row_empty_lines_count'} 
          = $self->{'empty_lines_count'};
        $self->{'document_context'}->[-1]->{'in_multitable'}++;
      } elsif ($command eq 'float') {
        $result .= _add_newline_if_needed($self);
        if ($element->{'extra'} and $element->{'extra'}->{'node_content'}) {
          $result .= $self->_anchor($element);
        }
      }
    } elsif ($command eq 'node') {
      $self->{'current_node'} = $element;
      $result .= $self->format_node($element);
      $self->{'format_context'}->[-1]->{'paragraph_count'} = 0;
    } elsif ($sectioning_heading_commands{$command}) {
      # use settitle for empty @top
      # ignore @part
      my $contents;
      if ($element->{'args'}->[0]->{'contents'}
          and @{$element->{'args'}->[0]->{'contents'}}
          and $command ne 'part') {
        $contents = $element->{'args'}->[0]->{'contents'};
      } elsif ($command eq 'top'
          and $self->{'global_commands'}->{'settitle'}
          and $self->{'global_commands'}->{'settitle'}->{'args'}
          and @{$self->{'global_commands'}->{'settitle'}->{'args'}}
          and $self->{'global_commands'}->{'settitle'}->{'args'}->[0]->{'contents'}
          and @{$self->{'global_commands'}->{'settitle'}->{'args'}->[0]->{'contents'}}) {
        $contents =
           $self->{'global_commands'}->{'settitle'}->{'args'}->[0]->{'contents'};
      }
             
      if ($contents) {
        push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0};
        my $heading = $self->convert_line({'type' => 'frenchspacing',
                         'contents' => $contents});
        pop @{$self->{'count_context'}};
        # @* leads to an end of line, underlying appears on the line below
        # over one line
        my $heading_underlined = 
             Texinfo::Convert::Text::text_heading($element, $heading, $self,
                                             $self->get_conf('NUMBER_SECTIONS'),
                           ($self->{'format_context'}->[-1]->{'indent_level'})
                                           * $indent_length);
        $result .= _add_newline_if_needed($self);
        $self->{'empty_lines_count'} = 0 unless ($heading_underlined eq '');
        add_text_to_count($self, $heading_underlined);
        $result .= $heading_underlined;
        if ($heading_underlined ne '') {
          _add_lines_count($self, 2);
          $result .= _add_newline_if_needed($self);
        }
      }
      $self->{'format_context'}->[-1]->{'paragraph_count'} = 0;
    } elsif (($command eq 'item' or $command eq 'itemx')
            and $element->{'args'} and $element->{'args'}->[0]
            and $element->{'args'}->[0]->{'type'}
            and $element->{'args'}->[0]->{'type'} eq 'line_arg') {
      if ($element->{'args'} and @{$element->{'args'}}
          and $element->{'args'}->[0]->{'contents'}) {

        my $table_item_tree = $self->table_item_content_tree($element,
                                         $element->{'args'}->[0]->{'contents'});

        $table_item_tree->{'type'} = 'frenchspacing';
        $result = $self->convert_line($table_item_tree,
                    {'indent_level'
                      => $self->{'format_context'}->[-1]->{'indent_level'} -1});
        if ($result ne '') {
          $result = $self->ensure_end_of_line($result);
          $self->{'empty_lines_count'} = 0;
        }
      }
    } elsif ($command eq 'item' and $element->{'parent'}->{'cmdname'}
             and $block_commands{$element->{'parent'}->{'cmdname'}}
             and $block_commands{$element->{'parent'}->{'cmdname'}} eq 'item_container') {
      $self->{'format_context'}->[-1]->{'paragraph_count'} = 0;
      my $line = $self->new_formatter('line', 
          {'indent_length' => 
              ($self->{'format_context'}->[-1]->{'indent_level'} -1)
                * $indent_length
                 + $item_indent_format_length{$element->{'parent'}->{'cmdname'}}});
      push @{$self->{'formatters'}}, $line;
      if ($element->{'parent'}->{'cmdname'} eq 'enumerate') {
        $result = _count_added($self, $line->{'container'},
            add_next($line->{'container'},
               Texinfo::Common::enumerate_item_representation(
                 $element->{'parent'}->{'extra'}->{'enumerate_specification'},
                 $element->{'extra'}->{'item_number'}) . '. '));
      } elsif ($element->{'parent'}->{'args'}
          and $element->{'parent'}->{'args'}->[0]) {
        # this is the text prepended to items.
        
        $result = _convert($self, $element->{'parent'}->{'args'}->[0]);
        $result .= _convert($self, { 'text' => ' ' });
      }
      $result .= _count_added($self, $line->{'container'}, 
                      Texinfo::Convert::Paragraph::end($line->{'container'}));
      pop @{$self->{'formatters'}};
      $self->{'text_element_context'}->[-1]->{'counter'} +=
         Texinfo::Convert::Unicode::string_width($result);
      $self->{'empty_lines_count'} = 0 unless ($result eq '');
    # open a multitable cell
    } elsif ($command eq 'headitem' or $command eq 'item'
             or $command eq 'tab') {
      my $cell_width = $self->{'format_context'}->[-1]->{'columns_size'}->[$element->{'extra'}->{'cell_number'}-1];
      $self->{'format_context'}->[-1]->{'item_command'} = $command
        if ($command ne 'tab');
      die if (!defined($cell_width));
      $self->{'empty_lines_count'} 
         = $self->{'format_context'}->[-1]->{'row_empty_lines_count'};

      push @{$self->{'format_context'}},
           { 'cmdname' => $command,
             'paragraph_count' => 0,
             'indent_level' => 0 };
      push @{$self->{'text_element_context'}}, {'max' => $cell_width - 2 };
      push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0,
                                                   'locations' => []};
      $cell = 1;
    } elsif ($command eq 'center') {
      #my ($counts, $new_locations);
      push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0, 
                                                   'locations' => []};
      # $element->{'args'}->[0]->{'contents'} not set cannot happen
      # as in that case missing_argument would be set.  This condition
      # is therefre not really needed, but still put in case missing_argument
      # disappears
      my $result = '';
      if ($element->{'args'}->[0]
          and $element->{'args'}->[0]->{'contents'}) {
        $result = $self->convert_line (
                       {'type' => 'frenchspacing',
                        'contents' => $element->{'args'}->[0]->{'contents'}},
                       {'indent_length' => 0});
      }
      if ($result ne '') {
        $result = $self->ensure_end_of_line($result);

        $result = $self->_align_environment ($result, 
             $self->{'text_element_context'}->[-1]->{'max'}, 'center');
        $self->{'empty_lines_count'} = 0;
      } else {
        # it has to be done here, as it is done in _align_environment above
        pop @{$self->{'count_context'}};
      }
      $self->{'format_context'}->[-1]->{'paragraph_count'}++;
      return $result;
    } elsif ($command eq 'exdent') {
      $result = '';
      # $element->{'args'}->[0]->{'contents'} not set cannot happen
      # as in that case missing_argument would be set.  This condition
      # is therefre not really needed, but still put in case missing_argument
      # disappears
      if ($element->{'args'}->[0]
          and $element->{'args'}->[0]->{'contents'}) {
        if ($self->{'preformatted_context_commands'}->{$self->{'context'}->[-1]}) {
          $result = $self->_convert_unfilled(
               {'contents' => $element->{'args'}->[0]->{'contents'}},
               {'indent_level'
                 => $self->{'format_context'}->[-1]->{'indent_level'} -1});
        } else {
          $result = $self->convert_line(
             {'contents' => $element->{'args'}->[0]->{'contents'}},
             {'indent_level'
               => $self->{'format_context'}->[-1]->{'indent_level'} -1});
        }
      }
      if ($result ne '') {
        $result = $self->ensure_end_of_line($result);
        $self->{'empty_lines_count'} = 0;
      }
      return $result;
    } elsif ($command eq 'verbatiminclude') {
      my $expansion = Texinfo::Convert::Utils::expand_verbatiminclude($self,
                                                               $self, $element);
      unshift @{$self->{'current_contents'}->[-1]}, $expansion
        if ($expansion);
      return '';
    } elsif ($command eq 'insertcopying') {
      if ($self->{'global_commands'}
          and $self->{'global_commands'}->{'copying'}) {
        unshift @{$self->{'current_contents'}->[-1]}, 
         {'contents' => $self->{'global_commands'}->{'copying'}->{'contents'}};
      }
      return '';
    } elsif ($command eq 'printindex') {
      $result = $self->format_printindex($element);
      return $result;
    } elsif ($command eq 'listoffloats') {
      my $lines_count = 0;
      if ($element->{'extra'} and $element->{'extra'}->{'type'}
          and defined($element->{'extra'}->{'type'}->{'normalized'})
          and $self->{'floats'} 
          and $self->{'floats'}->{$element->{'extra'}->{'type'}->{'normalized'}}
          and @{$self->{'floats'}->{$element->{'extra'}->{'type'}->{'normalized'}}}) {
        push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0};
        if (!$self->{'empty_lines_count'}) {
          $result .= "\n";
          $lines_count++;
        }
        $result .= "* Menu:\n\n";
        $lines_count += 2;
        foreach my $float (@{$self->{'floats'}->{$element->{'extra'}->{'type'}->{'normalized'}}}) {
          next if !$float->{'args'} or !$float->{'args'}->[1]
                   or !$float->{'args'}->[1]->{'contents'}
                   or !@{$float->{'args'}->[1]->{'contents'}};
          my $float_label_text = $self->convert_line({'type' => '_code',
             'contents' => $float->{'args'}->[1]->{'contents'}});
          my $float_entry = $self->float_type_number($float);
          my $float_entry_text = ':';
          if (defined($float_entry)) {
            $float_entry->{'type'} = 'frenchspacing';
            $float_entry_text = $self->convert_line($float_entry);
          }
          # no translation here, this is required Info format.
          my $float_line = "* $float_entry_text: $float_label_text.";
          my $line_width 
             = Texinfo::Convert::Unicode::string_width($float_line);
          if ($line_width > $listoffloat_entry_length) {
            $float_line .= "\n" . ' ' x $listoffloat_entry_length;
            $lines_count++;
          } else {
            $float_line .= ' ' x ($listoffloat_entry_length - $line_width);
          }
          $line_width = $listoffloat_entry_length;
          my $caption;
          if ($float->{'extra'}->{'shortcaption'}) {
            $caption = $float->{'extra'}->{'shortcaption'};
          } elsif ($float->{'extra'}->{'caption'}) {
            $caption = $float->{'extra'}->{'caption'};
          }
          if ($caption and $caption->{'args'}->[0]
              and $caption->{'args'}->[0]->{'contents'}) {
            $self->{'multiple_pass'} = 1;
            push @{$self->{'context'}}, 'listoffloats';
            my $tree = {'contents' => $caption->{'args'}->[0]->{'contents'}};
            # the following does nothing since there are paragraphs within
            # the shortcaption.
            #if ($caption->{'cmdname'} eq 'shortcaption') {
            #  $tree->{'type'} = 'frenchspacing';
            #}
            my $caption_text = _convert($self, $tree);
            my $old_context = pop @{$self->{'context'}};
            delete $self->{'multiple_pass'};
            die if ($old_context ne 'listoffloats');
            while ($caption_text =~ s/^\s*(\p{Unicode::EastAsianWidth::InFullwidth}\s*|\S+\s*)//) {
              my $new_word = $1;
              $new_word =~ s/\n//g;
              if ((Texinfo::Convert::Unicode::string_width($new_word) +
                   $line_width) > 
                       ($self->{'text_element_context'}->[-1]->{'max'} - 3)) {
                $float_line .= $listoffloat_append;
                last;
              } else {
                $float_line .= $new_word;
                $line_width += 
                  Texinfo::Convert::Unicode::string_width($new_word);
              }
            }
          }
          $result .= $float_line. "\n";
          $lines_count++;
        }
        $result .= "\n";
        $lines_count++;
        $self->{'empty_lines_count'} = 1;
        pop @{$self->{'count_context'}};
      }
      $self->{'format_context'}->[-1]->{'paragraph_count'}++;
      add_text_to_count($self, $result);
      _add_lines_count($self, $lines_count);
      return $result;
    } elsif ($command eq 'sp') {
      if ($element->{'extra'}->{'misc_args'}->[0]) {
        $result = _count_added($self, $formatter->{'container'},
                              add_pending_word($formatter->{'container'}));
        # this useless copy avoids perl changing the type to integer!
        my $sp_nr = $element->{'extra'}->{'misc_args'}->[0];
        for (my $i = 0; $i < $sp_nr; $i++) {
          $result .= _count_added($self, $formatter->{'container'},
                end_line($formatter->{'container'}));
        }
        
        $self->{'empty_lines_count'} += $sp_nr;
        delete $self->{'text_element_context'}->[-1]->{'counter'};
      }
      return $result;
    } elsif ($command eq 'contents') {
      if ($self->{'structuring'}
            and $self->{'structuring'}->{'sectioning_root'}) {
        my $lines_count;
        ($result, $lines_count) 
            = $self->format_contents($self->{'structuring'}->{'sectioning_root'}, 
                              'contents');
        _add_lines_count($self, $lines_count);
        add_text_to_count($self, $result);
      }
      return $result;
    } elsif ($command eq 'shortcontents' 
               or $command eq 'summarycontents') {
      if ($self->{'structuring'}
            and $self->{'structuring'}->{'sectioning_root'}) {
        my $lines_count;
        ($result, $lines_count) 
              = $self->format_contents($self->{'structuring'}->{'sectioning_root'}, 
                              'shortcontents');
        _add_lines_count($self, $lines_count);
        add_text_to_count($self, $result);
      }
      return $result;
    # all the @-commands that have an information for the formatting, like
    # @paragraphindent, @frenchspacing...
    } elsif ($informative_commands{$command}) {
      Texinfo::Common::set_informative_command_value($self, $element);
      return '';
    } else {
      $unknown_command = 1;
    }
    if ($unknown_command
        and !($element->{'extra'}
                and ($element->{'extra'}->{'index_entry'}
                     or $element->{'extra'}->{'seeentry'}
                     or $element->{'extra'}->{'seealso'}))
        # commands like def*x are not processed above, since only the def_line
        # associated is processed. If they have no name and no category they 
        # are not considered as index entries either so they have a specific
        # condition
        and !($def_commands{$command} 
              and $command =~ /x$/)) {
      warn "Unhandled $command\n";
      $result .= "!!!!!!!!! Unhandled $command !!!!!!!!!\n";
    }
  }

  # open 'type' constructs.
  my $paragraph;
  if ($element->{'type'}) {
    if ($element->{'type'} eq 'paragraph') {
      $self->{'empty_lines_count'} = 0;
      my $conf = {};
      # indent. Not first paragraph.
      if ($self->{'format_context'}->[-1]->{'cmdname'} eq '_top_format'
          and $self->get_conf('paragraphindent') ne 'asis' 
          and $self->get_conf('paragraphindent')
          and (($element->{'extra'} and $element->{'extra'}->{'indent'})
             or (!($element->{'extra'} and $element->{'extra'}->{'noindent'})
                and ($self->{'format_context'}->[-1]->{'paragraph_count'} 
                  or $self->get_conf('firstparagraphindent') eq 'insert') 
               and !$self->{'text_element_context'}->[-1]->{'counter'}))) {
        $conf->{'first_indent_length'} = $self->get_conf('paragraphindent');
        $conf->{'first_indent_length'} = 0
          if ($conf->{'first_indent_length'} eq 'none');
      }
      $paragraph = $self->new_formatter('paragraph', $conf);
      push @{$self->{'formatters'}}, $paragraph;
      $self->{'format_context'}->[-1]->{'paragraph_count'}++;
      if ($self->{'context'}->[-1] eq 'flushright') {
        push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0,
                                                   'locations' => []};
      }
    } elsif ($element->{'type'} eq 'preformatted'
             or $element->{'type'} eq 'rawpreformatted') {
      # if in a description reuse the main menu unfilled, to keep things
      # simpler and avoid having to do a separate count.
      if ($element->{'type'} eq 'rawpreformatted'
          or !$element->{'parent'}->{'type'}
          or $element->{'parent'}->{'type'} ne 'menu_entry_description') {
        $preformatted = $self->new_formatter('unfilled');
        push @{$self->{'formatters'}}, $preformatted;
        if ($self->{'context'}->[-1] eq 'flushright') {
          push @{$self->{'count_context'}}, {'lines' => 0, 'bytes' => 0,
                                                     'locations' => []};
        }
      }
    } elsif ($element->{'type'} eq 'def_line') {
      if ($element->{'extra'} and $element->{'extra'}->{'def_parsed_hash'}
             and %{$element->{'extra'}->{'def_parsed_hash'}}) {
        my $arguments = Texinfo::Convert::Utils::definition_arguments_content($element);
        my $tree;
        my $command;
        if ($Texinfo::Common::def_aliases{$element->{'extra'}->{'def_command'}}) {
          $command = $Texinfo::Common::def_aliases{$element->{'extra'}->{'def_command'}};
        } else {
          $command = $element->{'extra'}->{'def_command'};
        }
        my $name;
        if ($element->{'extra'}->{'def_parsed_hash'}->{'name'}) {
          $name = $element->{'extra'}->{'def_parsed_hash'}->{'name'};
        } else {
          $name = '';
        }

        my $omit_def_space = $element->{'extra'}->{'omit_def_name_space'};
        
        if ($command eq 'deffn'
            or $command eq 'defvr'
            or $command eq 'deftp'
            or (($command eq 'deftypefn'
                 or $command eq 'deftypevr')
                and !$element->{'extra'}->{'def_parsed_hash'}->{'type'})) {
          if ($arguments) {
            my $strings = {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'name' => $name,
                    'arguments' => $arguments};
            if ($omit_def_space) {
              $tree = $self->gdt('@tie{}-- {category}: {name}{arguments}', $strings);
            } else {
              $tree = $self->gdt('@tie{}-- {category}: {name} {arguments}', $strings);
            }
          } else {
            $tree = $self->gdt('@tie{}-- {category}: {name}', {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'name' => $name});
          }
        } elsif ($command eq 'deftypefn'
                 or $command eq 'deftypevr') {
          if ($arguments) {
            my $strings = {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'name' => $name,
                    'type' => $element->{'extra'}->{'def_parsed_hash'}->{'type'},
                    'arguments' => $arguments};
            if ($self->get_conf('deftypefnnewline') eq 'on'
                and $command eq 'deftypefn') {
              if ($omit_def_space) {
                $tree = $self->gdt('@tie{}-- {category}:@*{type}@*{name}{arguments}',
                                   $strings);
              } else {
                $tree = $self->gdt('@tie{}-- {category}:@*{type}@*{name} {arguments}',
                                   $strings);
              }
            } else {
              if ($omit_def_space) {
                $tree = $self->gdt('@tie{}-- {category}: {type} {name}{arguments}',
                                   $strings);
              } else {
                $tree = $self->gdt('@tie{}-- {category}: {type} {name} {arguments}',
                                   $strings);
              }
            }
          } else {
            my $strings = {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'type' => $element->{'extra'}->{'def_parsed_hash'}->{'type'},
                    'name' => $name};
            if ($self->get_conf('deftypefnnewline') eq 'on'
                and $command eq 'deftypefn') {
              $tree = $self->gdt('@tie{}-- {category}:@*{type}@*{name}',
                                 $strings);
            } else {
              $tree = $self->gdt('@tie{}-- {category}: {type} {name}',
                                 $strings);
            }
          }
        } elsif ($command eq 'defcv'
                 or ($command eq 'deftypecv'
                     and !$element->{'extra'}->{'def_parsed_hash'}->{'type'})) {
          if ($arguments) {
            my $strings = {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'name' => $name,
                    'class' => $element->{'extra'}->{'def_parsed_hash'}->{'class'},
                    'arguments' => $arguments};
            if ($omit_def_space) {
              $tree = $self->gdt('@tie{}-- {category} of {class}: {name}{arguments}',
                                 $strings);
            } else {
              $tree = $self->gdt('@tie{}-- {category} of {class}: {name} {arguments}',
                                 $strings);
            }
          } else {
            $tree = $self->gdt('@tie{}-- {category} of {class}: {name}', {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'class' => $element->{'extra'}->{'def_parsed_hash'}->{'class'},
                    'name' => $name});
          }
        } elsif ($command eq 'defop'
                 or ($command eq 'deftypeop'
                     and !$element->{'extra'}->{'def_parsed_hash'}->{'type'})) {
          if ($arguments) {
            my $strings = {
                   'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'name' => $name,
                    'class' => $element->{'extra'}->{'def_parsed_hash'}->{'class'},
                    'arguments' => $arguments};
            if ($omit_def_space) {
              $tree = $self->gdt('@tie{}-- {category} on {class}: {name}{arguments}',
                                 $strings);
            } else {
              $tree = $self->gdt('@tie{}-- {category} on {class}: {name} {arguments}',
                                 $strings);
            }
          } else {
            $tree = $self->gdt('@tie{}-- {category} on {class}: {name}', {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'class' => $element->{'extra'}->{'def_parsed_hash'}->{'class'},
                    'name' => $name});
          }
        } elsif ($command eq 'deftypeop') {
          if ($arguments) {
            my $strings = {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'name' => $name,
                    'class' => $element->{'extra'}->{'def_parsed_hash'}->{'class'},
                    'type' => $element->{'extra'}->{'def_parsed_hash'}->{'type'},
                    'arguments' => $arguments};
            if ($self->get_conf('deftypefnnewline') eq 'on') {
              if ($omit_def_space) {
                $tree
                  = $self->gdt('@tie{}-- {category} on {class}:@*{type}@*{name}{arguments}',
                               $strings);
              } else {
                $tree
                  = $self->gdt('@tie{}-- {category} on {class}:@*{type}@*{name} {arguments}',
                               $strings);
              }
            } else {
              if ($omit_def_space) {
                $tree
                  = $self->gdt('@tie{}-- {category} on {class}: {type} {name}{arguments}',
                               $strings);
              } else {
                $tree
                  = $self->gdt('@tie{}-- {category} on {class}: {type} {name} {arguments}',
                               $strings);
              }
            }
          } else {
            my $strings = {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'type' => $element->{'extra'}->{'def_parsed_hash'}->{'type'},
                    'class' => $element->{'extra'}->{'def_parsed_hash'}->{'class'},
                    'name' => $name};
            if ($self->get_conf('deftypefnnewline') eq 'on') {
              $tree
                = $self->gdt('@tie{}-- {category} on {class}:@*{type}@*{name}',
                             $strings);
            } else {
              $tree
                = $self->gdt('@tie{}-- {category} on {class}: {type} {name}',
                             $strings);
            }
          }
        } elsif ($command eq 'deftypecv') {
          if ($arguments) {
            my $strings = {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'name' => $name,
                    'class' => $element->{'extra'}->{'def_parsed_hash'}->{'class'},
                    'type' => $element->{'extra'}->{'def_parsed_hash'}->{'type'},
                    'arguments' => $arguments};
            if ($omit_def_space) {
              $tree
                = $self->gdt('@tie{}-- {category} of {class}: {type} {name}{arguments}',
                             $strings);
            } else {
              $tree
                = $self->gdt('@tie{}-- {category} of {class}: {type} {name} {arguments}',
                             $strings);
            }
          } else {
            my $strings = {
                    'category' => $element->{'extra'}->{'def_parsed_hash'}->{'category'},
                    'type' => $element->{'extra'}->{'def_parsed_hash'}->{'type'},
                    'class' => $element->{'extra'}->{'def_parsed_hash'}->{'class'},
                    'name' => $name};
            $tree
              = $self->gdt('@tie{}-- {category} of {class}: {type} {name}',
                             $strings);
          }
        }

        my $def_paragraph = $self->new_formatter('paragraph', 
         { 'indent_length' => ($self->{'format_context'}->[-1]->{'indent_level'} -1)
                                      * $indent_length,
           'indent_length_next' => (1+$self->{'format_context'}->[-1]->{'indent_level'})
                                      * $indent_length,
           'suppress_styles' => 1
         });
        push @{$self->{'formatters'}}, $def_paragraph;

        # FIXME the whole line is formatted in code here.  In other formats,
        # the category is normal text
        $result .= _convert($self, {'type' => '_code', 'contents' => [$tree]});
        $result .= _count_added($self, $def_paragraph->{'container'},
              Texinfo::Convert::Paragraph::end($def_paragraph->{'container'}));

        pop @{$self->{'formatters'}};
        delete $self->{'text_element_context'}->[-1]->{'counter'};
        $self->{'empty_lines_count'} = 0;
      }
    } elsif ($element->{'type'} eq 'menu_entry') {
      my $entry_name_seen = 0;
      foreach my $arg (@{$element->{'args'}}) {
        my ($pre_quote, $post_quote);
        if ($arg->{'type'} eq 'menu_entry_node') {
          $self->{'formatters'}->[-1]->{'suppress_styles'} = 1;
          $self->{'formatters'}->[-1]->{'no_added_eol'} = 1;

          # Flush a leading space
          $result .= _count_added($self, $formatter->{'container'},
                           add_pending_word($formatter->{'container'}, 1));

          # note that $arg->{'contents'} may be undefined in rare case
          # such as in sectionning in_menu_only_special_ascii_spaces_node
          # test
          my $node_text = _convert($self, {'type' => '_code',
                                      'contents' => $arg->{'contents'}});

          $node_text .= _count_added($self, $formatter->{'container'},
                           add_pending_word($formatter->{'container'}, 1));
          delete $self->{'formatters'}->[-1]->{'suppress_styles'};
          delete $self->{'formatters'}->[-1]->{'no_added_eol'};
          $pre_quote = $post_quote = '';
          if ($entry_name_seen) {
            if ($node_text =~ /([,\t]|\.\s)/) {
              if ($self->{'info_special_chars_warning'}) {
                $self->converter_line_warn($self, sprintf(__(
                   "menu entry node name should not contain `%s'"), $1),
                               $element->{'source_info'});
              }
              if ($self->{'info_special_chars_quote'}) {
                $pre_quote = $post_quote = "\x{7f}";
              }
            }
          } else {
            if ($node_text =~ /:/) {
              if ($self->{'info_special_chars_warning'}) {
                $self->converter_line_warn($self, __(
                 "menu entry node name should not contain `:'"),
                               $element->{'source_info'});
              }
              if ($self->{'info_special_chars_quote'}) {
                $pre_quote = $post_quote = "\x{7f}";
              }
            }
          }
          $result .= $pre_quote . $node_text . $post_quote;
        } elsif ($arg->{'type'} eq 'menu_entry_name') {
          $self->{'formatters'}->[-1]->{'no_added_eol'} = 1;
          my $entry_name = _convert($self, $arg);
          delete $self->{'formatters'}->[-1]->{'no_added_eol'};
          my $formatter = $self->{'formatters'}->[-1];
          $entry_name .= _count_added($self,
                           $formatter->{'container'},
                           add_pending_word($formatter->{'container'}, 1));
          $entry_name_seen = 1;
          $pre_quote = $post_quote = '';
          if ($entry_name =~ /:/) {
            if ($self->{'info_special_chars_warning'}) {
              $self->converter_line_warn($self, __(
                 "menu entry name should not contain `:'"),
                               $element->{'source_info'});
            }
            if ($self->{'info_special_chars_quote'}) {
              $pre_quote = $post_quote = "\x{7f}";
            }
          }
          $result .= $pre_quote . $entry_name . $post_quote;
        } else {
          $result .= _convert($self, $arg);
        }
      }

      # If we are nested inside an @example, a 'menu_entry_description' may not 
      # have been processed yet, and we need to output any pending spaces 
      # before 'end_line' throws them away.  The argument to 'add_pending_word' 
      # does this.
      if ($element->{'parent'}->{'type'}
              and $element->{'parent'}->{'type'} eq 'preformatted') {
        $result .= _count_added($self,
                         $formatter->{'container'},
                         add_pending_word($formatter->{'container'}, 1));
      } else {
        $result .= _count_added($self,
                         $formatter->{'container'},
                         add_pending_word($formatter->{'container'}));
        end_line($formatter->{'container'});
        $result = $self->ensure_end_of_line($result) ;
      }

    } elsif ($element->{'type'} eq 'frenchspacing') {
      push @{$formatter->{'frenchspacing_stack'}}, 'on';
      set_space_protection($formatter->{'container'}, undef, undef, undef, 1);
    } elsif ($element->{'type'} eq '_code') {
      _open_code($formatter);
    } elsif ($element->{'type'} eq 'bracketed') {
      $result .= _count_added($self, $formatter->{'container'}, 
                   add_text($formatter->{'container'}, '{'));
    }
  }

  # The processing of contents is done here.
  # $element->{'contents'} undef may happen for some empty commands/containers
  if ($element->{'contents'}) {
    my @contents = @{$element->{'contents'}};
    push @{$self->{'current_contents'}}, \@contents;
    push @{$self->{'current_roots'}}, $element;
    while (@contents) {
      my $content = shift @contents;
      my $text = _convert($self, $content);
      $self->{'empty_lines_count'} = 0 
        if ($preformatted and $text =~ /\S/);
      $result .= $text;
    }
    pop @{$self->{'current_contents'}};
    pop @{$self->{'current_roots'}};
  }

  # now closing. First, close types.
  if ($element->{'type'}) {
    if ($element->{'type'} eq 'frenchspacing') {
      pop @{$formatter->{'frenchspacing_stack'}};
      my $frenchspacing = 0;
      $frenchspacing = 1 if ($formatter->{'frenchspacing_stack'}->[-1] eq 'on');
      set_space_protection($formatter->{'container'}, undef,
                           undef, undef, $frenchspacing);
    } elsif ($element->{'type'} eq '_code') {
      _close_code($formatter);
    } elsif ($element->{'type'} eq 'bracketed') {
      $result .= _count_added($self, $formatter->{'container'}, 
                                     add_text($formatter->{'container'}, '}'));
    } elsif ($element->{'type'} eq 'row') {
      my @cell_beginnings;
      my @cell_lines;
      my $cell_beginning = 0;
      my $cell_idx = 0;
      my $max_lines = 0;
      my $indent_len = $indent_length 
             * $self->{'format_context'}->[-1]->{'indent_level'};
      foreach my $cell (@{$self->{'format_context'}->[-1]->{'row'}}) {
        $cell_beginnings[$cell_idx] = $cell_beginning;
        my $cell_width = $self->{'format_context'}->[-1]->{'columns_size'}->[$cell_idx];
        $cell_beginning += $cell_width +1;
        $cell_lines[$cell_idx] = [ split /^/, $cell ];
        $max_lines = scalar(@{$cell_lines[$cell_idx]}) 
          if (scalar(@{$cell_lines[$cell_idx]}) > $max_lines);
        $cell_idx++;
      }

      $cell_idx = 0;
      my $cell_updated_locations = [];
      my @row_locations;
      foreach my $cell_locations (@{$self->{'format_context'}->[-1]->{'row_counts'}}) {
        foreach my $location (@{$cell_locations->{'locations'}}) {
          next unless (defined($location->{'bytes'}) and defined($location->{'lines'}));
          $cell_updated_locations->[$cell_idx] = {}
            if (!$cell_updated_locations->[$cell_idx]);
          push @{$cell_updated_locations->[$cell_idx]->{$location->{'lines'}}},
                 $location;
          $max_lines = $location->{'lines'}+1 
                            if ($location->{'lines'}+1 > $max_lines);
        }
        push @row_locations, @{$cell_locations->{'locations'}};
        $cell_idx++;
      }

      # this is used to keep track of the last cell with content.
      my $max_cell = scalar(@{$self->{'format_context'}->[-1]->{'row'}});
      my $bytes_count = 0;
      my $line;
      for (my $line_idx = 0; $line_idx < $max_lines; $line_idx++) {
        my $line_width = $indent_len;
        $line = '';
        # determine the last cell in the line, to fill spaces in 
        # cells preceding that cell on the line
        my $last_cell = 0;
        for (my $cell_idx = 0; $cell_idx < $max_cell; $cell_idx++) {
          $last_cell = $cell_idx+1 if (defined($cell_lines[$cell_idx]->[$line_idx])
                                       or ($cell_updated_locations->[$cell_idx]
                                           and defined($cell_updated_locations->[$cell_idx]->{$line_idx})));
        }

        for (my $cell_idx = 0; $cell_idx < $last_cell; $cell_idx++) {
          my $cell_text = $cell_lines[$cell_idx]->[$line_idx];
          if (defined($cell_text)) {
            chomp($cell_text);
            if ($line eq '' and $cell_text ne '') {
              $line = ' ' x $indent_len;
              $bytes_count += count_bytes($self, $line);
            }
            $line .= $cell_text;
            $bytes_count += count_bytes($self, $cell_text);
            $line_width += Texinfo::Convert::Unicode::string_width($cell_text);
          }
          if ($cell_updated_locations->[$cell_idx]
              and defined($cell_updated_locations->[$cell_idx]->{$line_idx})) {
            foreach my $location (@{$cell_updated_locations->[$cell_idx]->{$line_idx}}) {
              $location->{'bytes'} = $bytes_count;
            }
          }
          if ($cell_idx+1 < $last_cell) {
            if ($line_width < $indent_len + $cell_beginnings[$cell_idx+1]) {
              if ($line eq '') {
                $line = ' ' x $indent_len;
                $bytes_count += count_bytes($self, $line);
              }
              my $spaces = ' ' x ($indent_len + $cell_beginnings[$cell_idx+1] - $line_width);
              $line_width += Texinfo::Convert::Unicode::string_width($spaces);
              $line .= $spaces;
              $bytes_count += count_bytes($self, $spaces);
            }
          }
        }
        $line .= "\n";
        $bytes_count += count_bytes($self, "\n");
        $result .= $line;
      }
      if ($self->{'format_context'}->[-1]->{'item_command'} eq 'headitem') {
        # at this point cell_beginning is at the beginning of
        # the cell following the end of the table -> full width
        my $line = ' ' x $indent_len . '-' x $cell_beginning . "\n";
        $bytes_count += count_bytes($self, $line);
        $result .= $line;
        $self->{'empty_lines_count'} = 0;
        $max_lines++;
      # there may be empty lines, in that case $line is undef, $max_lines == 0
      } elsif ($max_lines) {
        if ($line eq "\n") {
          $self->{'empty_lines_count'} = 1;
        } else {
          $self->{'empty_lines_count'} = 0;
        }
      }
      $self->_update_locations_counts(\@row_locations);
      push @{$self->{'count_context'}->[-1]->{'locations'}}, @row_locations;
      $self->{'count_context'}->[-1]->{'bytes'} += $bytes_count;
      $self->{'count_context'}->[-1]->{'lines'} += $max_lines;
      $self->{'format_context'}->[-1]->{'row'} = [];
      $self->{'format_context'}->[-1]->{'row_counts'} = [];
      $self->{'format_context'}->[-1]->{'row_empty_lines_count'} 
        = $self->{'empty_lines_count'};
    } elsif ($element->{'type'} eq 'before_node_section') {
      $self->{'text_before_first_node'} = $result;
    }
  }
  # close paragraphs and preformatted
  if ($paragraph) {
    $result .= _count_added($self, $paragraph->{'container'},
               Texinfo::Convert::Paragraph::end($paragraph->{'container'}));
    if ($self->{'context'}->[-1] eq 'flushright') {
      $result = $self->_align_environment($result, 
        $self->{'text_element_context'}->[-1]->{'max'}, 'right');
    }
    pop @{$self->{'formatters'}};
    delete $self->{'text_element_context'}->[-1]->{'counter'};
  } elsif ($preformatted) {
    $result .= _count_added($self, $preformatted->{'container'},
               Texinfo::Convert::Paragraph::end($preformatted->{'container'}));
    if ($result ne '') {
      $result = $self->ensure_end_of_line($result);
    }
    if ($self->{'context'}->[-1] eq 'flushright') {
      $result = $self->_align_environment ($result, 
        $self->{'text_element_context'}->[-1]->{'max'}, 'right');
    }
    pop @{$self->{'formatters'}};
    # We assume that, upon closing the preformatted we are at the 
    # beginning of a line.
    delete $self->{'text_element_context'}->[-1]->{'counter'};
  }

  # close commands
  if ($command) {
    if ($command eq 'float') {
      if ($element->{'extra'}
          and (($element->{'extra'}->{'type'}
                and $element->{'extra'}->{'type'}->{'normalized'} ne '')
               or ($element->{'structure'}
                   and defined($element->{'structure'}->{'float_number'}))
               or $element->{'extra'}->{'caption'} or $element->{'extra'}->{'shortcaption'})) {
        
        $result .= _add_newline_if_needed($self);
        my ($caption, $prepended)
             = Texinfo::Convert::Converter::float_name_caption($self, $element);
        if ($prepended) {
          $prepended->{'type'} = 'frenchspacing';
          my $float_number = $self->convert_line ($prepended);
          $result .= $float_number;
          $self->{'text_element_context'}->[-1]->{'counter'} +=
            Texinfo::Convert::Unicode::string_width($float_number);
          $self->{'empty_lines_count'} = 0;
        }
        if ($caption) {
          $self->{'format_context'}->[-1]->{'paragraph_count'} = 0;
          my $tree = $caption->{'args'}->[0];
          $result .= _convert($self, $tree);
        }
      }
    } elsif (($command eq 'quotation'
               or $command eq 'smallquotation')
             and $element->{'extra'} and $element->{'extra'}->{'authors'}) {
      foreach my $author (@{$element->{'extra'}->{'authors'}}) {
        # this cannot happen as this should be caugth by 'missing_argument'
        # but it is more robust to check anyway
        if ($author->{'args'}->[0]
            and $author->{'args'}->[0]->{'contents'}) {
          $result .= _convert($self,
                   $self->gdt("\@center --- \@emph{{author}}\n",
                      {'author' => $author->{'args'}->[0]->{'contents'}}));
        }
      }
    } elsif (($command eq 'multitable')) {
      $self->{'document_context'}->[-1]->{'in_multitable'}--;
    } elsif ($root_commands{$command}
        and $sectioning_heading_commands{$command}
        and $command ne 'part') {
      # add menu if missing
      my $node = $self->{'current_node'};
      my $automatic_directions;
      if ($node and defined($node->{'extra'}->{'nodes_manuals'})) {
        $automatic_directions =
            (scalar(@{$node->{'extra'}->{'nodes_manuals'}}) == 1);
      }
      if ($node and $automatic_directions
            and !$self->{'seenmenus'}->{$node}) {
        $self->{'seenmenus'}->{$node} = 1;
        my $menu_node = Texinfo::Structuring::new_complete_node_menu($node);
        if ($menu_node) {
          my $menu_text = $self->_convert($menu_node);
          if ($menu_text) {
            $result .= $menu_text;
          }
        }
      }
    }

    # close the contexts and register the cells
    if ($self->{'preformatted_context_commands'}->{$command}
        or $command eq 'float') {
      my $old_context = pop @{$self->{'context'}};
      die "Not a preformatted context: $old_context"
        if (!$self->{'preformatted_context_commands'}->{$old_context}
            and $old_context ne 'float');
      if ($old_context ne 'float' and !$menu_commands{$old_context}) {
        $self->{'empty_lines_count'} = 0;
      }
      delete ($self->{'preformatted_context_commands'}->{$command})
       unless ($default_preformatted_context_commands{$command});
    } elsif ($flush_commands{$command}) {
      my $old_context = pop @{$self->{'context'}};
      die if (! $flush_commands{$old_context});
    }

    if ($self->{'format_context_commands'}->{$command}) {
      pop @{$self->{'format_context'}};
      delete ($self->{'format_context_commands'}->{$command})
       unless ($default_format_context_commands{$command});
    } elsif ($cell) {
      pop @{$self->{'format_context'}};
      pop @{$self->{'text_element_context'}};
      push @{$self->{'format_context'}->[-1]->{'row'}}, $result;
      update_count_context($self);
      my $cell_counts = pop @{$self->{'count_context'}};
      push @{$self->{'format_context'}->[-1]->{'row_counts'}}, $cell_counts;
      $result = '';
    }
    if ($advance_paragraph_count_commands{$command}) {
      $self->{'format_context'}->[-1]->{'paragraph_count'}++;
    }
  }

  return $result;
}

1;

__END__
# Automatically generated from maintain/template.pod

=head1 NAME

Texinfo::Convert::Plaintext - Convert Texinfo tree to Plaintext

=head1 SYNOPSIS

  my $converter
    = Texinfo::Convert::Plaintext->converter({'parser' => $parser});

  $converter->output($tree);
  $converter->convert($tree);
  $converter->convert_tree($tree);

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

Texinfo::Convert::Plaintext converts a Texinfo tree to Plaintext.

=head1 METHODS

=over

=item $converter = Texinfo::Convert::Plaintext->converter($options)

Initialize converter from Texinfo to Plaintext.

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
